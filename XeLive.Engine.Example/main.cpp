#include "stdafx.h"

void ConstantLoop() {
	XeLive.Initialize();

	if (Native::XamGetCurrentTitleId() != GAME_TITLE) {
		LOG_DEV("Unsupported title!");
		XeLive.bTerminateProcess = true;
		goto jUnload;
	}

	if (!pSystemHooks.Apply()) {
		LOG_DEV("Failed to apply system hooks!");
		XeLive.bTerminateProcess = true;
		goto jUnload;
	}

	if (pUtils.MountPath(MOUNT_PATH, pUtils.GetMountPath(), FALSE) == 0) {
		LOG_DEV("Mounted path successfully!");
	}

	LOG_DEV("Started!");

	if (XeLive.pPatches->bHasPatches) {
		while (!XeLive.bTerminateProcess) {
			if (Native::XamGetCurrentTitleId() == GAME_TITLE && !XeLive.bShutdownCheats) {
				if (!pGame.bInitialized) {
					if (!pGame.Initialize()) {
						Sleep(1000);
					}
				}
				else {
					pGame.Run();
				}
			}
			else {
				LOG_DEV("Changing title! Shutdown initiated!");
				break;
			}
			Sleep(50);
		}

		if (pGame.bInitialized) {
			pGame.Shutdown();
		}
	}

jUnload:
	if (pSystemHooks.bInitialized)
		pSystemHooks.Remove();

	XeLive.Shutdown();

	LOG_DEV("Exiting!");

	XeLive.bConstantLoopRunning = false;

	if (!XeLive.bExternUnload) {
		*(WORD*)((DWORD)XeLive.hMyModule + 64) = 1;
		DWORD exitCode;
		GetExitCodeThread(GetCurrentThread(), &exitCode);
		XexUnloadImageAndExitThread(XeLive.hMyModule, (HANDLE)exitCode);
	}
}

HRESULT HiddenThreadStartup() {
	HANDLE hThread; DWORD hThreadID;

	if (ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)ConstantLoop, NULL, 0x1c000427) != S_OK) {
		return E_FAIL;
	}

	XeLive.bConstantLoopRunning = true;

	XSetThreadProcessor(hThread, 4);
	ResumeThread(hThread);
	CloseHandle(hThread);

	return S_OK;
}

HRESULT Initialize() {
	if (FAILED(Invoker::Initialize())) {
		LOG_DEV("Invoker failed!");
		return E_FAIL;
	}

	return HiddenThreadStartup();
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		srand(time(0));
		XeLive.hMyModule = hModule;
		XeLive.bDevkit = *(int*)0x8E038610 & 0x8000 ? false : true;

		if (FAILED(Initialize())) {
			*(WORD*)((DWORD)hModule + 64) = 1;
			return FALSE;
		}

		return TRUE;
	}

	case DLL_PROCESS_DETACH: {
		XeLive.bExternUnload = true;
		XeLive.bTerminateProcess = true;

		while (XeLive.bConstantLoopRunning) {
			Sleep(50);
		}
		return TRUE;
	}
	}

	return FALSE;
}