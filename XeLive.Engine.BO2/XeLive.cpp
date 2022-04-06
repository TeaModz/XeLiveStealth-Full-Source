#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "BO2.h"
#include "Detour.h"

BOOL g_killThread = FALSE;
extern void InitializeBO2Hooks();
extern void UninitializeBO2Cheater();

void unloadSysDLL(HANDLE mHandle)
{
	//addToList(mHandle);
	*(WORD*)((DWORD)mHandle + 64) = 1;
	DWORD exitCode;
	GetExitCodeThread(GetCurrentThread(), &exitCode);
	XexUnloadImageAndExitThread(mHandle, (HANDLE)exitCode);
}


void bailout() {
	DbgPrint("Unloading module\r\n");
	g_killThread = TRUE;
	unloadSysDLL(Handle.hME);
}


BOOL isHooked = FALSE;
BOOL hasMainLoopExited = FALSE;

void MainLoop(void)
{
	while (XamGetCurrentTitleId() != 0x415608C3)
		Sleep(500);


	PLDR_DATA_TABLE_ENTRY entry;
	XexPcToFileHeader((PVOID)0x82000000, &entry);
	if (wcscmp(entry->BaseDllName.Buffer, L"default_mp.xex") == 0)
	{
		//setup a memory manager class to store our patches
		PBYTE buf = (PBYTE)XPhysicalAlloc(0x2000, MAXULONG_PTR, 128, MEM_LARGE_PAGES | PAGE_READWRITE);
		mManager = new MemoryManager((DWORD)buf, 0x2000);
		mManager->alloc(sizeof(Security::CoDCheats), &CheatPatches); //allocate some memory for our cheat patches

		while (!g_killThread)
		{
			if (CheatPatches->fHasPatches)
			{
				if (!isHooked)
				{
					InitializeBO2Hooks();

					isHooked = TRUE;
				}
				else if (XamGetCurrentTitleId() != 0x415608C3)
				{
					break;
				}

				for (int i = 0; i < 18; i++)
				{
					if (PlayerInfo[i].GrabGeoIPInfo)
					{
						GeoIpInfo info;
						GeoIpParser::GeoIpParser parser(PlayerInfo[i].ip, &info);
						if (parser.success == true)
						{
							//printf("Parser success!\n");
							memcpy(PlayerInfo[i].city, info.city, sizeof(info.city));
							memcpy(PlayerInfo[i].state, info.state, sizeof(info.state));
							memcpy(PlayerInfo[i].stateFull, info.stateFull, sizeof(info.stateFull));
							memcpy(PlayerInfo[i].country, info.country, sizeof(info.country));
							memcpy(PlayerInfo[i].isp, info.isp, sizeof(info.isp));
							memcpy(PlayerInfo[i].zip, info.zip, sizeof(info.zip));
							PlayerInfo[i].GrabGeoIPInfo = FALSE;
							//DbgPrint("Success!");
						}
					}
				}
			}
			else {
				CheatPatches->setAddresses(NULL);
			}
			Sleep(100);
		}

		if (isHooked) {
			DbgPrint("Taking down detours\r\n");
			UninitializeBO2Cheater();
		}
		Sleep(500);
		XPhysicalFree((PVOID)mManager->memory);

		delete mManager;
	}

unload:
	DbgPrint("Main loop has exited successfully!");
	hasMainLoopExited = TRUE;

	if (!g_killThread)
		unloadSysDLL(Handle.hME);
}

DWORD flags = EX_CREATE_FLAG_SUSPENDED | EX_CREATE_FLAG_SYSTEM | EX_CREATE_FLAG_CORE2 | EX_CREATE_FLAG_PRIORITY1;
void HiddenThreadStartup() {
	initVariables();

	std::string iniPath;

	Flags.isInit = TRUE;

	// Let's begin our update presence thread now
	HANDLE hThread;
	DWORD hThreadId;

	ExCreateThread(&hThread, 0, &hThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)MainLoop, NULL, flags);
	if (hThread == NULL) return;
	ResumeThread(hThread);

	// Seed the random number generator
	srand((int)time(NULL));
}

HRESULT Initialize() {
	// Running on devkit?
	isDevkit = *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;

	// Get our module handles
	Handle.hXam = GetModuleHandle(NAME_XAM);
	Handle.hKernel = GetModuleHandle(NAME_KERNEL);
	Handle.hXBDM = GetModuleHandle(NAME_XBDM);
	if (Handle.hME == 0 || Handle.hXam == 0 || Handle.hKernel == 0)
	{
		DbgPrint("Failed to get system module handles");
		return E_FAIL;
	}

	DWORD startFunc = GetModuleImportAddr(Handle.hME, Handle.hKernel, 25);
	//DbgPrint("StartFunc: %08X  %08X", startFunc, *(DWORD*)startFunc);
	if (!startFunc)
		return E_FAIL;

	HiddenThreadStartup();
	return ERROR_SUCCESS;
}

char SecurityData[0x300] = { 0x78, 0x62, 0x4F, 0x62, 0x66, 0x75, 0x00, 0x00,
(sizeof(SecurityData) >> 24) & 0xFF, (sizeof(SecurityData) >> 16) & 0xFF, (sizeof(SecurityData) >> 8) & 0xFF, sizeof(SecurityData) & 0xFF };
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_THREAD_ATTACH:
		case DLL_PROCESS_ATTACH:{
			Handle.hME = hModule;

			if (FAILED(Initialize())) return FALSE;
			break;
		}
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH: {
			g_killThread = TRUE;
			while (!hasMainLoopExited)
				Sleep(10);
			return TRUE;
		}
	}

	return TRUE;
}