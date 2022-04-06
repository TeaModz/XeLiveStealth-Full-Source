#include "stdafx.h"

struct ExpansionInstallHeader {
	DWORD dwSignature;
	DWORD dwHashType;
	DWORD dwFullSize;
	BYTE szHash[0x14];
	BYTE szAesFeed[0x10];
	BYTE szPksc1Data[0x100];
	DWORD dwBlockSignature;
	DWORD dwKeyStatus;
	DWORD dwVer1;
	DWORD dwVer2;
	DWORD dwUnk1;
	DWORD dwUnk2;
	QWORD qwRestrictedPrivs;
	DWORD dwUnk3;
	DWORD dwUnk4;
	DWORD dwSourceOffset;
	DWORD dwCodeChunkSize;
	DWORD dwEntryOffset;
	DWORD dwHeaderSize;
	DWORD dwCodeSize;
	DWORD dwUnk5;
	DWORD dwCode[1];
};

BYTE* PayloadBuilder::ExpansionInstall(BYTE* pPayload, DWORD dwPayloadSize, DWORD dwID) {
	BYTE* expansionData = (BYTE*)Native::XPhysicalAlloc(0x1000, MAXULONG_PTR, 0, PAGE_READWRITE);
	if (expansionData) {
		memset(expansionData, 0, 0x1000);

		ExpansionInstallHeader* header = (ExpansionInstallHeader*)expansionData;
		header->dwSignature = 'HXPR';
		header->dwFullSize = 0x1000;
		header->dwKeyStatus = Native::Read4Byte(0x8e038610);
		header->qwRestrictedPrivs = Native::Read8Byte(0x8e038630);
		header->dwBlockSignature = dwID;
		header->dwSourceOffset = 0x160;
		header->dwEntryOffset = 0x10;
		header->dwHeaderSize = 0x10;
		header->dwCodeSize = (dwPayloadSize + 0xF) & ~0xF;
		header->dwCodeChunkSize = header->dwHeaderSize + header->dwCodeSize;

		memcpy(header->dwCode, pPayload, dwPayloadSize);

		Native::XeCryptRandom(header->szPksc1Data, 0x100);
		Native::XeCryptSha(&expansionData[0x130], header->dwFullSize - 0x130, NULL, 0, NULL, 0, header->szHash, 0x14);
		return (BYTE*)header;
	}

	return nullptr;
}