#pragma once
class Global {
public:
	static HRESULT Initialize();

	bool bDevkit;
	bool bLoadedProperly;
	bool bConnectedToServerInit;
	bool bCanNotify;

	DWORD dwTickCount;
	HANDLE hMyModule;
	HANDLE dHandle;
	PLDR_DATA_TABLE_ENTRY pDataTableEntry;
	XEX_EXECUTION_ID ExecutionIDSpoof;

	BYTE szCPU[0x10];
	BYTE szCPUBinKey[0x10];
	BYTE szCPUDigest[0x14];
	BYTE szCPUFuseDigest[0x14];
	BYTE szConsoleKey[0x14];
	BYTE szSessionToken[0x10];
	BYTE szModuleDigest[0x14];

	bool bAccountBanned;
	char szAccountBannedMessage[0x100];

	int iTotalDaysOnKV;
	int iLastUsedVersion;

	/*Title plugin info*/
	vector<ServerXexInfo> PluginData;

	/*Challenge info*/
	bool bFCRT;
	bool bCRL;
	bool bTypeOneKV;
	int iTotalChallengesPassed;
	int iTotalChallengesPassedOnThisKV;
	DWORD dwUpdateSequence;

	Addresses* Address;

	BOOL GetPluginData(DWORD dwTitle, DWORD dwTimestamp, ServerXexInfo* pOut);
};

extern Global XeLive;