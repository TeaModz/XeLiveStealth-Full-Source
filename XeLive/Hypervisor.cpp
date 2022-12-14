#include "stdafx.h"

Hypervisor::tHvxKeysExecute Hypervisor::HvxKeysExecute;
Hypervisor::tHvxExpansionInstall Hypervisor::HvxExpansionInstall;
Hypervisor::tHvxExpansionCall Hypervisor::HvxExpansionCall;
Hypervisor::HVData Hypervisor::Data;

void* Hypervisor::SetupHiddenSyscall(BYTE syscallID) {
	auto allocated = Native::XEncryptedAlloc(12);

	BYTE syscall[12] = {
		0x38, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x02, 0x4E, 0x80, 0x00, 0x20
	};

	*(BYTE*)(syscall + 3) = (syscallID ^ 10);

	memcpy(allocated, syscall, 12);

	return allocated;
}

HRESULT Hypervisor::Initialize() {
	LOG_PRINT("Initializing #528bb4");
	HvxKeysExecute = (tHvxKeysExecute)SetupHiddenSyscall(XeLive.bDevkit ? 0x4A : 0x48);

	LOG_PRINT("Initializing #c6c763e3");
	HvxExpansionInstall = (tHvxExpansionInstall)SetupHiddenSyscall(XeLive.bDevkit ? 0x7A : 0x78);

	LOG_PRINT("Initializing #8e2a71b");
	HvxExpansionCall = (tHvxExpansionCall)SetupHiddenSyscall(XeLive.bDevkit ? 0x7B : 0x79);

	auto result = InstallExpansions();

	if (!XeLive.bDevkit) {
		LOG_PRINT("Initializing #729faa21");
		BYTE szBootLoaderKey[0x10] = { 0xDD, 0x88, 0xAD, 0x0C, 0x9E, 0xD6, 0x69, 0xE7, 0xB5, 0x67, 0x94, 0xFB, 0x68, 0x56, 0x3E, 0xFA };
		HvPokeBytes(0x200016510, szBootLoaderKey, 0x10);
	}

	XeLive.bDevkit = XeLive.bDevkit || HvPeekBYTE(0x0) == 0x5E;

	return result;
}

HRESULT Hypervisor::InstallExpansions() {
	BYTE szPeekPoke[472] = {
			0x7D, 0x88, 0x02, 0xA6, 0x91, 0x81, 0xFF, 0xF8, 0xFB, 0xC1, 0xFF, 0xE8,
			0xFB, 0xE1, 0xFF, 0xF0, 0x94, 0x21, 0xFF, 0x90, 0x7C, 0xBF, 0x2B, 0x78,
			0x7C, 0xFE, 0x3B, 0x78, 0x2B, 0x03, 0x00, 0x01, 0x40, 0x99, 0x00, 0x08,
			0x38, 0x63, 0x00, 0x01, 0x2B, 0x24, 0x00, 0x00, 0x40, 0x9A, 0x00, 0x1C,
			0x2B, 0x1F, 0x00, 0xF0, 0x41, 0x98, 0x00, 0x0C, 0x2B, 0x1F, 0x01, 0x00,
			0x40, 0x99, 0x00, 0xD0, 0x88, 0x7F, 0x00, 0x00, 0x48, 0x00, 0x01, 0x7C,
			0x2B, 0x24, 0x00, 0x01, 0x40, 0x9A, 0x00, 0x20, 0x2B, 0x1F, 0x00, 0xF0,
			0x41, 0x98, 0x00, 0x0C, 0x2B, 0x1F, 0x01, 0x00, 0x40, 0x99, 0x00, 0xB0,
			0xA1, 0x7F, 0x00, 0x00, 0x7D, 0x63, 0x07, 0x34, 0x48, 0x00, 0x01, 0x58,
			0x2B, 0x24, 0x00, 0x02, 0x40, 0x9A, 0x00, 0x1C, 0x2B, 0x1F, 0x00, 0xF0,
			0x41, 0x98, 0x00, 0x0C, 0x2B, 0x1F, 0x01, 0x00, 0x40, 0x99, 0x00, 0x8C,
			0x80, 0x7F, 0x00, 0x00, 0x48, 0x00, 0x01, 0x38, 0x2B, 0x24, 0x00, 0x03,
			0x40, 0x9A, 0x00, 0x1C, 0x2B, 0x1F, 0x00, 0xF0, 0x41, 0x98, 0x00, 0x0C,
			0x2B, 0x1F, 0x01, 0x00, 0x40, 0x99, 0x00, 0x6C, 0xE8, 0x7F, 0x00, 0x00,
			0x48, 0x00, 0x01, 0x18, 0x2B, 0x24, 0x00, 0x04, 0x40, 0x9A, 0x00, 0x64,
			0x39, 0x60, 0x28, 0x28, 0x38, 0x80, 0x00, 0x01, 0x7C, 0xC3, 0x33, 0x78,
			0x7D, 0x69, 0x03, 0xA6, 0x4E, 0x80, 0x04, 0x21, 0x57, 0xCA, 0x00, 0x3E,
			0x2F, 0x0A, 0x00, 0x00, 0x40, 0x99, 0x00, 0x3C, 0x7D, 0x49, 0x03, 0xA6,
			0x7F, 0xEB, 0xFB, 0x78, 0x7D, 0x5F, 0x18, 0x50, 0x39, 0x20, 0x00, 0x00,
			0x2B, 0x0B, 0x00, 0xF0, 0x41, 0x98, 0x00, 0x14, 0x2B, 0x0B, 0x01, 0x00,
			0x41, 0x99, 0x00, 0x0C, 0x7D, 0x2A, 0x59, 0xAE, 0x48, 0x00, 0x00, 0x0C,
			0x89, 0x0B, 0x00, 0x00, 0x7D, 0x0A, 0x59, 0xAE, 0x39, 0x6B, 0x00, 0x01,
			0x42, 0x00, 0xFF, 0xDC, 0x38, 0x60, 0x00, 0x00, 0x48, 0x00, 0x00, 0xB0,
			0x2B, 0x24, 0x00, 0x05, 0x40, 0x9A, 0x00, 0x10, 0x38, 0x60, 0x00, 0x00,
			0x98, 0xDF, 0x00, 0x00, 0x48, 0x00, 0x00, 0x9C, 0x2B, 0x24, 0x00, 0x06,
			0x40, 0x9A, 0x00, 0x10, 0x38, 0x60, 0x00, 0x00, 0xB0, 0xDF, 0x00, 0x00,
			0x48, 0x00, 0x00, 0x88, 0x2B, 0x24, 0x00, 0x07, 0x40, 0x9A, 0x00, 0x10,
			0x38, 0x60, 0x00, 0x00, 0x90, 0xDF, 0x00, 0x00, 0x48, 0x00, 0x00, 0x74,
			0x2B, 0x24, 0x00, 0x08, 0x40, 0x9A, 0x00, 0x10, 0xF8, 0xDF, 0x00, 0x00,
			0x38, 0x60, 0x00, 0x00, 0x48, 0x00, 0x00, 0x60, 0x2B, 0x24, 0x00, 0x09,
			0x40, 0x9A, 0x00, 0x54, 0x39, 0x60, 0x28, 0x28, 0x38, 0x80, 0x00, 0x01,
			0x7C, 0xC3, 0x33, 0x78, 0x7D, 0x69, 0x03, 0xA6, 0x4E, 0x80, 0x04, 0x21,
			0x57, 0xCA, 0x00, 0x3E, 0x2F, 0x0A, 0x00, 0x00, 0x40, 0x99, 0x00, 0x2C,
			0x7D, 0x49, 0x03, 0xA6, 0x7F, 0xEB, 0xFB, 0x78, 0x7D, 0x5F, 0x18, 0x50,
			0x7D, 0x2A, 0x58, 0xAE, 0x99, 0x2B, 0x00, 0x00, 0x7C, 0x00, 0x58, 0x6C,
			0x7C, 0x00, 0x04, 0xAC, 0x4C, 0x00, 0x01, 0x2C, 0x39, 0x6B, 0x00, 0x01,
			0x42, 0x00, 0xFF, 0xE8, 0x38, 0x60, 0x00, 0x00, 0x48, 0x00, 0x00, 0x08,
			0x78, 0x63, 0x00, 0x20, 0x38, 0x21, 0x00, 0x70, 0x81, 0x81, 0xFF, 0xF8,
			0x7D, 0x88, 0x03, 0xA6, 0xEB, 0xC1, 0xFF, 0xE8, 0xEB, 0xE1, 0xFF, 0xF0,
			0x4E, 0x80, 0x00, 0x20
	};

	if (!XeLive.bDevkit) {
		*(short*)(szPeekPoke + 0xB6) = 0x29D8;
		*(short*)(szPeekPoke + 0x16E) = 0x29D8;
	}

	LOG_PRINT("Initializing #39829ea5");
	auto payload = PayloadBuilder::ExpansionInstall(szPeekPoke, 472, HvPeekPokeExpID);
	if (payload) {
		LOG_PRINT("Initializing #fe65cb3es");
		auto result = (HRESULT)HvxExpansionInstall((DWORD)Native::MmGetPhysicalAddress(payload), 0x1000);
		Native::XPhysicalFree(payload);
		return result;
	}

	return S_FAIL;
}

BYTE Hypervisor::HvPeekBYTE(QWORD address) {
	LOG_DEV("HvPeekBYTE on %I64X", address);
	return (BYTE)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTE, address, 0, 0);
}

WORD Hypervisor::HvPeekWORD(QWORD address) {
	LOG_DEV("HvPeekWORD on %I64X", address);
	return (WORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_WORD, address, 0, 0);
}

DWORD Hypervisor::HvPeekDWORD(QWORD address) {
	LOG_DEV("HvPeekDWORD on %I64X", address);
	return (DWORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_DWORD, address, 0, 0);
}

QWORD Hypervisor::HvPeekQWORD(QWORD address) {
	LOG_DEV("HvPeekQWORD on %I64X", address);
	return HvxExpansionCall(HvPeekPokeExpID, PEEK_QWORD, address, 0, 0);
}

HRESULT Hypervisor::HvPeekBytes(QWORD address, void* pBuffer, DWORD size) {
	LOG_DEV("HvPeekBytes on %I64X", address);

	VOID* data = Native::XPhysicalAlloc(size, MAXULONG_PTR, 0, PAGE_READWRITE);
	ZeroMemory(data, size);

	HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTES, address, (QWORD)Native::MmGetPhysicalAddress(data), size);

	if (result == S_OK) memcpy(pBuffer, data, size);

	Native::XPhysicalFree(data);
	return result;
}

HRESULT Hypervisor::HvPokeBYTE(QWORD address, BYTE value) {
	LOG_DEV("HvPokeBYTE on %I64X", address);
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTE, address, value, 0);
}

HRESULT Hypervisor::HvPokeWORD(QWORD address, WORD value) {
	LOG_DEV("HvPokeWORD on %I64X", address);
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_WORD, address, value, 0);
}

HRESULT Hypervisor::HvPokeDWORD(QWORD address, DWORD value) {
	LOG_DEV("HvPokeDWORD on %I64X", address);
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_DWORD, address, value, 0);
}

HRESULT Hypervisor::HvPokeQWORD(QWORD address, QWORD value) {
	LOG_DEV("HvPokeQWORD on %I64X", address);
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_QWORD, address, value, 0);
}

HRESULT Hypervisor::HvPokeBytes(QWORD address, const void* pBuffer, DWORD size) {
	LOG_DEV("HvPokeBytes on %I64X", address);
	VOID* data = Native::XPhysicalAlloc(size, MAXULONG_PTR, 0, PAGE_READWRITE);
	memcpy(data, pBuffer, size);

	HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTES, address, (QWORD)Native::MmGetPhysicalAddress(data), size);

	Native::XPhysicalFree(data);
	return result;
}

QWORD Hypervisor::HvGetFuseLine(BYTE fuseIndex) {
	if (fuseIndex > 11) return 0;
	return HvPeekQWORD(0x8000020000020000 + (fuseIndex * 0x200));
}

BYTE* Hypervisor::GetHypervisorCPU() {
	BYTE cpukey[0x10];
	HvPeekBytes(0x20, cpukey, 0x10);
	return cpukey;
}

BYTE* Hypervisor::GetFuseCPU() {
	BYTE cpukey[0x10];
	UINT64 f1p1 = Native::HvGetFuseLine(3);
	UINT64 f1p2 = Native::HvGetFuseLine(5);
	memcpy(cpukey, &f1p1, 0x08);
	memcpy(cpukey + 0x08, &f1p2, 0x08);
	return cpukey;
}

BYTE* Hypervisor::GetConsoleType() {
	BYTE version[0x4];
	DWORD val = HvPeekDWORD(0x14);
	memcpy(version, &val, 0x4);
	return version;
}

BYTE* Hypervisor::GetUpdateSequence() {
	BYTE version[0x4];
	memcpy(version, &Data.dwUpdateSequence, 0x4);
	return version;
}