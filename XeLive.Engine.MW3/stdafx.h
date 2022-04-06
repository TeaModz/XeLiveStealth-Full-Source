#pragma once
#define _USE_MATH_DEFINES

/*Required game information*/
#define GAME_NAME "MW3"
#define GAME_TITLE 0x415608CB

#pragma warning(disable : 4244)
#pragma warning(disable : 4172)
#pragma warning(disable : 4800)
#pragma warning(disable : 4018)

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
#define INI_PATH CACHE_PATH GAME_NAME "-XeLive.ini"
#define LOG_PATH CACHE_PATH GAME_NAME "-XeLive.log"
#define DEVICE_HDD "\\Device\\Harddisk0\\Partition1\\"
#define DEVICE_USB "\\Device\\Mass0"

/*Utils*/
#include "enums.h"
#include "structs.h"
#include "log.h"
#include "patches.h"
#include "global.h"
#include "utils.h"

/*Security*/
#include "nativeInvoker.h"
#include "natives.h"

#include "interface.h"

/*engine*/
#include "Engine.h"

/*renderer*/
#include "Renderer.h"

/*Input*/
#include "controller.h"

/*Backend*/
#include "hooking.h"
#include "systemHooks.h"
#include "titleHooks.h"

/*options*/
#include "Option.h"
#include "buttonOption.h"
#include "toggleOption.h"
#include "numberOption.h"
#include "scrollOption.h"
#include "submenuOption.h"

/*Submenu*/
#include "Submenu.h"
#include "SubmenuHandler.h"

/*Menu*/
#include "Game.h"
#include "Menu.h"
#include "MenuFunctions.h"