#pragma once

struct once_flag {
	bool bDone;
};

static void call_once(once_flag* flag, function<void()> func) {
	if (flag) {
		if (!flag->bDone) {
			if (func) {
				func();
			}

			flag->bDone = true;
		}
	}
}

class Invoke {
public:
	template<typename T>
	static T Call(DWORD dwAddress) { return ((T(*)())dwAddress)(); }

	template<typename T, typename P1>
	static T Call(DWORD dwAddress, P1 p1) { return ((T(*)(P1))dwAddress)(p1); }

	template<typename T, typename P1, typename P2>
	static T Call(DWORD dwAddress, P1 p1, P2 p2) { return ((T(*)(P1, P2))dwAddress)(p1, p2); }

	template<typename T, typename P1, typename P2, typename P3>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3) { return ((T(*)(P1, P2, P3))dwAddress)(p1, p2, p3); }

	template<typename T, typename P1, typename P2, typename P3, typename P4>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4) { return ((T(*)(P1, P2, P3, P4))dwAddress)(p1, p2, p3, p4); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) { return ((T(*)(P1, P2, P3, P4, P5))dwAddress)(p1, p2, p3, p4, p5); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) { return ((T(*)(P1, P2, P3, P4, P5, P6))dwAddress)(p1, p2, p3, p4, p5, p6); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) { return ((T(*)(P1, P2, P3, P4, P5, P6, P7))dwAddress)(p1, p2, p3, p4, p5, p6, p7); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) { return ((T(*)(P1, P2, P3, P4, P5, P6, P7, P8))dwAddress)(p1, p2, p3, p4, p5, p6, p7, p8); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) { return ((T(*)(P1, P2, P3, P4, P5, P6, P7, P8, P9))dwAddress)(p1, p2, p3, p4, p5, p6, p7, p8.p9); }

	template<typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
	static T Call(DWORD dwAddress, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10) { return ((T(*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10))dwAddress)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
};

class MemoryBuffer {
public:
	MemoryBuffer(DWORD Size = 0x200)
	{
		m_Buffer = 0;
		m_DataLength = 0;
		m_BufferSize = 0;
		if ((Size != 0) && (Size < UINT_MAX))
		{
			m_Buffer = (PBYTE)malloc(Size + 1);
			if (m_Buffer)
			{
				m_BufferSize = Size;
				m_Buffer[0] = 0;
			}
		}
	}
	~MemoryBuffer()
	{
		if (m_Buffer) free(m_Buffer);
		m_Buffer = 0;
		m_DataLength = 0;
		m_BufferSize = 0;
	}
	BOOL CheckSize(DWORD Size)
	{
		if (m_BufferSize >= (m_DataLength + Size)) return TRUE;
		else
		{
			DWORD NewSize = max((m_DataLength + Size), (m_BufferSize * 2));
			PBYTE NewBuffer = (PBYTE)realloc(m_Buffer, NewSize + 1);
			if (NewBuffer)
			{
				m_BufferSize = NewSize;
				m_Buffer = NewBuffer;
				return TRUE;
			}
			else return FALSE;
		}
	}
	VOID Add(CONST PVOID Buffer, DWORD Size)
	{
		if (CheckSize(Size))
		{
			memcpy(m_Buffer + m_DataLength, Buffer, Size);
			m_DataLength += Size;
			*(m_Buffer + m_DataLength) = 0;
		}
	}
	DWORD GetSize() CONST { return m_DataLength; }
	PBYTE GetBuffer() CONST { return m_Buffer; }
private:
	PBYTE m_Buffer;
	DWORD m_DataLength;
	DWORD m_BufferSize;
};

class Utils {
public:
	static char* va(const char* fmt, ...);
	static PWCHAR vaw(const char* Text, ...);
	static char* vaBuff(char* vaBuffer, int size, const char* fmt, ...);
	static HANDLE ResolveFunction(const char* pModuleName, DWORD dwOrdinal);
	static DWORD DecryptValue(DWORD val);
	static HRESULT DoMountPath(const char* szDrive, const char* szDevice, const char* sysStr);
	static HRESULT MountPath(const char* szDrive, const char* szDevice, bool both);
	static const char* GetMountPath();
	static HRESULT ApplyPatchData(DWORD* patches, size_t dataSize);
	static bool FileExists(const char* file);
	static bool WriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size);
	static DWORD Joaat(const char* value);
	static void EnsureRape();
	static BYTE CharToByte(char input);
	static char* ReadFileBytes(const char* name);
	static BOOL CReadFile(CONST PCHAR FilePath, MemoryBuffer& Buffer);
	static BOOL BufferIsNull(PBYTE Buffer, int Size);
	static void LaunchDashboard();
	static char GenerateRandomChar();
	static char GenerateRandomChar2();
	static void XeCryptRandom2(PBYTE buffer, int size);
	static SIZE_T GetAvailableRam();
	static BOOL IsBufferEmpty(BYTE* pBuffer, DWORD length);
	static BOOL GetSectionInfo(const char* SectionName, DWORD* Address, DWORD* Length);
	static FARPROC ResolveFunction(HMODULE hHandle, DWORD Ordinal);
	static string sprintfaA(const char* format, ...);
	static HRESULT SetMemory(void* destination, void* buffer, DWORD size);
	static bool IsUserSignedIn(int playerIndex);
	static HRESULT GetGamerPicture(int playerIndex, PBYTE ret, BOOL small, DWORD pitch, DWORD height);
	static HRESULT ForceSignIn(int dwPlayerIndex, XUID profileXuid);
	static HRESULT ForceSignOut(int dwPlayerIndex);
	static int GetGamerXUID(int playerIndex, PXUID ret);
};

