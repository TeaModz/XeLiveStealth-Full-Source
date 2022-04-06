using System;
using System.IO;
using System.Collections.Generic;

namespace XeLive.ClientHandler {
    class PacketConnect {
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip)
        {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketConnect", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            // the buffer that the resp is written into
            byte[] resp = new byte[1];

            EndianWriter writer = new EndianWriter(new MemoryStream(resp), EndianStyle.BigEndian);

            writer.Write(true);
            writer.Close();

            serverWriter.Write(resp);
            serverWriter.Close();
            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
