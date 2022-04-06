#include "stdafx.h"

Global XeLive;

void Global::Shutdown() {
	Native::XPhysicalFree(pPatches);
}

HRESULT Global::Initialize() {
	pPatches = (Cheats*)Native::XPhysicalAlloc(sizeof(Cheats), MAXULONG_PTR, NULL, PAGE_READWRITE);
	pPatches->Initialize();

	return STATUS_SUCCESS;
}

bool bTerminating = false;
void Terminate() {
	if (XeLive.bGameInitialized && !bTerminating) {
		bTerminating = true;
		XeLive.bShutdownCheats = true;

		while (XeLive.bShutdownCheats) {
			Sleep(50);
		}
	}
}