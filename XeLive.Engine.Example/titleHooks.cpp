#include "stdafx.h"

Title pTitle;

Detour<void> OriginalR_EndFrame;
void R_EndFrameHook() {
	static once_flag initFlag;
	call_once(&initFlag, [] {
		pRenderer.Initialize();
		pMenu.Initialize();
		});

	pController.Update();
	pSubmenuHandler.ASyncUpdate();
	pMenu.Render();
	pMenu.ASyncUpdate();

	OriginalR_EndFrame.CallOriginal();
}

bool Title::ApplyHooks() {
	if (!bInitialized) {
		*(DWORD*)XeLive.pPatches->MW3.BoneNotFoundErrorPatch = 0x60000000;

		OriginalR_EndFrame.SetupDetour(XeLive.pPatches->MW3.R_EndFrame, R_EndFrameHook);

		LOG_DEV("Applied detours!");
		bInitialized = true;
		return true;
	} else {
		LOG_DEV("TitleHooks already initialized!");
	}

	return false;
}

void Title::Remove(bool bOverride) {
	if (bInitialized || bOverride) {
		pHooking.UnhookAll();
		OriginalR_EndFrame.TakeDownDetour();

		Sleep(1000);

		LOG_DEV("Removed detours!");
		bInitialized = false;
	} else {
		LOG_DEV("TitleHooks have not been initialized!");
	}
}