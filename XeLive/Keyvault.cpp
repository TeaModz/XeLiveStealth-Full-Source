#include "stdafx.h"

bool Keyvault::bInitializedKVHash;
BYTE Keyvault::szKV[0x4000];
BYTE Keyvault::szKVDigest[0x10];
BYTE Keyvault::szKVHash[0x10];

HRESULT Keyvault::Initialize() {
	LOG_PRINT("Loading KV from kv.bin");
	if (Utils::FileExists("XELIVE:\\kv.bin")) {
		FILE* fp = fopen("XELIVE:\\kv.bin", "rb");
		if (fp) {
			fread(szKV, 1, 0x4000, fp);
			fclose(fp);

			return SetKeyvault((BYTE*)szKV);
		}
		else {
			LOG_PRINT("Failed to read KV from %s", "XELIVE:\\kv.bin");
			return S_FAIL;
		}
	}

	LOG_PRINT("KV.bin not found, using flash KV");

	Hypervisor::HvPeekBytes(Hypervisor::HvPeekQWORD(KEYVAULT_HV_PTR), szKV, 0x4000);

	return SetKeyvault((BYTE*)szKV);
}

HRESULT Keyvault::SetKeyvault(BYTE* pBuffer, bool reboot) {
	HRESULT ret;

	KEY_VAULT* keyVault = (PKEY_VAULT)pBuffer;

	BYTE currentMac[6] = { 0 };

	bool cond = (XboxHardwareInfo->Flags & 0xF0000000) > 0x40000000;

	BYTE mac[6] = {
		cond ? 0x7C : 0x00,
		cond ? 0xED : 0x22,
		cond ? 0xD8 : 0x48,
		keyVault->consoleCertificate.ConsoleId.asBits.MacIndex3,
		keyVault->consoleCertificate.ConsoleId.asBits.MacIndex4,
		keyVault->consoleCertificate.ConsoleId.asBits.MacIndex5
	};

	DWORD currentEnvironment;
	DWORD environment = 0x50524F44;

	memset(keyVault->roamableObfuscationKey, 0, 0x10);
	memcpy(szKVDigest, keyVault->hmacShaDigest, 0x10);

	XeLive.bFCRT = (keyVault->oddFeatures & ODD_POLICY_FLAG_CHECK_FIRMWARE) != 0;
	XeLive.bTypeOneKV = true;

	for (int i = 0; i < sizeof(keyVault->keyVaultSignature); ++i) {
		if (keyVault->keyVaultSignature[i] != 0) {
			XeLive.bTypeOneKV = false;
			break;
		}
	}

	memcpy((void*)0x8E03A000, &keyVault->consoleCertificate, 0x1A8);
	memcpy((void*)0x8E038020, &keyVault->consoleCertificate.ConsoleId, 5);

	BYTE newHash[XECRYPT_SHA_DIGEST_SIZE];
	Native::XeCryptSha((BYTE*)0x8E038014, 0x3EC, NULL, NULL, NULL, NULL, newHash, XECRYPT_SHA_DIGEST_SIZE);

	memcpy((void*)0x8E038000, newHash, XECRYPT_SHA_DIGEST_SIZE);

	QWORD kvAddress = Hypervisor::HvPeekQWORD(KEYVAULT_HV_PTR);

	if (FAILED(ret = Hypervisor::HvPeekBytes(kvAddress + 0xD0, &keyVault->consoleObfuscationKey, 0x40))) {
		LOG_DEV("HvPeekBytes failed (0x%X)", ret);
		return ret;
	}

	if (FAILED(ret = Hypervisor::HvPokeBytes(kvAddress, keyVault, 0x4000))) {
		LOG_DEV("HvPokeBytes failed (0x%X)", ret);
		return ret;
	}

	if (FAILED(ret = HRESULT_FROM_NT(Native::ExGetXConfigSetting(XCONFIG_SECURED_CATEGORY, XCONFIG_SECURED_MAC_ADDRESS, currentMac, 6, NULL)))) {
		LOG_DEV("ExGetXConfigSetting failed (0x%X)", ret);
		return ret;
	}

	if (memcmp(currentMac, mac, 6) != 0) {
		LOG_DEV("Setting new MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

		if (FAILED(ret = HRESULT_FROM_NT(Native::ExSetXConfigSetting(XCONFIG_SECURED_CATEGORY, XCONFIG_SECURED_MAC_ADDRESS, mac, 6)))) {
			LOG_DEV("ExSetXConfigSetting failed (0x%X)", ret);
			return ret;
		}
		LOG_DEV("Setting new MAC address");

		Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
	}

	DWORD Temp = 0;
	XeCryptSha(currentMac, 6, 0, 0, 0, 0, (PBYTE)&Temp, 4);
	XeLive.dwUpdateSequence |= (Temp & ~0xFF);

	if (FAILED(ret = HRESULT_FROM_NT(Native::ExGetXConfigSetting(XCONFIG_SECURED_CATEGORY, XCONFIG_SECURED_ONLINE_NETWORK_ID, &currentEnvironment, 4, NULL)))) {
		LOG_DEV("ExGetXConfigSetting failed (0x%X)", ret);
		return ret;
	}

	if (currentEnvironment != environment) {
		LOG_DEV("Setting production live environment");

		if (FAILED(ret = HRESULT_FROM_NT(Native::ExSetXConfigSetting(XCONFIG_SECURED_CATEGORY, XCONFIG_SECURED_ONLINE_NETWORK_ID, &environment, 4)))) {
			LOG_DEV("ExSetXConfigSetting failed (0x%X)", ret);
			return ret;
		}
	}

	if (!Native::XamCacheReset(XAM_CACHE_TICKETS)) {
		LOG_DEV("XamCacheReset failed");
		return S_FAIL;
	}

	LOG_PRINT("KV set!");

	return S_OK;
}

int Keyvault::GetMotherboardIndex() {
	BYTE serial = 0;
	auto kv = GetKeyVault();

	serial = (((Utils::CharToByte(kv->consoleCertificate.ConsolePartNumber[2]) << 4) & 0xF0) | ((Utils::CharToByte(kv->consoleCertificate.ConsolePartNumber[3]) & 0x0F)));

	if (serial < 0x10) {
		serial = 0;
	}
	else if (serial < 0x14) {
		serial = 1;
	}
	else if (serial < 0x18) {
		serial = 2;
	}
	else if (serial < 0x52) {
		serial = 3;
	}
	else if (serial < 0x58) {
		serial = 4;
	}
	else serial = 5;

	return serial;
}

DWORD Keyvault::GetKeyvaultHash() {
	return ((szKVDigest[0] << 24) | (szKVDigest[1] << 16) | (szKVDigest[2] << 8) | szKVDigest[3]);
}