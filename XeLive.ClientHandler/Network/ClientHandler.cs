using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.IO;
using System.Threading;
using System.Collections.Concurrent;
using System.Threading.Tasks;

namespace XeLive.ClientHandler {
    class ClientHandler {
        private TcpListener ListenerHandle;
        private Thread ListenerThread;
        public static Dictionary<string, SocketSpam> SocketSpamConnectionLog = new Dictionary<string, SocketSpam>();

        public ClientHandler()
        {
            ListenerHandle = new TcpListener(IPAddress.Any, Global.iPort);
        }

        public void Start()
        {
            ListenerThread = new Thread(new ThreadStart(() => {
                ListenerHandle.Start();

                while (true)
                {
                    Thread.Sleep(100);
                    if (ListenerHandle.Pending()) new Thread(new ParameterizedThreadStart(Handler)).Start(ListenerHandle.AcceptTcpClient());
                }
            }));
            ListenerThread.Start();
        }

        private void Handler(object client) {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream netStream = tcpClient.GetStream();
            string IP = tcpClient.Client.RemoteEndPoint.ToString().Split(new char[] { ':' })[0];

            EndianWriter serverWriter = new EndianWriter(netStream, EndianStyle.BigEndian);

            if (FirewallBanHandler.SpamDetection(IP)) {
                Console.WriteLine("Socket closed for {0} - SpamDetection was true!", IP);

                serverWriter.Write(0x13371337);
                serverWriter.Write((byte)0x1);
                serverWriter.Close();
                tcpClient.Close();
                return;
            }

            if (!netStream.CanRead) {
                Console.WriteLine("Socket closed for {0} - CanRead was false!", IP);
                tcpClient.Close();
                return;
            }

            try {
                List<Log.PrintQueue> logId = Log.GetQueue();
                Header header = new Header();

                byte[] neededHeaderData = new byte[0x8];
                if (netStream.Read(neededHeaderData, 0, 0x8) != 0x8)
                {
                    Log.Add(logId, ConsoleColor.DarkYellow, "Flag", "Failed to read header start. Banning IP for 1 hour", IP);
                    Log.Print(logId);

                    Utils.AddToFirewall(IP);
                    tcpClient.Close();
                    return;
                }

                EndianReader baseHeaderParse = new EndianReader(new MemoryStream(neededHeaderData), EndianStyle.BigEndian);

                header.Command = (ePackets)(baseHeaderParse.ReadInt32());

                if (!Enum.IsDefined(typeof(ePackets), header.Command))
                {
                    Log.Add(logId, ConsoleColor.DarkYellow, "Flag", "Client sent an invalid packet (" + header.Command + "). Banning IP for 1 hour", IP);
                    Log.Print(logId);

                    Utils.AddToFirewall(IP);
                    tcpClient.Close();
                    return;
                }

                header.iSize = baseHeaderParse.ReadInt32();

                if (header.iSize > Global.iMaximumRequestSize) {
                    Log.Add(logId, ConsoleColor.DarkYellow, "Flag", "Client sent a header size bigger than the max (" + header.iSize + "). Banning IP for 1 hour", IP);
                    Log.Print(logId);

                    Utils.AddToFirewall(IP);
                    tcpClient.Close();
                    return;
                }

                byte[] data = new byte[header.iSize - 8];
                if (netStream.Read(data, 0, header.iSize - 8) != header.iSize - 8) {
                    Log.Add(logId, ConsoleColor.DarkYellow, "Flag", "Failed to read header (" + header.iSize + "). Banning IP for 1 hour", IP);
                    Log.Print(logId);

                    Utils.AddToFirewall(IP);

                    tcpClient.Close();
                    return;
                }

                byte[] untouchedData = new byte[header.iSize - 8];
                Buffer.BlockCopy(data, 0, untouchedData, 0, header.iSize - 8);

                EndianReader dataReader = new EndianReader(new MemoryStream(data), EndianStyle.BigEndian);

                header.bCPUEncryptionKey = dataReader.ReadByte();
                header.szCPU = dataReader.ReadBytes(0x10);
                header.bHypervisorCPUEncryptionKey = dataReader.ReadByte();
                header.szHypervisorCPU = dataReader.ReadBytes(0x10);
                header.bConsoleKeyEncryptionKey = dataReader.ReadByte();
                header.szConsoleKey = dataReader.ReadBytes(0x14);

                header.bDevkit = dataReader.ReadBoolean();

                for (int i = 0; i < 0x10; i++)
                {
                    header.szCPU[i] ^= header.bCPUEncryptionKey;
                    header.szHypervisorCPU[i] ^= header.bHypervisorCPUEncryptionKey;
                }

                for (int i = 0; i < 0x14; i++) {
                    header.szConsoleKey[i] ^= header.bConsoleKeyEncryptionKey;
                }

                if (!header.szCPU.SequenceEqual(header.szHypervisorCPU)) {
                    Log.Add(logId, ConsoleColor.DarkYellow, "Flag", "Client sent a mismatching cpu set (spoofed)", IP);
                    Log.Print(logId);

                    tcpClient.Close();
                    return;
                }

                if (header.Command == ePackets.PACKET_CONNECT) {
                    // handle connection status
                    PacketConnect.Handle(dataReader, serverWriter, header, logId, IP);
                }
                else if (header.Command == ePackets.PACKET_WELCOME) {
                    // check status
                    PacketWelcome.Handle(dataReader, serverWriter, header, logId, IP);
                } else {
                    switch (header.Command) {
                        case ePackets.PACKET_XEKEYSEXECUTE:
                            PacketXeKeysExecute.Handle(dataReader, serverWriter, header, logId, IP);
                            break;

                        case ePackets.PACKET_XOSC:
                            PacketXOSC.Handle(dataReader, serverWriter, header, logId, IP);
                            break;

                        case ePackets.PACKET_DOWNLOAD_PLUGIN:
                            PacketDownloadPlugin.Handle(dataReader, serverWriter, header, logId, IP);
                            break;

                        case ePackets.PACKET_GET_PLUGINS:
                            PacketGetPlugins.Handle(dataReader, serverWriter, header, logId, IP);
                            break;
                    }
                }

            } catch(Exception ex) {
                serverWriter.Close();
                Console.WriteLine("Error: {0} - {1}", ex.Message, ex.StackTrace);
            }

            tcpClient.Close();
        }
    }
}
