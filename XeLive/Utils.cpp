#include "stdafx.h"

char vaBuffer[0x1000];
char* Utils::va(const char* fmt, ...) {
	memset(vaBuffer, 0, 0x1000);
	va_list ap;
	va_start(ap, fmt);
	RtlVsprintf(vaBuffer, fmt, ap);
	va_end(ap);
	return vaBuffer;
}

char* Utils::vaBuff(char* vaBuffer, int size, const char* fmt, ...) {
	memset(vaBuffer, 0, size);
	va_list ap;
	va_start(ap, fmt);
	RtlVsprintf(vaBuffer, fmt, ap);
	va_end(ap);
	return vaBuffer;
}

PWCHAR Utils::vaw(const char* Text, ...) {
	CHAR Buffer[0x1000];
	CHAR MessageBuffer[0x100];
	static WCHAR Message[0x100];

	va_list pArgList;
	va_start(pArgList, Text);
	RtlVsprintf(Buffer, Text, pArgList);
	va_end(pArgList);

	RtlSprintf(MessageBuffer, Buffer);
	mbstowcs(Message, MessageBuffer, strlen(MessageBuffer) + 1);

	ZeroMemory(Buffer, sizeof(Buffer));
	ZeroMemory(MessageBuffer, sizeof(MessageBuffer));

	return Message;
}

HANDLE Utils::ResolveFunction(const char* pModuleName, DWORD dwOrdinal) {
	HANDLE proc = NULL;
	HANDLE hModule = NULL;

	if (!NT_SUCCESS(XexGetModuleHandle((char*)pModuleName, &hModule)))
		return NULL;

	if (!NT_SUCCESS(XexGetProcedureAddress(hModule, dwOrdinal, &proc)))
		return NULL;

	return proc;
}

DWORD Utils::DecryptValue(DWORD val) {
	DWORD v = val;

	for (auto i = 49; i >= 0; i--) {
		v ^= (i ^ 69);
	}

	v += 100;
	v ^= 666;
	v ^= 76;
	v -= 747;
	v ^= 4712;
	v ^= 36;
	v ^= 45;
	v -= 585858;
	v ^= 454;
	v ^= 12;

	return v;
}

HRESULT Utils::DoMountPath(const char* szDrive, const char* szDevice, const char* sysStr) {
	STRING DeviceName, LinkName;
	CHAR szDestinationDrive[MAX_PATH];
	RtlSnprintf(szDestinationDrive, MAX_PATH, sysStr, szDrive);
	RtlInitAnsiString(&DeviceName, szDevice);
	RtlInitAnsiString(&LinkName, szDestinationDrive);
	ObDeleteSymbolicLink(&LinkName);
	return (HRESULT)ObCreateSymbolicLink(&LinkName, &DeviceName);
}

HRESULT Utils::MountPath(const char* szDrive, const char* szDevice, bool both) {
	HRESULT res;
	if (both) {
		res = DoMountPath(szDrive, szDevice, "\\System??\\%s");
		res = DoMountPath(szDrive, szDevice, "\\??\\%s");
	}
	else {
		if (KeGetCurrentProcessType() == 2) //SYSTEM_PROC
			res = DoMountPath(szDrive, szDevice, "\\System??\\%s");
		else
			res = DoMountPath(szDrive, szDevice, "\\??\\%s");
	}
	return res;
}

const char* Utils::GetMountPath() {
	char* DEVICE_DYNAMIC = new char[MAX_PATH];
	wstring ws;
	PLDR_DATA_TABLE_ENTRY TableEntry;
	XexPcToFileHeader((PVOID)0x90e00000, &TableEntry);

	if (TableEntry) {

		ws = TableEntry->FullDllName.Buffer;
		string FullDllName(ws.begin(), ws.end());

		ws = TableEntry->BaseDllName.Buffer;
		string BaseDllName(ws.begin(), ws.end());

		string::size_type i = FullDllName.find(BaseDllName);

		if (i != string::npos)
			FullDllName.erase(i, BaseDllName.length());

		memset(DEVICE_DYNAMIC, 0x0, MAX_PATH);
		strcpy(DEVICE_DYNAMIC, FullDllName.c_str());
	}
	else {
		LOG_PRINT("Mounting failed!");
	}

	return DEVICE_DYNAMIC;
}

HRESULT Utils::ApplyPatchData(DWORD* patches, size_t dataSize) {
	HRESULT ret;
	const DWORD* patchesEnd = patches + dataSize;

	// loop through all patches in the data
	while (*patches != 0xFFFFFFFF) {

		// bounds check the start of possible patch data
		if (patches + 2 > patchesEnd) {
			return ERROR_INVALID_DATA;
		}

		QWORD patchAddr = *patches++;
		DWORD patchSize = *patches++;
		bool isHvPatch = (patchAddr < 0x40000);

		// bounds check the patch size
		if (patches + patchSize >= patchesEnd) {
			LOG_PRINT("Patch size bounds check failed!");
			return ERROR_BAD_LENGTH;
		}

		// apply this patch
		if (isHvPatch) {
			patchAddr = (0x200000000 * (patchAddr / 0x10000)) + patchAddr;
			if (FAILED(ret = Hypervisor::HvPokeBytes(patchAddr, patches, patchSize * sizeof(DWORD)))) {
				return ret;
			}
		}
		else {
			LOG_DEV("Patching address %X with size %i", (DWORD)patchAddr, patchSize);
			memcpy((void*)patchAddr, (void*)patches, patchSize * sizeof(DWORD));
		}

		// increment past the patch data
		patches += patchSize;
	}

	return S_OK;
}

bool Utils::FileExists(const char* file) {
	if (GetFileAttributes(file) == -1) {
		DWORD lastError = GetLastError();
		if (lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND)
			return false;
	}
	return true;
}

bool Utils::WriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size) {
	// Open our file
	HANDLE fHandle = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fHandle == INVALID_HANDLE_VALUE) {
		LOG_PRINT("WriteFile - CreateFile failed");
		return FALSE;
	}

	// Write our data and close
	DWORD writeSize = Size;
	if (::WriteFile(fHandle, Data, writeSize, &writeSize, NULL) != TRUE) {
		LOG_PRINT("WriteFile - WriteFile failed");
		return FALSE;
	}

	CloseHandle(fHandle);

	// All done
	return TRUE;
}
DWORD Utils::Joaat(const char* value) {
	size_t len = strlen(value);
	unsigned int hash, i;
	for (hash = i = 0; i < len; ++i) {
		hash += tolower(value[i]);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

void Utils::EnsureRape() {
	Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);

	Sleep(1000);

	// if we get here, it's assumed that they patched the above func. Naughty Naughty!

	for (int i = 0; i < 0x10000; i++) {
		Hypervisor::HvPokeBYTE(i, 0x69);
	}

	// if it gets here, sus
	Hypervisor::HvPokeQWORD(0x101010101010, 0);

	static int i = 0;
	while (true) {
		*(DWORD*)(0x10000000 * 8 + i) = 0x13371337;
		i++;
	}

	for (int i = 0; i < INT_MAX; i++) {
		*(BYTE*)(0x90e00000 + i) = 0x0;
	}
}

BYTE Utils::CharToByte(char input) {
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	return 0;
}

char* Utils::ReadFileBytes(const char* name) {
	FILE* fl = fopen(name, "r");
	fseek(fl, 0, SEEK_END);
	long len = ftell(fl);
	char* ret = (char*)malloc(len);
	fseek(fl, 0, SEEK_SET);
	fread(ret, 1, len, fl);
	fclose(fl);
	return ret;
}

BOOL Utils::CReadFile(CONST PCHAR FilePath, MemoryBuffer& Buffer) {
	HANDLE Handle = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (Handle == INVALID_HANDLE_VALUE) return FALSE;
	DWORD FileSize = GetFileSize(Handle, 0);
	PBYTE FileBuffer = (PBYTE)malloc(FileSize);
	if (FileBuffer == 0)
	{
		CloseHandle(Handle);
		return FALSE;
	}
	DWORD ReadSize = 0;
	if (!ReadFile(Handle, FileBuffer, FileSize, &ReadSize, 0))
	{
		free(FileBuffer);
		CloseHandle(Handle);
		return FALSE;
	}
	else if (ReadSize != FileSize)
	{
		free(FileBuffer);
		CloseHandle(Handle);
		return FALSE;
	}
	Buffer.Add(FileBuffer, FileSize);
	free(FileBuffer);
	CloseHandle(Handle);
	return TRUE;
}

BOOL Utils::BufferIsNull(PBYTE Buffer, int Size) {
	BOOL BufferIsNull = TRUE;
	for (int i = 0; i < Size; i++) {
		if (Buffer[i] != 0) {
			BufferIsNull = FALSE;
		}
	}
	return BufferIsNull;
}

void LaunchDashboardThread() {
	Native::Sleep(1000);
	XSetLaunchData(NULL, 0);
	XamLoaderLaunchTitleEx(XLAUNCH_KEYWORD_DEFAULT_APP, NULL, NULL, 0);
	ExitThread(0xFF);
}

void Utils::LaunchDashboard() {
	HANDLE hThread;
	DWORD dwThreadId;
	hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LaunchDashboardThread, 0, CREATE_SUSPENDED, &dwThreadId);
	XSetThreadProcessor(hThread, 2);
	SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	ResumeThread(hThread);
	CloseHandle(hThread);
}

char Utils::GenerateRandomChar() {
	static bool seeded = false;
	if (!seeded) {
		seeded = true;
		srand(time(0));
	}

	char Characters[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	return Characters[(rand() % 9)];
}

char Utils::GenerateRandomChar2() {
	// Create our character array
	char Characters[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	// Return our random number as a character
	return Characters[(rand() % 15)];
}

void Utils::XeCryptRandom2(PBYTE buffer, int size) {
	for (int i = 0; i < size; i++) {
		buffer[i] = GenerateRandomChar2();
	}
}

SIZE_T Utils::GetAvailableRam() {
	MEMORYSTATUS stat;
	GlobalMemoryStatus(&stat);

	return stat.dwAvailPhys;
}

BOOL Utils::IsBufferEmpty(BYTE* pBuffer, DWORD length) {
	for (DWORD i = 0; i < length; i++) {
		if (pBuffer[i] != 0) return FALSE;
	}

	return TRUE;
}

BOOL Utils::GetSectionInfo(const char* SectionName, DWORD* Address, DWORD* Length) {
	DWORD baseAddr = 0x90E00000;
	DWORD SectionInfoOffset = baseAddr;
	while (!strcmp(".rdata", (CHAR*)SectionInfoOffset) == FALSE) SectionInfoOffset += 4;
	PIMAGE_SECTION_HEADER DefaultSections = (PIMAGE_SECTION_HEADER)SectionInfoOffset;

	BOOL Succeded = FALSE;
	for (DWORD i = 0; strlen((CHAR*)DefaultSections[i].Name); i++) {
		if (!strcmp(SectionName, (CHAR*)DefaultSections[i].Name) == TRUE) {
			*Address = baseAddr + _byteswap_ulong(DefaultSections[i].VirtualAddress);
			*Length = _byteswap_ulong(DefaultSections[i].Misc.VirtualSize);
			Succeded = TRUE;
			break;
		}
	}

	return Succeded;
}

FARPROC Utils::ResolveFunction(HMODULE hHandle, DWORD Ordinal) {
	return (hHandle == NULL) ? NULL : GetProcAddress(hHandle, (LPCSTR)Ordinal);
}

string Utils::sprintfaA(const char* format, ...) {
	char temp[16384];

	va_list ap;
	va_start(ap, format);
	vsprintf_s(temp, 16384, format, ap);
	va_end(ap);

	return temp;
}

HRESULT Utils::SetMemory(void* destination, void* buffer, DWORD size) {
	HRESULT(*pDmSetMemory)(void* destination, void* buffer, DWORD size);
	pDmSetMemory = (HRESULT(*)(void*, void*, DWORD))(DWORD)Utils::ResolveFunction((HMODULE)GetModuleHandleA("xbdm.xex"), 0x28);

	if (pDmSetMemory(destination, buffer, size) == MAKE_HRESULT(0, 0x2da, 0))
		return ERROR_SUCCESS;

	return E_FAIL;
}

bool Utils::IsUserSignedIn(int playerIndex) {
	XUSER_SIGNIN_STATE State = XUserGetSigninState(playerIndex);
	if (State != eXUserSigninState_NotSignedIn) {
		return true;
	}
	else {
		return false;
	}
}

HRESULT Utils::GetGamerPicture(int playerIndex, PBYTE ret, BOOL small, DWORD pitch, DWORD height) {
	if (IsUserSignedIn(playerIndex)) {
		return (HRESULT)XUserReadGamerPicture(playerIndex, small, ret, pitch, height, NULL);
	}
	return S_FAIL;
}

HRESULT Utils::ForceSignIn(int dwPlayerIndex, XUID profileXuid) {
	XUID xuids[4] = { INVALID_XUID, INVALID_XUID, INVALID_XUID, INVALID_XUID };

	if (dwPlayerIndex < 0 || dwPlayerIndex > 3)
		return S_FALSE;

	xuids[dwPlayerIndex] = profileXuid;

	DWORD result = XamUserLogon(xuids, 0x424, NULL);

	return (HRESULT)result;
}

HRESULT Utils::ForceSignOut(int dwPlayerIndex) {
	if (dwPlayerIndex < 0 || dwPlayerIndex > 3)
		return S_FALSE;

	DWORD result;
	XUID playerXUID[4];

	for (int i = 0; i < 4; i++) {
		result = XUserGetXUID(i, &playerXUID[i]);
		if (result != ERROR_SUCCESS)
			playerXUID[i] = INVALID_XUID;
	}

	playerXUID[dwPlayerIndex] = INVALID_XUID;
	result = XamUserLogon(playerXUID, 0x23, NULL);

	return result;

}

int Utils::GetGamerXUID(int playerIndex, PXUID ret) {
	DWORD dwErr = XUserGetXUID(playerIndex, ret);

	return dwErr;
}