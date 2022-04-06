#include "stdafx.h"

Hooking pHooking;

void Hooking::UnhookAll() {
	for (DWORD i = 0; i < iHookCount; i++) {
		DWORD address = *(DWORD*)&szHookReplaceSection[i * 0x14];
		memcpy((void*)address, &szHookReplaceSection[(i * 0x14) + 4], 0x10);
	}
}