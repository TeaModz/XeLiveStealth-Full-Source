using System;
using System.IO;
using System.Collections.Generic;

namespace XeLive.ClientHandler {
    class PacketDownloadPlugin {
        private enum eDownloadPluginPacketStatus {
            STATUS_SUCCESS = 1,
            STATUS_ERROR
        };
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketDownloadPlugin", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            bool sizeOnly = reader.ReadBoolean();
            int pluginID = reader.ReadInt32();

            // the buffer that the resp is written into
            byte[] resp;
            byte[] xexBytes = new byte[0];
            XexInfo xeinfo = new XexInfo();
            ClientInfo info = new ClientInfo();

            // the latest xex size
            int xexSize = 0;

            EndianWriter writer;

            eDownloadPluginPacketStatus status = eDownloadPluginPacketStatus.STATUS_SUCCESS;

            // if it's for devkit and it's requesting a plugin that isn't the stealth.xex
            if (!MySQL.GetXexInfo(pluginID, ref xeinfo))
            {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", string.Format("Xex identifier not found ({0})", pluginID), ip);

                status = eDownloadPluginPacketStatus.STATUS_ERROR;
                goto end;
            }

            if (!xeinfo.bEnabled)
            {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", "Xex isn't enabled!", ip);

                status = eDownloadPluginPacketStatus.STATUS_ERROR;
                goto end;
            }

            MySQL.GetClientData(Utils.BytesToString(header.szConsoleKey), ref info);

            if (File.Exists("Server Data/Plugins/" + xeinfo.Name)) {
                xexBytes = File.ReadAllBytes("Server Data/Plugins/" + xeinfo.Name);
                xexSize = xexBytes.Length;
            }
            else {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", "File wasn't found on server", ip);
                status = eDownloadPluginPacketStatus.STATUS_ERROR;
            }

        end:
            resp = new byte[4 + (sizeOnly ? 4 : xexSize)];

            writer = new EndianWriter(new MemoryStream(resp), EndianStyle.BigEndian);

            writer.Write((int)status);

            if (status == eDownloadPluginPacketStatus.STATUS_SUCCESS) {
                if (sizeOnly) {
                    writer.Write(xexSize);
                    Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Sent xex size to client: {0}", xexSize), ip);
                }
                else {
                    writer.Write(xexBytes);
                    Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Streamed {0} bytes to client", xexSize), ip);
                }
            }

            writer.Close();

            serverWriter.Write(resp);
            serverWriter.Close();
            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
