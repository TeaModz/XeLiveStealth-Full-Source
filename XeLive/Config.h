#pragma once

class Config {
public:
	static map<DWORD, bool> Cheats;
	static map<DWORD, bool> Bypasses;

	static bool bBypassAllUI;
	static bool bCustomColors;
	static bool bCustomDashboard;
	static bool bCustomGuide;
	static bool bGuideInfo;
	static bool bCustomNotify;
	static bool bUsingNoKV;

	static char szXShellEmail[0xC];
	static char szXShellPassword[0xB];

	static HRESULT Initialize();
	static void UpdateConfig();
	static void InstallDefaultConfig();
};