#include "stdafx.h"

HRESULT CPU::Initialize() {
	bool bNoFile = false;
	const char* pLocation = NULL;

	if (Utils::FileExists("XELIVE:\\cpukey.txt"))
		pLocation = "XELIVE:\\cpukey.txt";
	else if (Utils::FileExists("XELIVE:\\cpukey.bin"))
		pLocation = "XELIVE:\\cpukey.bin";
	else bNoFile = true;

	byte buffer[0x10];

	if (pLocation != NULL) {
		FILE* fp = fopen(pLocation, "rb");
		if (fp) {
			if (strstr(pLocation, ".txt")) {
				BYTE v[0x20];
				fread(v, 0x20, 1, fp);

				for (int i = 0, b = 0; i < 0x20; i += 2, b++) {
					buffer[i == 0 ? 0 : (i / 2)] = (((Utils::CharToByte(v[i]) << 4) & 0xF0) | ((Utils::CharToByte(v[i + 1]) & 0x0F)));
				}
			}
			else {
				fread(buffer, 0x10, 1, fp);
			}

			fclose(fp);
		}
		else bNoFile = true;
	}

	auto kv = (KEY_VAULT*)Keyvault::GetKeyVault();

	if (bNoFile) {
		LOG_PRINT("Couldn't find cpukey.txt or cpukey.bin, using KV");
		memcpy(buffer, kv->hmacShaDigest, 0x10);
	}

	memcpy(XeLive.szCPUBinKey, buffer, 0x10);

	Native::XeCryptSha(XeLive.szCPUBinKey, 0x10, 0, 0, 0, 0, XeLive.szCPUDigest, XECRYPT_SHA_DIGEST_SIZE);
	Native::XeCryptSha(Native::HvGetFuseCPU(), 0x10, 0, 0, 0, 0, XeLive.szCPUFuseDigest, XECRYPT_SHA_DIGEST_SIZE);

	struct pBuf {
		BYTE szCPU[0x10];
		BYTE szSmcVersion[0x2];
		BYTE szConsoleType[0x4];
	};

	pBuf* temp = new pBuf();

	memcpy(temp->szCPU, Native::HvGetFuseCPU(), 0x10);
	memcpy(temp->szSmcVersion, Native::SmcGetVersion(), 0x2);
	memcpy(temp->szConsoleType, Native::HvGetConsoleType(), 0x4);

	Native::XeCryptSha((BYTE*)(DWORD)temp, sizeof(pBuf), 0, 0, 0, 0, XeLive.szConsoleKey, XECRYPT_SHA_DIGEST_SIZE);
	IntegrityManager::Push((DWORD)XeLive.szConsoleKey, 0x14, IntegrityRegisterSettings(ProcessIntegrityMetricAndReboot, 0xad23c26a));
	delete temp;

	return S_OK;
}