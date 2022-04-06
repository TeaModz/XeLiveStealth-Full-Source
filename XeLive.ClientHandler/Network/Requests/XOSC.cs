using System;
using System.IO;
using System.Collections.Generic;

namespace XeLive.ClientHandler {
    class PacketXOSC {
        public static byte[,] fuses = new byte[7, 0x10] {
            { 0xc0, 0xdc, 0xfe, 0xf3, 0xd7, 0x3e, 0xed, 0x7e, 0x5a, 0xf8, 0xb1, 0xbb, 0xb2, 0xe0, 0x26, 0x95 }, // Xenon
	        { 0x96, 0x23, 0x74, 0x9c, 0x9e, 0xc5, 0x2b, 0x30, 0xc6, 0x68, 0x05, 0x9e, 0xad, 0x9c, 0x12, 0xa8 }, // Zephyr
	        { 0x82, 0xc1, 0xf0, 0x00, 0x9e, 0x79, 0x97, 0xf3, 0x34, 0x0e, 0x01, 0x45, 0x1a, 0xd0, 0x32, 0x57 }, // Falcon
	        { 0x3a, 0x5b, 0x47, 0xd6, 0xdd, 0x5a, 0xf8, 0x66, 0x93, 0xed, 0x05, 0x47, 0x25, 0x66, 0x15, 0x69 }, // Jasper
	        { 0xdb, 0xe6, 0x35, 0x87, 0x78, 0xcb, 0xfc, 0x2f, 0x52, 0xa3, 0xba, 0xf8, 0x92, 0x45, 0x8d, 0x65 }, // Trinity
	        { 0xd1, 0x32, 0xfb, 0x43, 0x9b, 0x48, 0x47, 0xe3, 0x9f, 0xe5, 0x46, 0x46, 0xf0, 0xa9, 0x9e, 0xb1 }, // Corona
	        { 0xd1, 0x32, 0xfb, 0x43, 0x9b, 0x48, 0x47, 0xe3, 0x9f, 0xe5, 0x46, 0x46, 0xf0, 0xa9, 0x9e, 0xb1 }  // Winchester
        };
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketXOSC", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            long hvProtectedFlags = reader.ReadInt64();
            int motherboardIndex = reader.ReadInt32();
            byte[] kv_xeIkaCertificateInquiryData = reader.ReadBytes(0x24);
            byte[] kv_consoleSerialNumber = reader.ReadBytes(0xC);
            byte[] kv_consoleCertificateAbData = reader.ReadBytes(0x5);
            byte[] cpuKeyDigest = reader.ReadBytes(0x10);
            ushort kv_oddFeatures = reader.ReadUInt16();
            bool typeOneKv = reader.ReadBoolean();
            uint kv_policyFlashSize = reader.ReadUInt32();
            bool fcrt = reader.ReadBoolean();
            uint titleID = reader.ReadUInt32();
            uint mediaID = reader.ReadUInt32();

            byte[] xosc = reader.ReadBytes(0x2C8);
            byte[] temp = new byte[0x100];

            for (int i = 0; i < 0x10; i++) { temp[i] = fuses[motherboardIndex, i]; }
            Buffer.BlockCopy(temp, 0, xosc, 0x70, 0x10);
            Buffer.BlockCopy(cpuKeyDigest, 0, xosc, 0x50, 0x10);

            Buffer.BlockCopy(kv_xeIkaCertificateInquiryData, 0, xosc, 0xF0, 0x24);
            Buffer.BlockCopy(kv_xeIkaCertificateInquiryData, 0, xosc, 0x114, 0x24);

            Buffer.BlockCopy(kv_consoleSerialNumber, 0, xosc, 0x138, 0xC);

            for (int i = 0; i < 0x30; i++) { xosc[0x1A0 + i] = 0x0; }

            Buffer.BlockCopy(kv_consoleCertificateAbData, 0, xosc, 0x1A0, 0x5);

            for (int i = 0; i < 0x14; i++) { xosc[0x1D4 + i] = 0x0; }

            for (int i = 0; i < 0x8; i++) { xosc[0x1E8 + i] = 0x0; }

            for (int i = 0; i < 0x28; i++) { xosc[0x1F0 + i] = 0x0; }

            for (int i = 0; i < 0x4; i++) { xosc[0x218 + i] = 0x0; }

            byte[] bytes = BitConverter.GetBytes(kv_oddFeatures); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x14A, 0x2);

            for (int i = 0; i < 0x8; i++) { xosc[0x180 + i] = 0x0; }

            bytes = BitConverter.GetBytes(typeOneKv ? 0 : kv_policyFlashSize); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x150, 0x4);

            bytes = BitConverter.GetBytes(6); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x2C4, 0x4);

            bytes = BitConverter.GetBytes((ulong)0x00000000000001BF); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x08, 0x8);

            bytes = BitConverter.GetBytes(0x40000012); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x34, 0x4);

            bytes = BitConverter.GetBytes((ushort)0xD83E); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x146, 0x2);

            bytes = BitConverter.GetBytes(fcrt ? 0x033389D3 : 0x023389D3); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x158, 0x4);

            bytes = BitConverter.GetBytes(0x40000207); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x1D0, 0x4);

            if (!typeOneKv)
            {
                bytes = BitConverter.GetBytes(0x00033840); Array.Reverse(bytes);
                Buffer.BlockCopy(bytes, 0, xosc, 0x2B4, 0x4);
            }

            bytes = BitConverter.GetBytes(0x00200000); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x2B8, 0x4);

            if (titleID == 0 || titleID == 0xFFFFFFFF || mediaID == 0xFFFFFFFF || titleID == 0xFFFF0055 || titleID == 0xFFFE07FF || titleID == 0xF5D10000)
            {
                titleID = 0xFFFE07D1;

                bytes = BitConverter.GetBytes(0); Array.Reverse(bytes);
                Buffer.BlockCopy(bytes, 0, xosc, 0x84, 0x4);

                bytes = BitConverter.GetBytes(0xFFFE07D1); Array.Reverse(bytes);
                Buffer.BlockCopy(bytes, 0, xosc, 0x88, 0x4);
            }

            if (titleID != 0xFFFE07D1)
            {
                hvProtectedFlags = 4 | (hvProtectedFlags);
            }
            else
            {
                hvProtectedFlags = 4 | (hvProtectedFlags & 1);
            }

            bytes = BitConverter.GetBytes(hvProtectedFlags); Array.Reverse(bytes);
            Buffer.BlockCopy(bytes, 0, xosc, 0x198, 0x8);

            ClientInfo client = new ClientInfo();
            if (MySQL.GetClientData(Utils.BytesToString(header.szConsoleKey), ref client))
            {
                MySQL.IncrementChallengeCount(Utils.BytesToString(header.szConsoleKey));
            }

            serverWriter.Write(xosc);
            serverWriter.Close();
            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
