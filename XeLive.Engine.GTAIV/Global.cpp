#include "stdafx.h"

Global XeLive;

bool bTerminating = false;
void Terminate() {
	if (XeLive.bGameInitialized && !bTerminating) {
		bTerminating = true;
		XeLive.bShutdownCheats = true;

		while (XeLive.bShutdownCheats) {
			Sleep(10);
		}
	}
}