using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace XeLive.ClientHandler {
    class PacketXeKeysExecute
    {
        public static bool SendAPIRequest(byte[] CPUKey, byte[] HVSalt, byte[] SessionKey, byte[] ConsolePartNumber, byte[] UpdateSequence, bool Type1KV, bool Fcrt, bool Crl, ref byte[] output) {
            try {
                TcpClient client = new TcpClient("94.199.212.46", 1337);
                NetworkStream stream = client.GetStream();

                byte[] data = new byte[0x52];

                BinaryWriter serverWriter = new BinaryWriter(new MemoryStream(data));

                serverWriter.Write(Utils.StringToByteArray("8911EB7DF9DA69822C2850332026532A"));
                serverWriter.Write(CPUKey);
                serverWriter.Write(HVSalt);
                serverWriter.Write(SessionKey);
                serverWriter.Write(ConsolePartNumber);
                serverWriter.Write(UpdateSequence);

                serverWriter.Write(Type1KV);
                serverWriter.Write(Fcrt);
                serverWriter.Write(Crl);

                serverWriter.Close();

                stream.Write(data, 0, data.Length);

                int bytes = stream.Read(output, 0, output.Length);
                if (bytes != 0x100) {
                    stream.Close();
                    client.Close();
                    Console.WriteLine("API Error - Received: {0} instead of 0x100", bytes);
                    return false;
                }

                stream.Close();
                client.Close();

                return true;
            }
            catch (Exception ex) {
                Console.WriteLine(ex.Message);
            }

            return false;
        }
        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketXeKeysExecute", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            byte[] ChallengeResponse = new byte[0x100];

            bool good = false;

            byte[] szSessionToken = reader.ReadBytes(0x10);
            byte[] HVSalt = reader.ReadBytes(0x10);
            byte[] ConsolePartNumber = reader.ReadBytes(0xB);
            byte[] UpdateSequence = reader.ReadBytes(0x4);

            bool Type1KV = reader.ReadBoolean();
            bool fcrt = reader.ReadBoolean();
            bool crl = reader.ReadBoolean();

            serverWriter.Write((uint)0x4A000000);

            ClientInfo client = new ClientInfo();
            if (MySQL.GetClientData(Utils.BytesToString(header.szConsoleKey), ref client)) {
                MySQL.IncrementChallengeCount(Utils.BytesToString(header.szConsoleKey));

                for (int i = 0; i < 5; i++) {
                    if (SendAPIRequest(header.szCPU, HVSalt, szSessionToken, ConsolePartNumber, UpdateSequence, Type1KV, fcrt, crl, ref ChallengeResponse)) {
                        good = true;
                        break;
                    }
                }
            }

            if (good) {
                serverWriter.Write(ChallengeResponse);
                serverWriter.Close();
            }

            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
