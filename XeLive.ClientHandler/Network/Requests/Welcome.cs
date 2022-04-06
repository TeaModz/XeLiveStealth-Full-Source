using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XeLive.ClientHandler {
    class PacketWelcome {
        private enum eWelcomePacketStatus {
            WELCOME_STATUS_SUCCESS = 1,
            WELCOME_STATUS_REQUIRED_UPDATE,
            WELCOME_STATUS_DISABLED,
            WELCOME_STATUS_BANNED,
            WELCOME_STATUS_ERROR
        }

        public static void Handle(EndianReader reader, EndianWriter serverWriter, Header header, List<Log.PrintQueue> logId, string ip) {
            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketWelcome", ip);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToString(header.szConsoleKey), ip);

            byte[] resp = new byte[20];

            ClientInfo client = new ClientInfo();
            KVStats stats = new KVStats();
            int daysOnKV = 1;
            int lastUsedVersion = 0;

            EndianWriter writer = new EndianWriter(new MemoryStream(resp), EndianStyle.BigEndian);

            eWelcomePacketStatus status = eWelcomePacketStatus.WELCOME_STATUS_SUCCESS;

            int xex = reader.ReadInt32();
            int userVersion = reader.ReadInt32();
            uint kvhash = reader.ReadUInt32();
            bool kvbanned = reader.ReadBoolean();

            XexInfo xeinfo = new XexInfo();
            if (!MySQL.GetXexInfo(xex, ref xeinfo)) {
                Log.Add(logId, ConsoleColor.DarkYellow, "Reporting", string.Format("Xex identifier not found ({0})", xex), ip);

                status = eWelcomePacketStatus.WELCOME_STATUS_ERROR;
                goto end;
            }

            if (userVersion != xeinfo.dwLastVersion) {
                // needs an update
                status = eWelcomePacketStatus.WELCOME_STATUS_REQUIRED_UPDATE;

                goto end;
            }

            if (MySQL.GetClientData(Utils.BytesToString(header.szConsoleKey), ref client))
            {
                switch (client.Status)
                {
                    case ClientInfoStatus.Banned:
                        status = eWelcomePacketStatus.WELCOME_STATUS_BANNED;
                        break;
                    case ClientInfoStatus.Disabled:
                        status = eWelcomePacketStatus.WELCOME_STATUS_DISABLED;
                        break;
                    case ClientInfoStatus.Authed:
                        status = eWelcomePacketStatus.WELCOME_STATUS_SUCCESS;
                        break;
                }

                lastUsedVersion = client.iLastUsedVersion;

                // update stuff
                MySQL.UpdateUserInfoWelcomePacket(Utils.BytesToString(header.szConsoleKey), kvhash.ToString("X4"), ip);
            }
            else
            {
                MySQL.AddUserWelcomePacket(Utils.BytesToString(header.szConsoleKey), Utils.BytesToString(header.szCPU), ip, kvhash.ToString("X4"));
                status = eWelcomePacketStatus.WELCOME_STATUS_SUCCESS;
            }

            if (MySQL.GetKVStats(kvhash.ToString("X4"), ref stats))
            {
                // update shit
                MySQL.UpdateKVStat(kvhash.ToString("X4"), kvbanned);
            }
            else
            {
                MySQL.AddKVStat(kvhash.ToString("X4"), (int)Utils.GetTimeStamp(), (int)Utils.GetTimeStamp(), kvbanned, kvbanned ? (int)Utils.GetTimeStamp() : 0);
            }

            // update stats
            MySQL.GetKVStats(kvhash.ToString("X4"), ref stats);

            int daysOnKVDifference = (int)Utils.GetTimeStamp() - stats.iFirstConnection;
            if (daysOnKVDifference > 86400)
            {
                daysOnKV = (int)Math.Round((float)(daysOnKVDifference / 86400), 0);
            }

            Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("KV hash: {0}, banned: {1}, days: {2}", kvhash.ToString("X4"), kvbanned ? "yes" : "no", daysOnKV), ip);

        end:
            serverWriter.Write((int)status);
            serverWriter.Write(client.iTotalChallenges);
            serverWriter.Write(stats.iTotalChallenges);
            serverWriter.Write(lastUsedVersion);
            serverWriter.Write(daysOnKV);
            serverWriter.Close();

            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", ip);
            Log.Print(logId);
        }
    }
}
