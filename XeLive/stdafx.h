#pragma once

#define _USE_MATH_DEFINES

#pragma warning(disable : 4244)
#pragma warning(disable : 4172)
#pragma warning(disable : 4800)
#pragma warning(disable : 4018)
#pragma warning(disable : 4554)
#pragma warning(disable : 4826)
#pragma warning(disable : 4293)
#pragma warning(disable : 4307)

#include <xtl.h>
#include <xboxmath.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <time.h>
#include <vector>
#include <array>
#include <ppcintrinsics.h>
#include <math.h>
#include <fstream>
#include <cstdio>
#include <tchar.h>
#include <xui.h>
#include <xuiapp.h>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <xhttp.h>
#include <string>
#include <xaudio2.h>
#include <xact3wb.h>
#include <functional>
#include "xkelib.h"

using namespace std;


#define CURRENT_XEX_SERVER_BUILD 100
#define DASH_HACKS
#define S_FAIL -1
#define KERNEL_VERSION 0x4497
#define NUMOF(x) (sizeof(x)/sizeof(x[0]))

#define LOCAL_SERVER
#define DEVELOPMENT_BUILD

#define VERIFY_CALLER \
	DWORD dwLR = 0; \
	__asm { mflr dwLR } \
	DWORD myModuleBase = (DWORD)XeLive.pDataTableEntry->ImageBase; \
	DWORD myModuleEnd = myModuleBase + XeLive.pDataTableEntry->SizeOfFullImage; \
	if (!(dwLR > myModuleBase && dwLR < myModuleEnd)) \
		ProcessMetric(new MetricIntegrityCheckFailed());

/* Utils */
#include "Structs.h"
#include "Log.h"
#include "Patches.h"
#include "Global.h"
#include "Memory.h"
#include "Utils.h"
#include "Notify.h"
#include "INI.h"
#include "Config.h"
#include "UIStructs.h"

/*Security - Metrics*/
#include "Metric.h"
#include "metricIntegrityCheckFailed.h"
#include "metricBreakpoint.h"
#include "metricModuleDigestMismatch.h"

/*Security - Interfaces*/
#include "InterfaceRegistration.h"
#include "CheatInterface.h"

/* Security */
#include "NativeInvoker.h"
#include "Natives.h"
#include "FunctionObfuscation.h"
#include "Security.h"
#include "IntegrityManager.h"


/* Network */
#include "Requests.h"
#include "Network.h"

/* System */
#include "PayloadBuilder.h"
#include "CPU.h"
#include "SMC.h"
#include "Launch.h"
#include "Hypervisor.h"
#include "Keyvault.h"
#include "CrashHandler.h"

/*Scenes*/
#include "ScnGuideInfo.h"
#include "XeLiveTabScene.h"
#include "XeLiveMainScene.h"

/* Hooking */
#include "Hooking.h"

/* Hooks */
#include "AntiCheatHook.h"
#include "TitleHooks.h"
#include "SystemHooks.h"
#include "HudHooks.h"
#include "DashboardHooks.h"