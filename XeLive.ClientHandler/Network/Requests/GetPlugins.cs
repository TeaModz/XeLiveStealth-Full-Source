using System;
using System.IO;
using System.Collections.Generic;

namespace XeLive.ClientHandler {
    class PacketGetPlugins {
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketGetPlugins", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            List<XexInfo> xexInfos = MySQL.GetXexInfos();

            // 16 is current size of xexinfo
            byte[] resp = new byte[0x201];
            byte[] data = new byte[0x200];

            EndianWriter writer = new EndianWriter(new MemoryStream(resp), EndianStyle.BigEndian);
            EndianWriter dataWriter = new EndianWriter(new MemoryStream(data), EndianStyle.BigEndian);

            int realCount = 0;

            foreach (var xex in xexInfos) {
                if (xex.dwTitle != 0) realCount++;
            }

            dataWriter.Write(realCount);

            foreach (var xex in xexInfos) {
                if (xex.dwTitle != 0) {
                    dataWriter.Write(xex.iID);
                    dataWriter.Write(xex.dwLastVersion);
                    dataWriter.Write(xex.dwTitle);
                    dataWriter.Write(xex.dwTitleTimestamp);
                    dataWriter.Write(xex.bEnabled);
                }
            }

            dataWriter.Close();

            Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Sending {0} plugin(s) to client", realCount), ip);

            writer.Write(realCount > 1);
            writer.Write(data);
            writer.Close();

            serverWriter.Write(resp);
            serverWriter.Close();

            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
