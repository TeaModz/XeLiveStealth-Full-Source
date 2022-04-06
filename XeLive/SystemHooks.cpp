#include "stdafx.h"

bool SystemHooks::XexCheckExecutablePrivilegeHook(int priviledge) {
	if (priviledge == 6) return true; // PRIV_INSECURE_SOCKS
	if (priviledge == 0x11) return false; // PRIV_AP25_MEDIA
	return XexCheckExecutablePrivilege(priviledge);
}

HRESULT SystemHooks::XexStartExecutableHook(FARPROC TitleProcessInitThreadProc) {
	auto res = XexStartExecutable(TitleProcessInitThreadProc);
	TitleHooks::RunOnTitleLoad((PLDR_DATA_TABLE_ENTRY)*XexExecutableModuleHandle);
	return res;
}

void SystemHooks::XSecurityCloseProcessHook() {
	return;
}

void SystemHooks::APCWorker(void* Arg1, void* Arg2, void* Arg3) {
	if (Arg2)
		((LPOVERLAPPED_COMPLETION_ROUTINE)Arg2)((DWORD)Arg3, 0, (LPOVERLAPPED)Arg1);
}

int SystemHooks::XSecurityCreateProcessHook(int dwHardwareThread) {
	return 0;
}

int SystemHooks::XSecurityVerifyHook(DWORD dwMilliseconds, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	if (lpCompletionRoutine)
		NtQueueApcThread((HANDLE)-2, (PIO_APC_ROUTINE)APCWorker, lpOverlapped, (PIO_STATUS_BLOCK)lpCompletionRoutine, 0);

	return ERROR_SUCCESS;
}

int SystemHooks::XSecurityGetFailureInfoHook(PXSECURITY_FAILURE_INFORMATION pFailureInformation) {
	if (pFailureInformation->dwSize != 0x18)
		return ERROR_NOT_ENOUGH_MEMORY;

	pFailureInformation->dwBlocksChecked = 0;
	pFailureInformation->dwFailedReads = 0;
	pFailureInformation->dwFailedHashes = 0;
	pFailureInformation->dwTotalBlocks = 0;
	pFailureInformation->fComplete = TRUE;

	return ERROR_SUCCESS;
}

int SystemHooks::XexGetProcedureAddressHook(HANDLE hand, DWORD dwOrdinal, PVOID* pvAddress) {
	if (hand == GetModuleHandleA(MODULE_XAM)) {
		switch (dwOrdinal) {
		case 0x9BB:
			*pvAddress = XSecurityCreateProcessHook;
			return 0;
		case 0x9BC:
			*pvAddress = XSecurityCloseProcessHook;
			return 0;
		case 0x9BD:
			*pvAddress = XSecurityVerifyHook;
			return 0;
		case 0x9BE:
			*pvAddress = XSecurityGetFailureInfoHook;
			return 0;
		}
	}

	return XexGetProcedureAddress(hand, dwOrdinal, pvAddress);
}

void* SystemHooks::RtlImageXexHeaderFieldHook(void* headerBase, DWORD imageKey) {
	void* retVal = RtlImageXexHeaderField(headerBase, imageKey);

	if (imageKey == 0x40006 && retVal) {
		switch (((XEX_EXECUTION_ID*)retVal)->TitleID) {
		case 0xFFFF0055:   // Xex Menu
		case 0xFFFE07FF:   // XShellXDK
		case 0xF5D10000:   // dl main
		case 0xFFFF011D:   // dl installer
		case 0xF5D20000:   // fsd
		case 0x00000195:   // XeBoy Advance
		case 0x1CED291:    // PlayStation 1
		case 0x00000174:   // MAME360
		case 0x00000177:   // NXE2GOD
		case 0x00000180:   // DosBox
		case 0x00000167:   // Freestyle 3
		case 0x00000176:   // XM360
		case 0x00000184:   // OpenBOR360
		case 0xFFED7301:   // GameBoyAdvance360
		case 0x00001039:   // Snes360 PAL simpel v1
		case 0xFFED0707:   // Snes360
		case 0xFFFF051F:   // Atari 2600
		case 0x00000178:   // SuperMarioWar
		case 0x00000170:   // XexMenu 2.0
		case 0x00000166:   // Aurora
		case 0x4D5707DB:   // Unreal dev engine
		case 0x584b87ff:   // 360dashit
		case 0x00000155:   // psx emulator (early version)
		case 0x1CED2911: {  // psx emulator
			int ver = ((XboxKrnlVersion->Major & 0xF) << 28) | ((XboxKrnlVersion->Minor & 0xF) << 24) | (KERNEL_VERSION << 8) | (XboxKrnlVersion->Qfe);
			XeLive.ExecutionIDSpoof.BaseVersion = ver;
			XeLive.ExecutionIDSpoof.Version = ver;
			memcpy(retVal, &XeLive.ExecutionIDSpoof, sizeof(XEX_EXECUTION_ID));
			break;
		}
		}
	}
	else if (imageKey == 0x40006 && !retVal) {
		retVal = &XeLive.ExecutionIDSpoof;
	}

	return retVal;
}

typedef int(*tNetDll_connect)(XNCALLER_TYPE xnCaller, SOCKET socket, const sockaddr* name, DWORD length);
tNetDll_connect OriginalNetDll_connect;
int SystemHooks::NetDll_connectHook(XNCALLER_TYPE xnCaller, SOCKET socket, const sockaddr* name, DWORD length) {
	if (name) {
		sockaddr_in* ptr = (sockaddr_in*)name;
		if (ptr) {
			if (ptr->sin_addr.S_un.S_addr == 0x69696969) {
#ifdef LOCAL_SERVER
				ptr->sin_addr.S_un.S_un_b.s_b1 = 192;
				ptr->sin_addr.S_un.S_un_b.s_b2 = 168;
				ptr->sin_addr.S_un.S_un_b.s_b3 = 31;
				ptr->sin_addr.S_un.S_un_b.s_b4 = 24;
#else
				ptr->sin_addr.S_un.S_un_b.s_b1 = 45;
				ptr->sin_addr.S_un.S_un_b.s_b2 = 79;
				ptr->sin_addr.S_un.S_un_b.s_b3 = 27;
				ptr->sin_addr.S_un.S_un_b.s_b4 = 181;
#endif
			}
		}
	}

	return OriginalNetDll_connect(xnCaller, socket, name, length);
}

typedef DWORD(*tXamShowMessageBoxUI)(DWORD dwUserIndex, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR* pwszButtons, DWORD dwFocusButton, DWORD dwFlags, PMESSAGEBOX_RESULT pResult, PXOVERLAPPED pOverlapped);
tXamShowMessageBoxUI OriginalXamShowMessageUI;
DWORD SystemHooks::XamShowMessageBoxUIHook(DWORD dwUserIndex, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR* pwszButtons, DWORD dwFocusButton, DWORD dwFlags, PMESSAGEBOX_RESULT pResult, PXOVERLAPPED pOverlapped) {
	if (wszText) {
		bool valid = false;
		if (wcsstr(wszText, L"Status Code: 807b0190")) {
			valid = true;
			wszText = L"We broke the store! Sorry about that :( We aim to have it fixed real soon.";
		}
		else if (wcsstr(wszText, L"80151907")) {
			valid = true;
			wszText = L"We got a bad response from Live, most likely related to your KV.";
		}
		else if (wcsstr(wszText, L"8015190E") || wcsstr(wszText, L"83859DD2")) {
			valid = true;
			wszText = L"You're temporarily blocked from connecting to Xbox Live! Please turn off your console for at least 5 minutes, then try again.";
			Launch::SetLiveBlock(true);
		}
		else if (wcsstr(wszText, L"8015000A")) {
			valid = true;
			wszText = L"You've got an error, 8015000A. This is most common when using a bridged connection to connect your console to the internet. If this is the case for you, reset your network adapter config.";
		}

		if (valid) {
			pwszButtons = new const wchar_t* [1];
			pwszButtons[0] = new wchar_t[6];
			lstrcpyW((wchar_t*)pwszButtons[0], L"Okay!");

			return OriginalXamShowMessageUI(dwUserIndex, wszTitle, wszText, 1, pwszButtons, dwFocusButton, dwFlags, pResult, pOverlapped);
		}
	}

	return OriginalXamShowMessageUI(dwUserIndex, wszTitle, wszText, cButtons, pwszButtons, dwFocusButton, dwFlags, pResult, pOverlapped);
}

typedef HRESULT(*tXamShowMessageBox)(DWORD unk, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR* pwszButtons, DWORD dwFocusButton, MBOXRESULT resFun, DWORD dwFlags);
tXamShowMessageBox OriginalXamShowMessageBox;
HRESULT SystemHooks::XamShowMessageBoxHook(DWORD unk, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR* pwszButtons, DWORD dwFocusButton, MBOXRESULT resFun, DWORD dwFlags) {
	if (wszText) {
		bool valid = false;
		if (wcsstr(wszText, L"Status Code: 807b0190")) {
			valid = true;
			wszText = L"We broke the store! Sorry about that :( We aim to have it fixed real soon.";
		}
		else if (wcsstr(wszText, L"80151907")) {
			valid = true;
			wszText = L"We got a bad response from Live, most likely related to your KV.";
		}
		else if (wcsstr(wszText, L"8015190E") || wcsstr(wszText, L"83859DD2")) {
			valid = true;
			wszText = L"You're temporarily blocked from connecting to Xbox Live! Please turn off your console for at least 5 minutes, then try again.";
			Launch::SetLiveBlock(true);
		}
		else if (wcsstr(wszText, L"8015000A")) {
			valid = true;
			wszText = L"You've got an error, 8015000A. This is most common when using a bridged connection to connect your console to the internet. If this is the case for you, reset your network adapter config.";
		}

		if (valid) {
			pwszButtons = new const wchar_t* [1];
			pwszButtons[0] = new wchar_t[6];
			lstrcpyW((wchar_t*)pwszButtons[0], L"Okay!");

			return OriginalXamShowMessageBox(unk, wszTitle, wszText, 1, pwszButtons, dwFocusButton, resFun, dwFlags);
		}
	}

	return OriginalXamShowMessageBox(unk, wszTitle, wszText, cButtons, pwszButtons, dwFocusButton, resFun, dwFlags);
}

DWORD SystemHooks::XeKeysExecuteHook(PBYTE Buffer, DWORD Size, PBYTE HVSalt, PVOID r6, PVOID r7, PVOID r8) {
	if (XeLive.bAccountBanned) {
		LOG_PRINT("Enabling live block! #3");
		Launch::SetLiveBlock(true);
		return Native::DecryptDWORD(0xC008E3E5 /*0xC000009A*/);
	}

	int retryCount = 0;

checkStatus:
	if (!XeLive.bLoadedProperly) {
		if (retryCount > 25) {
			LOG_PRINT("Enabling live block! #4");
			Launch::SetLiveBlock(true);
			return Native::DecryptDWORD(0xC008E3E5 /*0xC000009A*/);
		}

		retryCount++;
		Native::Sleep(Native::DecryptDWORD(0x8E103 /*1000*/));
		goto checkStatus;
	}

	Request::ServerPacketXeKeysExecute* packetXeKeysExecute = (Request::ServerPacketXeKeysExecute*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketXeKeysExecute));
	Requests::PopulateHeader(packetXeKeysExecute, PACKET_XEKEYSEXECUTE, sizeof(Request::ServerPacketXeKeysExecute));
	Response::ServerPacketWelcome* packetXeKeysExecuteResponse = (Response::ServerPacketWelcome*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketWelcome));

	auto cleanup = [&] {
		Native::XEncryptedFree(packetXeKeysExecute);
		Native::XEncryptedFree(packetXeKeysExecuteResponse);
	};

	auto KV = Keyvault::GetKeyVault();

	memcpy(packetXeKeysExecute->HVSalt, HVSalt, 0x10);
	memcpy(packetXeKeysExecute->ConsolePartNumber, &KV->consoleCertificate.ConsolePartNumber, 0xB);
	packetXeKeysExecute->UpdateSequence = XeLive.dwUpdateSequence;
	packetXeKeysExecute->Type1KV = XeLive.bTypeOneKV;
	packetXeKeysExecute->Fcrt = XeLive.bFCRT;
	packetXeKeysExecute->Crl = XeLive.bCRL;

	if (Network::Connect()) {
		if (Network::Process(packetXeKeysExecute, sizeof(Request::ServerPacketXeKeysExecute), packetXeKeysExecuteResponse, sizeof(Response::ServerPacketXeKeysExecute), FALSE)) {
			if (packetXeKeysExecuteResponse->Status == 0x4A000000) {
				memset(Buffer, 0, Size);
				if (!Network::Receive(Buffer, 0x100)) {
					Notify("XeLive - Failed to Receive Challenge Response").Message();
					goto Failed;
				}
				if (Utils::BufferIsNull(Buffer, 0x100) == TRUE) {
					Notify("XeLive - Challenge Response is 0").Message();
					goto Failed;
				}

				memcpy(Buffer + 0x64, XeLive.szCPUDigest, 0x14);
			}
		}
		else goto Failed;
	}
	else goto Failed;

	Network::Disconnect();
	!XeLive.bCRL ? Notify("XeLive - Connected To Xbox Live!").Message(), XeLive.bCRL = TRUE : 0;

	return S_OK;
Failed:
	Sleep(4000);
	Native::HalReturnToFirmware(HalRebootQuiesceRoutine);
	return E_FAIL;
}

typedef void(*tNetDll_XnpSetChallengeResponse)(XNCALLER_TYPE xnc, DWORD r4, BYTE* respBuff, DWORD respSize);
tNetDll_XnpSetChallengeResponse OriginalNetDll_XnpSetChallengeResponse;
void SystemHooks::NetDll_XnpSetChallengeResponseHook(XNCALLER_TYPE xnc, DWORD r4, BYTE* respBuff, DWORD respSize) {
	if (XeLive.bAccountBanned) {
		Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
		return;
	}

	if (Native::Read2Byte(Native::DecryptDWORD(0x900A44D8 /*0x90016715*/)) != Native::DecryptDWORD(0x925B1 /*17150*/)) {
		Launch::SetLiveBlock(true);
		Notify("XeLive - XOSC libary version updated. Rebooting for security!").Message();
		Native::Sleep(Native::DecryptDWORD(0x8ED4B /*4000*/));
		Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
	}

	Request::ServerPacketXOSC* packetXOSC = (Request::ServerPacketXOSC*)Native::XEncryptedAlloc(sizeof(Request::ServerPacketXOSC));
	Response::ServerPacketXOSC* packetXOSCResponse = (Response::ServerPacketXOSC*)Native::XEncryptedAlloc(sizeof(Response::ServerPacketXOSC));

	auto keyVault = Keyvault::GetKeyVault();

	packetXOSC->HvProtectedFlags = *(long long*)Native::DecryptDWORD(0x8E0C6F33 /*0x8E038678*/);
	packetXOSC->iMotherboardIndex = Keyvault::GetMotherboardIndex();

	memcpy(packetXOSC->szXeIkaCertificateInquiryData, (BYTE*)&keyVault->xeIkaCertificate.Data.OddData.InquiryData, 0x24);
	memcpy(packetXOSC->szConsoleSerialNumber, (BYTE*)keyVault->consoleSerialNumber, 0xC);
	memcpy(packetXOSC->szConsoleCertificateAbData, (BYTE*)keyVault->consoleCertificate.ConsoleId.abData, 5);
	memcpy(packetXOSC->szCpuKeyDigest, XeLive.szCPUDigest, 0x10);

	packetXOSC->wOddFeatures = keyVault->oddFeatures;
	packetXOSC->bTypeOneKV = XeLive.bTypeOneKV;
	packetXOSC->dwPolicyFlashSize = keyVault->policyFlashSize;
	packetXOSC->bFcrt = XeLive.bFCRT;
	packetXOSC->dwTitleID = 0;

	PXEX_EXECUTION_ID execID;
	if (NT_SUCCESS(XamGetExecutionId(&execID))) {
		packetXOSC->dwTitleID = execID->TitleID;
		packetXOSC->dwMediaID = execID->MediaID;
	}

	memcpy(packetXOSC->szXOSCBuffer, respBuff, sizeof(packetXOSC->szXOSCBuffer));

	if (Requests::PacketXOSC(packetXOSC, packetXOSCResponse)) {
		memcpy(respBuff, packetXOSCResponse->szXOSCBuffer, sizeof(packetXOSCResponse->szXOSCBuffer));

		XOSCResponse* response = (XOSCResponse*)respBuff;
		bool success = true;

		if (response->hvCpuKeyHash == 0
			|| response->zeroEncryptedConsoleType == 0
			|| response->xexHashing == 0
			|| response->BldrFlags != Native::DecryptDWORD(0x9D971 /*0xD83E*/)
			|| memcmp(response->DvdInqRespData, response->XeikaInqData, 0x24)
			|| response->crlVersion != 6
			|| response->respMagic != Native::DecryptDWORD(0x5F5C2A5B /*0x5F534750*/)) {
			success = false;
		}

		if (!success) {
			Launch::SetLiveBlock(true);
			Notify("XeLive - XOSC sanity failed!").Message();
			Native::Sleep(Native::DecryptDWORD(0x8ED4B /*4000*/));
			Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
			return;
		}
	}
	else {
		Launch::SetLiveBlock(true);
		Notify("XeLive - XOSC failed!").Message();
		Native::Sleep(Native::DecryptDWORD(0x8ED4B /*4000*/));
		Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
		return;
	}

	Native::XEncryptedFree(packetXOSCResponse);

	OriginalNetDll_XnpSetChallengeResponse(xnc, r4, respBuff, respSize);
}

HRESULT SystemHooks::Initialize() {
	int ver = ((XboxKrnlVersion->Major & 0xF) << 28) | ((XboxKrnlVersion->Minor & 0xF) << 24) | (KERNEL_VERSION << 8) | (XboxKrnlVersion->Qfe);
	memset(&XeLive.ExecutionIDSpoof, 0, sizeof(XEX_EXECUTION_ID));
	XeLive.ExecutionIDSpoof.Version = ver;
	XeLive.ExecutionIDSpoof.BaseVersion = ver;
	XeLive.ExecutionIDSpoof.TitleID = 0xFFFE07D1;

	Launch::SetLiveBlock(true);

	Utils::XeCryptRandom2(XeLive.szSessionToken, 0x10);

	Hooking::HookFunction(Native::ResolveFunction("xam.xex", Native::DecryptDWORD(0x8E3EB /*128*/)), &NetDll_XnpSetChallengeResponseHook, &OriginalNetDll_XnpSetChallengeResponse, true);
	Hooking::HookFunction(Native::ResolveFunction("xam.xex", Native::DecryptDWORD(0x8E15F /*12*/)), &NetDll_connectHook, &OriginalNetDll_connect, true);
	Hooking::HookFunction(Native::ResolveFunction("xam.xex", Native::DecryptDWORD(0x8E5B5 /*714*/)), &XamShowMessageBoxUIHook, &OriginalXamShowMessageUI);
	Hooking::HookFunction(Native::ResolveFunction("xam.xex", Native::DecryptDWORD(0x8E614 /*745*/)), &XamShowMessageBoxHook, &OriginalXamShowMessageBox);

	Hooking::HookModuleImport(MODULE_XAM, MODULE_KERNEL, Native::DecryptDWORD(0x8EB52 /*607*/), XeKeysExecuteHook);
	Hooking::HookModuleImport(MODULE_XAM, MODULE_KERNEL, Native::DecryptDWORD(0x8E74B /*416*/), XexStartExecutableHook);
	Hooking::HookModuleImport(MODULE_XAM, MODULE_KERNEL, Native::DecryptDWORD(0x8E757 /*404*/), XexCheckExecutablePrivilegeHook);
	Hooking::HookModuleImport(MODULE_XAM, MODULE_KERNEL, Native::DecryptDWORD(0x8E71A /*407*/), XexGetProcedureAddressHook);
	Hooking::HookModuleImport(MODULE_XAM, MODULE_KERNEL, Native::DecryptDWORD(0x8E6D6 /*299*/), RtlImageXexHeaderFieldHook);

	return S_OK;
}