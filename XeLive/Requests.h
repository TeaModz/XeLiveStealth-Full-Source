#pragma once

enum Packets {
	PACKET_WELCOME = 1,
	PACKET_CONNECT,
	PACKET_XEKEYSEXECUTE,
	PACKET_XOSC,
	PACKET_METRIC,
	PACKET_GET_PLUGINS,
	PACKET_DOWNLOAD_PLUGIN,
};

namespace Response {
	enum ePacketWelcomeStatus {
		WELCONE_STATUS_SUCCESS = 1,
		WELCONE_STATUS_REQUIRED_UPDATE,
		WELCONE_STATUS_DISABLED,
		WELCONE_STATUS_BANNED,
		WELCONE_STATUS_ERROR
	};

	enum eResponseStatus {
		RESPONSE_ERROR,
		RESPONSE_SUCCESS,
	};

#pragma pack(push)
#pragma pack(1)
	struct ServerPacketConnect {
		bool bConnected;
	};

	struct ServerPacketWelcome {
		ePacketWelcomeStatus Status;
		int iTotalChallengesPassed;
		int iTotalChallengesPassedOnCurrentKV;
		int iLastUsedVersion;
		int iTotalDaysOnKV;
	};

	struct ServerPacketXeKeysExecute {
		DWORD Status;
	};

	struct ServerPacketXOSC {
		BYTE szXOSCBuffer[0x2C8];
	};

	struct ServerPacketMetric {};

	struct ServerPacketsGetPlugins {
		bool bHasPlugins;
		BYTE szPluginBuffer[0x200];
	};

	struct ServerPacketDownloadPlugin_1 {
		eResponseStatus Status;
		int iSize;
		// if sizeOnly was true, it'll have an int for size, otherwise buffer for new xex
	};

	struct ServerPacketDownloadPlugin_2 {
		eResponseStatus Status;
	};
#pragma pack(pop)
}

namespace Request {
#pragma pack(push)
#pragma pack(1)
	struct Header {
		Packets Command;
		int iSize;
		BYTE bCPUEncryptionKey;
		BYTE szCPU[0x10];
		BYTE bHypervisorCPUEncryptionKey;
		BYTE szHypervisorCPU[0x10];
		BYTE bConsoleKeyEncryptionKey;
		BYTE szConsoleKey[0x14];
		bool bDevkit;
	};

	struct ServerPacketConnect : Header {};

	struct ServerPacketWelcome : Header {
		int iXexID; // 1 = stealth, everything after is cheats
		int iXexVersion;
		DWORD dwKVHash;
		bool bKVBanned;
	};

	struct ServerPacketXeKeysExecute : Header {
		BYTE SessionKey[0x10];
		BYTE HVSalt[0x10];
		BYTE ConsolePartNumber[0xB];
		DWORD UpdateSequence;
		BOOL Type1KV;
		BOOL Fcrt;
		BOOL Crl;
	};

	struct ServerPacketXOSC : Header {
		long long HvProtectedFlags;
		int iMotherboardIndex;
		BYTE szXeIkaCertificateInquiryData[0x24];
		BYTE szConsoleSerialNumber[0xC];
		BYTE szConsoleCertificateAbData[0x5];
		BYTE szCpuKeyDigest[0x10];
		WORD wOddFeatures;
		bool bTypeOneKV;
		DWORD dwPolicyFlashSize;
		bool bFcrt;
		DWORD dwTitleID;
		DWORD dwMediaID;
		BYTE szXOSCBuffer[0x2C8];
	};

	struct ServerPacketMetric : Header {
		eMetricType MetricType;
		eMetrics Metric;
		bool bHasInfo;
		char szExtraInfo[0x100];
	};

	struct ServerPacketsGetPlugins : Header {};

	struct ServerPacketDownloadPlugin : Header {
		bool bOnlySize;
		int iPluginID;
	};
#pragma pack(pop)
}

class Requests {
public:
	static void PopulateHeader(Request::Header* header, Packets packet, int size);

	static BOOL PacketConnect();
	static BOOL PacketWelcome();
	static BOOL PacketXOSC(Request::ServerPacketXOSC* packetXOSC, Response::ServerPacketXOSC* packetXOSCResponse);
	static BOOL PacketMetric(Metric* pMetric, const char* pExtraInfo = NULL);
	static BOOL PacketGetPlugins();
	static BOOL PacketDownloadPlugin(int iPluginID, DWORD* pAddress, DWORD* pSize);
	static BOOL PacketDownloadPluginBytes(int iPluginID, DWORD* pAddress, DWORD* pSize);

	static void InitThread();

	static int iPluginSize;
};

