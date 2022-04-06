#pragma once

class Utilities {
public:
	bool FileExists(const char* file);
	HRESULT DoMountPath(const char* szDrive, const char* szDevice, const char* sysStr);
	HRESULT MountPath(const char* szDrive, const char* szDevice, bool both);
	const char* GetMountPath();
	char* va(const char* fmt, ...);
	bool WriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size);
};

extern Utilities pUtils;