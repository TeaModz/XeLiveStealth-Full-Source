using System.Collections.Generic;

namespace XeLive.ClientHandler {
    class Global {
        public static int iPort = 7554;
        public static int iMaximumRequestSize = 2048;
    }

    public struct SocketSpam {
        public long InitialTimestamp;
        public int iConnectionsMade;
        public bool bBanned;
        public long BannedTimestamp;
        public List<long> ConnectionTimestamps;

        public SocketSpam(long init, int con, bool banned, long bannedInit) {
            InitialTimestamp = init;
            iConnectionsMade = con;
            bBanned = banned;
            BannedTimestamp = bannedInit;
            ConnectionTimestamps = new List<long>();
        }
    }

    enum ePackets {
        PACKET_WELCOME = 1,
        PACKET_CONNECT,
        PACKET_XEKEYSEXECUTE,
        PACKET_XOSC,
        PACKET_METRIC,
        PACKET_GET_PLUGINS,
        PACKET_DOWNLOAD_PLUGIN,
    };

    struct Header {
		public ePackets Command;
		public int iSize;
        public byte bCPUEncryptionKey;
        public byte[] szCPU; // 0x10
        public byte bHypervisorCPUEncryptionKey;
        public byte[] szHypervisorCPU; // 0x10
        public byte bConsoleKeyEncryptionKey;
        public byte[] szConsoleKey; // 0x14 - SPECIAL IDENTIFIER!
        public bool bDevkit;
	}

    public enum ClientInfoStatus
    {
        Authed,
        Banned,
        Disabled
    }

    public struct ClientInfo {
        public int iID;
        public string ConsoleKey;
        public string CPUKey;
        public string FirstGamertag;
        public string LastGamertag;
        public string FirstIP;
        public string LastIP;
        public ClientInfoStatus Status;
        public string FirstKVHash;
        public string LastKVHash;
        public int iLastConnection;
        public int iTotalChallenges;
        public int iLastUsedVersion;
    }

    public struct XexInfo {
        public int iID;
        public string Name;
        public uint dwLastVersion;
        public uint dwTitle;
        public uint dwTitleTimestamp;
        public bool bEnabled;
    }

    public struct KVStats {
        public int iID;
        public string KVHash;
        public int iFirstConnection;
        public int iLastConnection;
        public bool bBanned;
        public int iBannedTime;
        public int iTotalChallenges;
    }
}
