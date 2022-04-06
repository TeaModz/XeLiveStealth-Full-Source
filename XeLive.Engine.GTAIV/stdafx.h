#pragma once
#define _USE_MATH_DEFINES

#pragma warning(disable : 4244)
#pragma warning(disable : 4172)
#pragma warning(disable : 4800)
#pragma warning(disable : 4018)

/*Required game information*/
#define GAME_NAME "GTA IV"
#define GAME_TITLE 0x545407F2

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
#include <sstream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <xhttp.h>
#include <string>
#include <xaudio2.h>
#include <xact3wb.h>
#include <functional>
#include <unordered_map>

#include "xkelib.h"

using namespace std;

#define NUMOF(x) (sizeof(x)/sizeof(x[0]))

#define MOUNT_PATH "XELIVE:"
#define CACHE_PATH MOUNT_PATH "\\XeLive Cheat Cache\\"
#define GAME_PATH CACHE_PATH GAME_NAME "\\"
#define ONLOAD_INI_PATH CACHE_PATH GAME_NAME "-XeLive.ini"
#define LOG_PATH CACHE_PATH GAME_NAME "-XeLive.log"
#define DEVICE_HDD "\\Device\\Harddisk0\\Partition1\\"
#define DEVICE_USB "\\Device\\Mass0"

#define S_FAIL -1

/*Utils*/
#include "Log.h"
#include "Global.h"
#include "Utils.h"

/*Security*/
#include "nativeInvoker.h"
#include "natives.h"
#include "functionObfuscation.h"
#include "Interface.h"

/*Backend*/
#include "Hooking.h"
#include "SystemHooks.h"

/*Menu*/
#include "Game.h"