#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "BO1.h"
#include "Detour.h"

#if _DEVKIT
DWORD supportedVersion = 17526;
#else
DWORD supportedVersion = 17526;
#endif

BOOL g_killThread = FALSE;
extern void InitializeBO1Hooks();
extern void UninitializeBO1Cheater();


#define InsertTailList(ListHead,Entry) {\
	PLIST_ENTRY _EX_Blink;\
	PLIST_ENTRY _EX_ListHead;\
	_EX_ListHead = (ListHead);\
	_EX_Blink = _EX_ListHead->Blink;\
	(Entry)->Flink = _EX_ListHead;\
	(Entry)->Blink = _EX_Blink;\
	_EX_Blink->Flink = (Entry);\
	_EX_ListHead->Blink = (Entry);\
}

void DoXNotifyUI(LPCWSTR msg) {
	XNotifyQueueUI(XNOTIFYUI_TYPE_COMPLAINT, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_DEFAULT, (PWCHAR)msg, NULL);
}

void unloadSysDLL(HANDLE mHandle)
{
	//addToList(mHandle);
	*(WORD*)((DWORD)mHandle + 64) = 1;
	DWORD exitCode;
	GetExitCodeThread(GetCurrentThread(), &exitCode);
	XexUnloadImageAndExitThread(mHandle, (HANDLE)exitCode);
}

BOOL isStringEmpty(PCHAR String)
{
	if(String == NULL) return TRUE;

	if(String[0] == 0) return TRUE;

	return FALSE;
}

// Use this one!
BOOL IsTrayOpen() 
{
	unsigned char msg[0x10]; 
	unsigned char resp[0x10]; 
	memset(msg, 0x0, 0x10); 
	msg[0] = 0xA; 
	HalSendSMCMessage(msg, resp); 
	
	if (resp[1] == 0x60) return true; 

	return false; 
}

void bailout() {
	DbgPrint("Unloading module\r\n");
	g_killThread = TRUE;
	unloadSysDLL(Handle.hME);
}


BOOL isHooked = FALSE;
BOOL hasMainLoopExited = FALSE;
void MainLoop(void)  {
	while (XamGetCurrentTitleId() != 0x41560855)
		Sleep(500);

	PLDR_DATA_TABLE_ENTRY entry;
	XexPcToFileHeader((PVOID)0x82000000, &entry);
	if (wcscmp(entry->BaseDllName.Buffer, L"default_mp.xex") == 0) {
		//setup a memory manager class to store our patches
		PBYTE buf = (PBYTE)XPhysicalAlloc(0x2000, MAXULONG_PTR, 128, MEM_LARGE_PAGES | PAGE_READWRITE);
		mManager = new MemoryManager((DWORD)buf, 0x2000);
		mManager->alloc(sizeof(Security::CoDCheats), &CheatPatches);

		while (!g_killThread)
		{
			if (CheatPatches->fHasPatches)
			{
				if (!isHooked)
				{
					DbgPrint("XamGetCurrenTitleId(): %08X\r\n", XamGetCurrentTitleId());
					DbgPrint("Setting up detours");
					HANDLE h = GetModuleHandle("default_mp.xex");
					XNotifyUI(L"NiNJA Engine Initialized!");
					PXEX_EXECUTION_ID pXex;
					XamGetExecutionId(&pXex);
					// Initalize ur stuffz here
					InitializeBO1Hooks();

					isHooked = TRUE;
				}
				else if (XamGetCurrentTitleId() != 0x41560855) {
					break;
				}

				if (shouldSaveMenuOptions) {
					SaveMenuOptions();
					shouldSaveMenuOptions = false;
				}

				for (int i = 0; i < 18; i++)
				{
					if (PlayerInfo[i].GrabGeoIPInfo)
					{
						GeoIpInfo info;
						GeoIpParser::GeoIpParser parser(PlayerInfo[i].ip, &info);
						if (parser.success == true)
						{
							printf("Parser success!\n");
							memcpy(PlayerInfo[i].city, info.city, sizeof(info.city));
							memcpy(PlayerInfo[i].state, info.state, sizeof(info.state));
							memcpy(PlayerInfo[i].stateFull, info.stateFull, sizeof(info.stateFull));
							memcpy(PlayerInfo[i].country, info.country, sizeof(info.country));
							memcpy(PlayerInfo[i].isp, info.isp, sizeof(info.isp));
							memcpy(PlayerInfo[i].zip, info.zip, sizeof(info.zip));
							PlayerInfo[i].GrabGeoIPInfo = FALSE;
							DbgPrint("Success!");
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
			UninitializeBO1Cheater();
		}
		Sleep(500);
		XPhysicalFree((PVOID)mManager->memory);

		delete mManager;
	}

unload:
	DbgPrint("Main loop has exited successfully!");
	hasMainLoopExited = TRUE;

	if(!g_killThread)
		unloadSysDLL(Handle.hME);
}

DWORD flags = EX_CREATE_FLAG_SUSPENDED | EX_CREATE_FLAG_SYSTEM | EX_CREATE_FLAG_CORE2 | EX_CREATE_FLAG_PRIORITY1;

HRESULT Initialize()  {
	// Running on devkit?
	isDevkit = *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;

	// Get our module handles
	Handle.hXam = GetModuleHandle(NAME_XAM);
	Handle.hKernel = GetModuleHandle(NAME_KERNEL);
	Handle.hXBDM = GetModuleHandle(NAME_XBDM);
	if (Handle.hME == 0 || Handle.hXam == 0 || Handle.hKernel == 0) {
		return E_FAIL;
	}

	HANDLE hThread;
	DWORD hThreadId;

	ExCreateThread(&hThread, 0, &hThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)MainLoop, NULL, flags);
	ResumeThread(hThread);

	srand((int)time(NULL));
	return ERROR_SUCCESS;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_THREAD_ATTACH:
		case DLL_PROCESS_ATTACH: {
			if (!IsTrayOpen()) {
				Handle.hME = hModule;

				if (Initialize() == ERROR_SUCCESS) {
					if (!isDevkit)
						removeFromList(hModule);
				}
				else return FALSE;
			}
			break;
		}
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH: {
			g_killThread = TRUE;
			while (!hasMainLoopExited)
				Sleep(10);
			if (!isDevkit)
				addToList(Handle.hME);
			return TRUE;
		}
		default:
			break;
	}

	return TRUE;
}