using System;
using System.Collections.Generic;
using System.Net;
using Newtonsoft.Json.Linq;

namespace XeLive.ClientHandler {
    class PacketHeartbeat {
        private enum eHeartbeatPacketStatus {
            STATUS_SUCCESS = 1,
            STATUS_ERROR
        }
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketHeartbeat", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            eHeartbeatPacketStatus status = eHeartbeatPacketStatus.STATUS_SUCCESS;

            int xex = reader.ReadInt32();
            uint currentTitle = reader.ReadUInt32();
            uint kvhash = reader.ReadUInt32();
            bool kvbanned = reader.ReadBoolean();
            char[] gamertag = reader.ReadChars(16);

            ClientInfo client = new ClientInfo();
            if (MySQL.GetClientData(Utils.BytesToString(header.szConsoleKey), ref client)) {
                string gt = new string(gamertag);

                Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Gamertag: {0}", gt), ip);
                MySQL.UpdateUserGamertag(client, gt);

                switch (client.Status) {
                    case ClientInfoStatus.Banned:
                    case ClientInfoStatus.Disabled:
                        status = eHeartbeatPacketStatus.STATUS_ERROR;
                        goto end;
                }
            }
            else {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", "Failed to resolve client info", ip);
                status = eHeartbeatPacketStatus.STATUS_ERROR;
                goto end;
            }

            XexInfo xeinfo = new XexInfo();
            if (!MySQL.GetXexInfo(xex, ref xeinfo)) {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", string.Format("Xex identifier not found ({0})", xex), ip);
                status = eHeartbeatPacketStatus.STATUS_ERROR;
                goto end;
            }

            MySQL.UpdateKVStat(kvhash.ToString("X4"), kvbanned);

            if (kvbanned) {
                Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("KV hash: {0} is BANNED!", kvhash.ToString("X4")), ip);
            }

            Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Running title {0}", currentTitle.ToString("X8")), ip);
        end:
            serverWriter.Write((int)status);
            serverWriter.Close();

            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
