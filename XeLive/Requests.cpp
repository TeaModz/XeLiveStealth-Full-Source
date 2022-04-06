#include "stdafx.h"

int Requests::iPluginSize;

void Requests::PopulateHeader(Request::Header* header, Packets packet, int size) {
	header->iSize = size;
	header->Command = packet;
	header->bDevkit = XeLive.bDevkit;

	header->bCPUEncryptionKey = rand() % 254;
	header->bHypervisorCPUEncryptionKey = rand() % 254;
	header->bConsoleKeyEncryptionKey = rand() % 254;

	memcpy(header->szCPU, Native::HvGetFuseCPU(), 0x10);
	memcpy(header->szHypervisorCPU, Native::HvGetHypervisorCPU(), 0x10);
	memcpy(header->szConsoleKey, XeLive.szConsoleKey, 0x14);

	for (int i = 0; i < sizeof(header->szCPU); i++) {
		header->szCPU[i] ^= header->bCPUEncryptionKey;
	}

	for (int i = 0; i < sizeof(header->szHypervisorCPU); i++) {
		header->szHypervisorCPU[i] ^= header->bHypervisorCPUEncryptionKey;
	}

	for (int i = 0; i < sizeof(header->szConsoleKey); i++) {
		header->szConsoleKey[i] ^= header->bConsoleKeyEncryptionKey;
	}
}

BOOL Requests::PacketConnect() {
	LOG_DEV("Running PacketConnect...");

	XNADDR xbXnaddr;
	DWORD dwAttempts = 0;

	// wait for the console to get a IP address
	while (XNetGetTitleXnAddr(&xbXnaddr) == XNET_GET_XNADDR_PENDING || XNetGetEthernetLinkStatus() == 0) {
		if (dwAttempts > 50)
			break;

		Native::Sleep(Native::DecryptDWORD(0x8E405 /*250*/));
		dwAttempts++;
	}

	if (XNetGetTitleXnAddr(&xbXnaddr) == XNET_GET_XNADDR_PENDING || XNetGetEthernetLinkStatus() == 0) {
		return FALSE;
	}

	Request::ServerPacketConnect* packetConnect = (Request::ServerPacketConnect*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketConnect));
	PopulateHeader(packetConnect, PACKET_CONNECT, sizeof(Request::ServerPacketConnect));
	Response::ServerPacketConnect* packetConnectResponse = (Response::ServerPacketConnect*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketConnect));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetConnect);
		Native::XEncryptedFree(packetConnectResponse);
	};

	if (Network::Connect()) {
		if (Network::Process(packetConnect, sizeof(Request::ServerPacketConnect), packetConnectResponse, sizeof(Response::ServerPacketConnect), FALSE)) {
			if (packetConnectResponse->bConnected) {
				cleanup();
				return TRUE;
			}
		}
	}

	cleanup();
	return FALSE;
}

BOOL Requests::PacketWelcome() {
    LOG_DEV("Running PacketWelcome...");

	Request::ServerPacketWelcome* packetWelcome = (Request::ServerPacketWelcome*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketWelcome));
	PopulateHeader(packetWelcome, PACKET_WELCOME, sizeof(Request::ServerPacketWelcome));
	Response::ServerPacketWelcome* packetWelcomeResponse = (Response::ServerPacketWelcome*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketWelcome));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetWelcome);
		Native::XEncryptedFree(packetWelcomeResponse);
	};

    DWORD kvX, kvStatus;
    XNetLogonGetExtendedStatus(&kvX, &kvStatus);

    LOG_DEV("KV Status: %X", kvStatus);

	packetWelcome->bKVBanned = kvStatus == 0x8015190D;
	packetWelcome->iXexID = 1;
	packetWelcome->iXexVersion = CURRENT_XEX_SERVER_BUILD;
	packetWelcome->dwKVHash = Keyvault::GetKeyvaultHash();

	if (Network::Connect()) {
		if (Network::Process(packetWelcome, sizeof(Request::ServerPacketWelcome), packetWelcomeResponse, sizeof(Response::ServerPacketWelcome), FALSE)) {
			XeLive.iTotalChallengesPassed = packetWelcomeResponse->iTotalChallengesPassed;
			XeLive.iTotalChallengesPassedOnThisKV = packetWelcomeResponse->iTotalChallengesPassedOnCurrentKV;
			XeLive.iTotalDaysOnKV = packetWelcomeResponse->iTotalDaysOnKV;
			XeLive.iLastUsedVersion = packetWelcomeResponse->iLastUsedVersion;

			switch (packetWelcomeResponse->Status) {
				case Response::WELCONE_STATUS_SUCCESS: {
					cleanup();
					return TRUE;
				}

				case Response::WELCONE_STATUS_ERROR: {

					Notify("XeLive - An unknown error occured!").Message();

					cleanup();
					return FALSE;
				}

				case Response::WELCONE_STATUS_BANNED: {
				case Response::WELCONE_STATUS_DISABLED:

					XeLive.bAccountBanned = true;

					strcpy(XeLive.szAccountBannedMessage, "You got banned from XeLive!");

					Notify("You got banned from XeLive!").Message();
					cleanup();
					return FALSE;
				}

				case Response::WELCONE_STATUS_REQUIRED_UPDATE: {
					cleanup();
					return TRUE;
				}
			}
		}
	}

    cleanup();
    return FALSE;
}

BOOL Requests::PacketXOSC(Request::ServerPacketXOSC* packetXOSC, Response::ServerPacketXOSC* packetXOSCResponse) {
	LOG_DEV("Running PacketXOSC...");

	PopulateHeader(packetXOSC, PACKET_XOSC, sizeof(Request::ServerPacketXOSC));

	if (Network::Connect()) {
		if (Network::Process(packetXOSC, sizeof(Request::ServerPacketXOSC), packetXOSCResponse, sizeof(Response::ServerPacketXOSC), FALSE)) {
			XeLive.iTotalChallengesPassedOnThisKV++;
			XeLive.iTotalChallengesPassed++;
			Native::XEncryptedFree(packetXOSC);
			return TRUE;
		}
	}

	Native::XEncryptedFree(packetXOSC);
	Native::XEncryptedFree(packetXOSCResponse);

	return FALSE;
}

BOOL Requests::PacketMetric(Metric* pMetric, const char* pExtraInfo) {
	if (!pMetric) return FALSE;
	LOG_DEV("Running PacketMetric...");

	Request::ServerPacketMetric* packetGetMetric = (Request::ServerPacketMetric*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketMetric));
	PopulateHeader(packetGetMetric, PACKET_METRIC, sizeof(Request::ServerPacketMetric));
	Response::ServerPacketMetric* packetGetMetricResponse = (Response::ServerPacketMetric*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketMetric));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetGetMetric);
		Native::XEncryptedFree(packetGetMetricResponse);
	};

	packetGetMetric->bHasInfo = false;
	packetGetMetric->Metric = pMetric->GetIndex();
	packetGetMetric->MetricType = pMetric->GetType();

	if (pExtraInfo && strlen(pExtraInfo) > 1) {
		packetGetMetric->bHasInfo = true;
		strcpy(packetGetMetric->szExtraInfo, pExtraInfo);
	}

	if (Network::Connect()) {
		if (Network::Process(packetGetMetric, sizeof(Request::ServerPacketMetric), packetGetMetricResponse, sizeof(Response::ServerPacketMetric), FALSE)) {
			cleanup();
			return TRUE;
		}
	}

	cleanup();
	return FALSE;
}

BOOL Requests::PacketGetPlugins() {
	LOG_DEV("Running PacketGetPlugins...");

	Request::ServerPacketsGetPlugins* packetGetPlugins = (Request::ServerPacketsGetPlugins*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketsGetPlugins));
	PopulateHeader(packetGetPlugins, PACKET_GET_PLUGINS, sizeof(Request::ServerPacketsGetPlugins));
	Response::ServerPacketsGetPlugins* packetGetPluginsResponse = (Response::ServerPacketsGetPlugins*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketsGetPlugins));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetGetPlugins);
		Native::XEncryptedFree(packetGetPluginsResponse);
	};

	if (Network::Connect()) {
		if (Network::Process(packetGetPlugins, sizeof(Request::ServerPacketsGetPlugins), packetGetPluginsResponse, sizeof(Response::ServerPacketsGetPlugins), FALSE)) {
			if (packetGetPluginsResponse->bHasPlugins) {
				LOG_DEV("Plugins detected...");

				int count = *(int*)packetGetPluginsResponse->szPluginBuffer;
				ServerXexInfo* info = (ServerXexInfo*)(packetGetPluginsResponse->szPluginBuffer + 4);

				XeLive.PluginData.clear();

				for (int i = 0; i < count; i++) {
					LOG_DEV("#### PLUGIN INFO ####");
					LOG_DEV("\t> ID: %i", info[i].iID);
					LOG_DEV("\t> Last Version: %i", info[i].dwLastVersion);
					LOG_DEV("\t> Title: 0x%X", info[i].dwTitle);
					LOG_DEV("\t> Title Timestamp: %i", info[i].dwTitleTimestamp);
					LOG_DEV("\t> Enabled: %s", info[i].bEnabled ? "YES" : "NO");

					XeLive.PluginData.push_back(info[i]);
				}
			}
			else {
				LOG_DEV("No plugins detected :(");
			}

			cleanup();
			return TRUE;
		}
	}

	cleanup();
	return FALSE;
}

BOOL Requests::PacketDownloadPlugin(int iPluginID, DWORD* pAddress, DWORD* pSize) {
	LOG_DEV("Running PacketGetPlugins...");

	Request::ServerPacketDownloadPlugin* packetDownloadPlugin = (Request::ServerPacketDownloadPlugin*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketDownloadPlugin));
	PopulateHeader(packetDownloadPlugin, PACKET_DOWNLOAD_PLUGIN, sizeof(Request::ServerPacketDownloadPlugin));
	Response::ServerPacketDownloadPlugin_1* packetDownloadPluginResponse = (Response::ServerPacketDownloadPlugin_1*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketDownloadPlugin_1));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetDownloadPlugin);
		Native::XEncryptedFree(packetDownloadPluginResponse);
	};

	packetDownloadPlugin->bOnlySize = true;
	packetDownloadPlugin->iPluginID = iPluginID;

	if (Network::Connect()) {
		if (Network::Process(packetDownloadPlugin, sizeof(Request::ServerPacketDownloadPlugin), packetDownloadPluginResponse, sizeof(Response::ServerPacketDownloadPlugin_1), FALSE)) {
			switch (packetDownloadPluginResponse->Status) {
			case Response::RESPONSE_SUCCESS:
				iPluginSize = packetDownloadPluginResponse->iSize;
				cleanup();
				return PacketDownloadPluginBytes(iPluginID, pAddress, pSize);

			case Response::RESPONSE_ERROR:
				Notify("xbLive - Failed to download plugin!").Message();
				break;
			}
		}
	}

	cleanup();
	return FALSE;
}

BOOL Requests::PacketDownloadPluginBytes(int iPluginID, DWORD* pAddress, DWORD* pSize) {
	LOG_DEV("Running PacketDownloadPluginBytes...");

	Request::ServerPacketDownloadPlugin* packetDownloadPlugin = (Request::ServerPacketDownloadPlugin*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketDownloadPlugin));
	PopulateHeader(packetDownloadPlugin, PACKET_DOWNLOAD_PLUGIN, sizeof(Request::ServerPacketDownloadPlugin));
	Response::ServerPacketDownloadPlugin_2* packetDownloadPluginResponse = (Response::ServerPacketDownloadPlugin_2*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketDownloadPlugin_2) + iPluginSize);

	auto cleanup = [&] {
		Native::XEncryptedFree(packetDownloadPlugin);
		Native::XEncryptedFree(packetDownloadPluginResponse);
	};

	packetDownloadPlugin->bOnlySize = false;
	packetDownloadPlugin->iPluginID = iPluginID;
	
	const char* error;

	if (Network::Connect()) {
		if (Network::Process(packetDownloadPlugin, sizeof(Request::ServerPacketDownloadPlugin), packetDownloadPluginResponse, sizeof(Response::ServerPacketDownloadPlugin_2), FALSE)) {
			switch (packetDownloadPluginResponse->Status) {
			case Response::RESPONSE_SUCCESS: {
				if (iPluginSize > 100) {
					if (pSize) *pSize = iPluginSize;
					BYTE* xexData = (BYTE*)malloc(iPluginSize);

					if (Network::Receive(xexData, iPluginSize)) {
						if (pAddress) *pAddress = (DWORD)xexData;
					}
					else {
						Notify("XeLive - failed to download plugin buffer").Message();
					}
				}
				else {
					error = "XeLive - Failed to load engine, plugin was too small!";
					break;
				}

				cleanup();
				return TRUE;
			}

			case Response::RESPONSE_ERROR:
				error = "XeLive - Failed to download plugin!";
				break;
			}
		}
	}

	if (error) Notify(error).Message(true);
	cleanup();

	return FALSE;
}

void Requests::InitThread() {
	while (true) {
		if (Requests::PacketConnect()) {
			XeLive.bConnectedToServerInit = true;
			break;
		}

		Notify("XeLive - Failed to connect. Check your internet connection!").Message(true);
		Native::Sleep(10000);
	}

	ExitThread(0);
}