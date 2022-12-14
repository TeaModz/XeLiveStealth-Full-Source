#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "BO2.h"
#include "Detour.h"

DWORD scroll = 0;
DWORD subScroll = 0;
BOOL AButton;
BOOL BButton;
BOOL menuOpen;
BOOL menuWait;
BOOL subMenu;
BOOL clientMenu;
int selectedClient;
const int MenuOptions = 7;
int SubMenuOptions[MenuOptions];
DWORD ArrowIcon;

BOOL DPads[4];

DWORD OptionEnabled[18][MenuOptions];
DWORD ClientOptionEnabled[10][18];
PCHAR OptionStrings[18][MenuOptions];
PCHAR* OptionValues[18][MenuOptions];
int OptionArraySize[18][MenuOptions];
int OptionArrayCurrent[18][MenuOptions];

PCHAR SubMenuNames[18][MenuOptions];

pclientActive_t_BO2 LocalClient_BO2;
pcentity_s_BO2 Entities_BO2;
pcgs_t_BO2 cgs_BO2;
pcg_s_BO2 cg_BO2;
ClientInformation_BO2 PlayerInfo[1024];

Detour<void> *Menu_PaintAllDetour;
Detour<DWORD> *XInputGetStateDetour;
Detour<int> *CL_SendCmdDetour;
Detour<void> *CL_CreateNewCommandsDetour;
Detour<void> *PlayLocalSoundAliasByNameDetour;
Detour<void> *R_EndFrameDetour;
Detour<DWORD> *NetDll_XNetGetTitleXnAddrDetour;
Detour<void> *CG_EntityEventDetour;
Detour<void> *R_DrawXModelSkinnedCachedDetour;
Detour<HRESULT> *R_DrawIndexedPrimitiveDetour;
Detour<HRESULT> *DrawIndexedPrimitiveDetour;
Detour<void> *EngineDrawRadarDetour;
Detour<DWORD> *PreGameAntiFreezeDetour;
Detour<DWORD> *GameMessageOneDetour;
Detour<DWORD> *GameMessageTwoDetour;
Detour<DWORD> *ParseKillFeedTextDetour;
Detour<DWORD> *sub_821D8A18Detour;
Detour<void> *DynEntCl_DestroyEventDetour;
Detour<DWORD> *sub_82267520Detour;
Detour<void> *XMemCpyDetour;

BOOL menuReady = FALSE;
BOOL DroppedAllTargets = FALSE;
BOOL OnInit = FALSE;
BOOL SpinbotFlip = FALSE;
BOOL SpinbotFlipX = FALSE;
BOOL WasReloading = FALSE;
BOOL YButtonPressed = FALSE;
BOOL FoundTarget = FALSE;
BOOL SetXUIDOnce = FALSE;
BOOL FlipRapidFire = FALSE;
BOOL ShouldSendPacket = TRUE;
BOOL HearEveryoneValue = FALSE;
BOOL AutoCrouched = FALSE;
BOOL LobbyInit = FALSE;
BOOL Zombies = FALSE;
float rotation = 0;
float dx, dy, dz;
float SpinbotSpin = 5;
float SpinbotSpinFlip = 0;
float SpinbotSpinZ = 5;
float SpinbotSpinZR = 5;
int ClientName = 0;
int NameStealDelay = 0; 
int RainbowGTDelay = 0;
int PrestigeDiscoDelay = 0;
int UpdateIPLogDelay = 0;
int ClosestClient;
int FakeLagCount = 0;
int PacketSkipAmount = 26;
int HudSmallFont;
int BigDevFont;
int SmallFont;
int ExtraSmallFont;
int ExtraBigDevFont;
int WhiteMaterial;
int ScavMaterial;
int CompassPingMaterial;
int GradientFadeIn;
int yyCount = 0;
int BlacklistCtr = 0;
int RapidFireCtr = 0;
int TriggerValue = 0;
int SpinbotHealthSave = 0;
int GClosestClient = -1;
int CrouchCounter = 0;
bool Crouching = false;
int OurHealth = 0;
short BoneIndexs[21];

bool flip;
bool flip2;

char RandomXUID[0xC];
char OriginalXUID[0xE];

wchar_t wcOldGT[0x30];
char OriginalGT[0x1F];

float menuWidth = 450.0f;
float menuHeight = 410.0f;

D3DTexture* OrangeTexture = NULL;
D3DTexture* LightBlueTexture = NULL;
MemoryBuffer* IPBuffer;
DWORD GReactionDelay = 0;
PCHAR AimTag;

DWORD LastCompassType = 0;
DWORD DrawCompassTimer = 0;
DWORD screenMaxX = NULL, screenMaxY = NULL;

CONST PCHAR GetMenuOptionString(eMenuNames SubMenu, PCHAR Name)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option string\n");
		return "null";
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(Name, SubMenuNames[i][Tabindex]) && (OptionValues[i][Tabindex] != NULL))
		{
			return OptionValues[i][Tabindex][OptionArrayCurrent[i][Tabindex]];
		}

		if (!strcmp(Name, SubMenuNames[i][Tabindex]))
		{
			return OptionStrings[i][Tabindex];
		}
	}
	return "null";
}

CONST PCHAR GetMenuOptionString(eMenuNames SubMenu, eMenuOptions Name)
{
	return GetMenuOptionString(SubMenu, MenuOptionNames[Name]);
}

int GetMenuOptionArrayIndex(eMenuNames SubMenu, eMenuOptions Name)
{
	if (!menuReady) return 0;
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option array index\n");
		return 0;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			return OptionArrayCurrent[i][Tabindex];
		}
	}
	return 0;
}

int GetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name)
{
	if (!menuReady) return 0;
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option value\n");
		return 0;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			return OptionEnabled[i][Tabindex];
		}
	}
	return 0;
}

void SetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name, DWORD ValueToSet)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to set menu option value\n");
		return;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			OptionEnabled[i][Tabindex] = ValueToSet;
		}
	}
	return;
}

void SetMenuOptionString(eMenuNames SubMenu, eMenuOptions Name, DWORD ArrayNumber)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to set menu option value\n");
		return;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]) && (OptionValues[i][Tabindex] != NULL))
			{
				OptionArrayCurrent[i][Tabindex] = ArrayNumber;
			}
		}
	}
	return;
}

void AddMenuOption(eMenuNames SubMenu, eMenuOptions Name, int index, BOOL defaultValue = FALSE, CONST PCHAR defaultString = "false", char** Array = NULL, int ArraySize = NULL, int ArrayCur = 0)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to add menu option\n");
		return;
	}

	OptionStrings[index][Tabindex] = defaultString;
	OptionEnabled[index][Tabindex] = defaultValue;
	OptionValues[index][Tabindex] = Array;
	OptionArraySize[index][Tabindex] = ArraySize;
	OptionArrayCurrent[index][Tabindex] = ArrayCur;
	SubMenuNames[index][Tabindex] = MenuOptionNames[Name];
	SubMenuOptions[Tabindex] += 1;

}

int GetNetInfoAddress(int index)
{
	return (getCryptedCoD(CheatPatches->CoDPatches.NetInfoAddress) + (index * 0x148));
}

PCHAR GetNameFromNetInfo(int index)
{
	return (char*)(GetNetInfoAddress(index) + 0x08);
}

BYTE* GetIPFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0x7C); 
}

BYTE* GetMacAddressFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0x82);
}

QWORD GetXUIDFromNetInfo(int index, BOOL read = FALSE)
{
	if(read)
		return *(QWORD*)GetNetInfoAddress(index);
	return GetNetInfoAddress(index);
}

QWORD GetMachineIDFromNetInfo(int index)
{
	return _byteswap_uint64(*(QWORD*)(GetNetInfoAddress(index) + 0x90)); 
}

void UpdateClientsNames()
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[MENU_CLIENTS], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to update client names\n");
		return;
	}

	for (int iClients = 0; iClients < 18; iClients++)
	{
		char* Name = GetNameFromNetInfo(iClients);
		BYTE* PlayerXUID = (BYTE*)GetXUIDFromNetInfo(iClients);
		if (strcmp(Name, ""))
		{
			if (PlayerXUID[0] == 0x0 && PlayerXUID[1] == 0x09 && Name[0] != 0)
				SubMenuNames[iClients][Tabindex] = Name;
			else
				SubMenuNames[iClients][Tabindex] = MenuOptionNames[OPTION_NAMEDEFAULT];
		}
		else
			SubMenuNames[iClients][Tabindex] = MenuOptionNames[OPTION_NAMEDEFAULT];
	}
}

void GenerateRandomXUIDCharacter()
{
	char Characters[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	for (int i = 0; i < 0xC; i++)
	{
		DWORD dwRandom = rand() % 9;

		RandomXUID[i] = Characters[dwRandom];
	}
}

void SetupMenuText() {

	//Menu Names
	MenuNames.push_back(("XeLive"));
	MenuNames.push_back(("Rage"));
	MenuNames.push_back(("Legit"));
	MenuNames.push_back(("HvH"));
	MenuNames.push_back(("Visuals"));
	MenuNames.push_back(("Clients"));
	MenuNames.push_back(("Settings"));

	//Main
	MenuOptionNames.push_back(("Radar"));
	MenuOptionNames.push_back(("No Recoil"));
	MenuOptionNames.push_back(("No Sway"));
	MenuOptionNames.push_back(("Change Team?"));
	MenuOptionNames.push_back(("Steal GT's"));
	MenuOptionNames.push_back(("Rainbow GT"));
	MenuOptionNames.push_back(("Spoof GT?"));
	MenuOptionNames.push_back(("Show Host"));
	MenuOptionNames.push_back(("Show Ping"));
	MenuOptionNames.push_back(("Fast Reload"));
	MenuOptionNames.push_back(("Everyone Hears Me"));
	MenuOptionNames.push_back(("Never Host"));
	MenuOptionNames.push_back(("Field Of View:"));
	MenuOptionNames.push_back(("Third Person"));
	MenuOptionNames.push_back(("Fake Lag Amount:"));
	MenuOptionNames.push_back(("Jugg Class?"));
	MenuOptionNames.push_back(("End Game?"));
	MenuOptionNames.push_back(("Leave Game?"));

	//Ragebot
	MenuOptionNames.push_back(("Enable Ragebot"));
	MenuOptionNames.push_back(("Ragebot Type:"));
	MenuOptionNames.push_back(("Aim Bone:"));
	MenuOptionNames.push_back(("Prediction Scale:"));
	MenuOptionNames.push_back(("No Spread"));
	MenuOptionNames.push_back(("No Flinch"));
	MenuOptionNames.push_back(("Auto Wall"));
	MenuOptionNames.push_back(("Auto Shoot"));
	MenuOptionNames.push_back(("Auto Aim"));
	MenuOptionNames.push_back(("Rapid Fire"));
	MenuOptionNames.push_back(("Target Shields"));
	MenuOptionNames.push_back(("Preferred Settings"));

	//Legitbot
	MenuOptionNames.push_back(("Enable Legitbot"));
	MenuOptionNames.push_back(("Aim Button:"));
	MenuOptionNames.push_back(("Reaction Delay:"));
	MenuOptionNames.push_back(("FOV Size:"));
	MenuOptionNames.push_back(("Draw FOV Box"));
	MenuOptionNames.push_back(("Smooth Scale:"));
	MenuOptionNames.push_back(("Triggerbot"));

	//Spinbot
	MenuOptionNames.push_back(("Enable Spinbot"));
	MenuOptionNames.push_back(("X Spin Direction:"));
	MenuOptionNames.push_back(("Y Spin Direction:"));
	MenuOptionNames.push_back(("Y Spin Speed:"));
	MenuOptionNames.push_back(("Z Spin Direction:"));
	MenuOptionNames.push_back(("Z Spin Speed:"));
	MenuOptionNames.push_back(("X Anti Aim Spinbot:"));
	MenuOptionNames.push_back(("Y Anti Aim Spinbot:"));

	//Drawing
	MenuOptionNames.push_back(("Draw Enemies"));
	MenuOptionNames.push_back(("Draw Friendlies"));
	MenuOptionNames.push_back(("ESP Box Type:"));
	MenuOptionNames.push_back(("Player Color:"));
	MenuOptionNames.push_back(("Draw Name"));
	MenuOptionNames.push_back(("Draw Weapon"));
	MenuOptionNames.push_back(("Draw Bones"));
	MenuOptionNames.push_back(("Draw Explosives"));
	MenuOptionNames.push_back(("Draw Snaplines:"));
	MenuOptionNames.push_back(("Draw Killstreaks"));
	MenuOptionNames.push_back(("Directional Arrows"));
	MenuOptionNames.push_back(("Crosshair Type:"));
	MenuOptionNames.push_back(("Draw Custom Radar"));
	MenuOptionNames.push_back(("Draw Dropped Items"));
	MenuOptionNames.push_back(("Draw Enemy Visible Text"));
	MenuOptionNames.push_back(("Draw Proximity Warning"));
	MenuOptionNames.push_back(("Draw Through Walls"));
	MenuOptionNames.push_back(("Cartoon Mode"));

	//Clients
	MenuOptionNames.push_back(("Searching..."));

	//Settings
	MenuOptionNames.push_back(("Disable Menu Sounds"));
	MenuOptionNames.push_back(("Disable Menu Instructions"));
	MenuOptionNames.push_back(("Disable XeLive Engine Text"));
	MenuOptionNames.push_back(("Disable Main Menu Music"));
	MenuOptionNames.push_back(("Prestige Disco"));
	MenuOptionNames.push_back(("Prestige:"));
	MenuOptionNames.push_back(("Level 55?"));
	MenuOptionNames.push_back(("1,000,000 COD Points?"));
	MenuOptionNames.push_back(("Invert Team Check"));
	MenuOptionNames.push_back(("Log IP's"));
	MenuOptionNames.push_back(("Freeze Gun?"));
}

void SetupMenu() {
	SetupMenuText();

	/* Main Submenu*/
	AddMenuOption(MENU_MAIN, OPTION_RADAR, 0, bRadar);
	AddMenuOption(MENU_MAIN, OPTION_NORECOIL, 1, bNoRecoil);
	AddMenuOption(MENU_MAIN, OPTION_NOSWAY, 2, bNoSway);
	AddMenuOption(MENU_MAIN, OPTION_STEALGTS, 3, bStealGTs);
	AddMenuOption(MENU_MAIN, OPTION_RAINBOWGT, 4, bRainbowGT);
	AddMenuOption(MENU_MAIN, OPTION_SPOOFGT, 5);
	AddMenuOption(MENU_MAIN, OPTION_SHOWHOST, 6, bShowHost);
	AddMenuOption(MENU_MAIN, OPTION_SHOWPING, 7, bShowPing);
	AddMenuOption(MENU_MAIN, OPTION_FASTRELOAD, 8, bFastReload);
	AddMenuOption(MENU_MAIN, OPTION_EVERYONEHEARSME, 9, bEveryoneHearsMe);
	AddMenuOption(MENU_MAIN, OPTION_NEVERHOST, 10, bNeverHost);
	AddMenuOption(MENU_MAIN, OPTION_FOV, 11, FOVSizes_Start, "Normal", FOVS, FOVS_Size, iFieldOfViewIndex);
	AddMenuOption(MENU_MAIN, OPTION_THIRDPERSON, 12, bThirdPerson);
	AddMenuOption(MENU_MAIN, OPTION_FAKELAGAMOUNT, 13, FakeLagTypes_Start, "Off", FakeLagTypes, FakeLagTypes_Size, iFakeLagAmount);
	AddMenuOption(MENU_MAIN, OPTION_ENDGAME, 14);
	AddMenuOption(MENU_MAIN, OPTION_LEAVEGAME, 15);

	/*Ragebox Submenu*/
	AddMenuOption(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0, bRagebot);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAGEBOTTYPE, 1, RagebotTypes_Start, "Silent", RagebotTypes, RagebotTypes_Size, iRagebotType);
	AddMenuOption(MENU_RAGEBOT, OPTION_AIMBONE, 2, AimBones_Start, "Best", AimBones, AimBones_Size, iRBAimBone);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREDICTION, 3, PredictionScales_Start, "Off", PredictionScales, PredictionScales_Size, iRBPrediction);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOSPREAD, 4, bNoSpread);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOFLINCH, 5, bNoFlinch);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOWALL, 6, bAutoWall);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOSHOOT, 7, bAutoShoot);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOAIM, 8, bAutoAim);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAPIDFIRE, 9, bRapidFire);
	AddMenuOption(MENU_RAGEBOT, OPTION_TARGETSHIELDS, 10, bTargetShields);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 11, bRagebotPreferred);

	/*Legit Submenu*/
	AddMenuOption(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0, bLegitbot);
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMBONE, 1, AimBonesLegit_Start, "j_head", AimBonesLegit, AimBonesLegit_Size, iLBAimBone);
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMKEY, 2, 0, "Off", AimKeys, AimKeys_Size, iLBAimButton);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOSHOOT, 3, bLBAutoShoot);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOAIM, 4, bLBAutoAim);
	AddMenuOption(MENU_LEGITBOT, OPTION_REACTIONDELAY, 5, ReactionDelays_Start, "Off", ReactionDelays, ReactionDelays_Size, iReactionDelay);
	AddMenuOption(MENU_LEGITBOT, OPTION_FOVSIZE, 6, iFOVSize, "80.0", FOVSizes, FOVSizes_Size, FOVSizes_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_DRAWFOVBOX, 7, bDrawFOVBox);
	AddMenuOption(MENU_LEGITBOT, OPTION_SMOOTHSCALE, 8, SmoothScales_Start, "10.0", SmoothScales, SmoothScales_Size, iSmoothScale);
	AddMenuOption(MENU_LEGITBOT, OPTION_TRIGGERBOT, 9, bTriggerbot);
	AddMenuOption(MENU_LEGITBOT, OPTION_RAPIDFIRE, 10, bLBRapidFire);
	AddMenuOption(MENU_LEGITBOT, OPTION_TARGETSHIELDS, 11, bLBTargetShields);
	AddMenuOption(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 12, bLegitbotPreferred);


	AddMenuOption(MENU_SPINBOT, OPTION_ENABLESPINBOT, 0, bSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_XSPINDIRECTION, 1, XSpinDirects_Start, "Off", XSpinDirects, XSpinDirects_Size, iXSpinDirection); 
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINDIRECTION, 2, YSpinDirects_Start, "Off", YSpinDirects, YSpinDirects_Size, iYSpinDirection); 
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINSPEED, 3, YSpinSpeeds_Start, "10.0", YSpinSpeeds, YSpinSpeeds_Size, iYSpinSpeed);
	AddMenuOption(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 4, XAASpinbots_Start, "Off", XAASpinbots, XAASpinbots_Size, iXAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 5, YAASpinbots_Start, "Off", YAASpinbots, YAASpinbots_Size, iYAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_PREFERREDSETTINGS, 6, bSpinbotPreferred);


	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMIES, 0, bDrawEnemies);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWFRIENDLIES, 1, bDrawFriendlies);
	AddMenuOption(MENU_DRAWING, OPTION_ESPBOXTYPE, 2, ESPBoxes_Start, "Off", ESPBoxes, ESPBoxes_Size, iESPBoxType);
	AddMenuOption(MENU_DRAWING, OPTION_PLAYERCOLOR, 3, PlayerColors_Start, "Normal", PlayerColors, PlayerColors_Size, iPlayerColor);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWNAME, 4, bDrawName);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWWEAPON, 5, bDrawWeapon);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWBONES, 6, bDrawBones);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWEXPLOSIVES, 7, bDrawExplosives);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWSNAPLINES, 8, SnaplinesTypes_Start, "Off", SnaplinesTypes, SnaplinesTypes_Size, iSnaplines);
	AddMenuOption(MENU_DRAWING, OPTION_DIRECTIONALARROWS, 9, bDirectionalArrows);
	AddMenuOption(MENU_DRAWING, OPTION_CROSSHAIRTYPE, 10, CrosshairTypess_Start, "Off", CrosshairTypes, CrosshairTypes_Size, iCrosshairType);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR, 11, bDrawCustomRadar);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS, 12, bDrawDroppedItems);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE, 13, bDrawEnemyVis);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING, 14, bDrawProximityWrn);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS, 15, bDrawThroughWalls);
	AddMenuOption(MENU_DRAWING, OPTION_CARTOONMODE, 16, bCartoonMode);


	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 0);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 1);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 2);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 3);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 4);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 5);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 6);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 7);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 8);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 9);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 10);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 11);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 12);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 13);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 14);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 15);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 16);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 17);

	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS, 0, bDisableMenuSounds);
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS, 1, bDisableMenuInstructions);
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEXeLiveTEXT, 2, bDisableXeLiveEngineText);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGEDISCO, 3, bPrestigeDisco);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGE, 4, 0, "N/A", Prestiges, Prestiges_Size, Prestiges_Start);
	AddMenuOption(MENU_SETTINGS, OPTION_LEVEL55, 5);
	AddMenuOption(MENU_SETTINGS, OPTION_INVERTTEAMCHECK, 6);
	AddMenuOption(MENU_SETTINGS, OPTION_LOGIPS, 7, bLogIPs);
	AddMenuOption(MENU_SETTINGS, OPTION_FREEZEGUN, 8);

	HudSmallFont = DB_FindXAssetHeader_BO2(font_BO2, "fonts/hudsmallfont", 1);
	BigDevFont = DB_FindXAssetHeader_BO2(font_BO2, "fonts/bigDevFont", 1);
	SmallFont = DB_FindXAssetHeader_BO2(font_BO2, "fonts/720/smallFont", 1);
	ExtraBigDevFont = DB_FindXAssetHeader_BO2(font_BO2, "fonts/720/extraBigFont", 1);
	ExtraSmallFont = DB_FindXAssetHeader_BO2(font_BO2, "fonts/720/extraSmallFont", 1);
	WhiteMaterial = DB_FindXAssetHeader_BO2(material_BO2, "white", 1); 
	GradientFadeIn = DB_FindXAssetHeader_BO2(material_BO2, "gradient_fadein", 1);
	ScavMaterial = DB_FindXAssetHeader_BO2(material_BO2, "hud_scavenger_pickup", 1);
	ArrowIcon = DB_FindXAssetHeader_BO2(material_BO2, "compassping_player", 1);

	menuReady = TRUE;
}

bool UserInGame(BOOL onlyDisconected = FALSE)
{
	bool allLocalClientsDisconnected = ((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_AllLocalClientsDisconnected))();
	if (onlyDisconected)
		return !allLocalClientsDisconnected;
	return *(bool*)(*(int*)getCryptedCoD(CheatPatches->CoDPatches.DVAR_CL_Ingame) + 0x28) && !allLocalClientsDisconnected;
}

bool buttonFinished = false;
void HandleInput() {
	XINPUT_STATE currentState;
	XInputGetState(0, &currentState);

	#pragma region OPEN MENU
	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && (currentState.Gamepad.bLeftTrigger) && menuWait == FALSE) {
		menuWait = TRUE;
		menuOpen ^= 1;
	}
	else if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && !(currentState.Gamepad.bLeftTrigger) && menuWait == TRUE) {
		menuWait = FALSE;
	}
	#pragma endregion

	#pragma region Scroll Submenu Left
	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && DPads[0] == FALSE && menuOpen) {
		DPads[0] = TRUE;
		if (subMenu == FALSE)
			scroll--;
		else
			subScroll--;
	}

	if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && DPads[0] == TRUE) {
		DPads[0] = FALSE;
	}
	#pragma endregion

	#pragma region Scroll Submenu Right
	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && DPads[1] == FALSE && menuOpen) {
		DPads[1] = TRUE;
		if (subMenu == FALSE) {
			scroll++;
		}
		else { subScroll++; }
	}

	if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && DPads[1] == TRUE) {
		DPads[1] = FALSE;
	}
	#pragma endregion

	#pragma region Enable Option
	if (menuOpen && subMenu && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && AButton == FALSE) {
		AButton = TRUE;
		if (!clientMenu)
			OptionEnabled[subScroll][scroll] ^= 1;
		if (clientMenu)
			ClientOptionEnabled[subScroll][selectedClient] ^= 1;


		if (!clientMenu && OptionValues[subScroll][scroll] != NULL) {
			if (OptionArrayCurrent[subScroll][scroll] == (OptionArraySize[subScroll][scroll]) - 1)
				OptionArrayCurrent[subScroll][scroll] = 0;
			else
				OptionArrayCurrent[subScroll][scroll] = OptionArrayCurrent[subScroll][scroll] + 1;
		}
		if (scroll == 5 && subMenu && !clientMenu) {
			clientMenu = TRUE;
			selectedClient = subScroll;
			subScroll = 1;
		}
	}

	if (menuOpen && !(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && AButton == TRUE) {
		AButton = FALSE;
	}
	#pragma endregion

	#pragma region Disable Option
	if (menuOpen && subMenu && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_X) && BButton == FALSE) {
		BButton = TRUE;

		if (!clientMenu && OptionValues[subScroll][scroll] != NULL) {
			if (OptionArrayCurrent[subScroll][scroll] == 0)
				OptionArrayCurrent[subScroll][scroll] = (OptionArraySize[subScroll][scroll]) - 1;
			else
				OptionArrayCurrent[subScroll][scroll] = (OptionArrayCurrent[subScroll][scroll]) - 1;
		}

		if (scroll == 5 && subMenu && clientMenu) {
			clientMenu = FALSE;
			subScroll = 0;
		}
	}

	if (menuOpen && !(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && BButton == TRUE) {
		BButton = FALSE;
	}
	#pragma endregion

	#pragma region Scroll up and down options
	if (menuOpen && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) {
		subMenu = TRUE;
		subScroll = 0;
	}

	if (menuOpen && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) {
		subMenu = FALSE;
		subScroll = 0;
		clientMenu = FALSE;
	}
	#pragma endregion

	if (scroll == MenuOptions) scroll = 0;
	if (scroll == -1) scroll = (MenuOptions - 1);
	if (subScroll == SubMenuOptions[scroll]) subScroll = 0;
	if (subScroll == -1) subScroll = (SubMenuOptions[scroll] - 1);
	if (clientMenu) {
		if (subScroll < 1) subScroll = 5;
		else if (subScroll > 5) subScroll = 1;
	}
}

void DrawRect(float X, float Y, float W, float H, Vector4& Color) {
	R_AddCmdDrawStretchPic(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawOutlinedRect(float x, float y, float w, float h, float thickness, Vector4& boxColor, Vector4& borderColor, bool sides) {
	R_AddCmdDrawStretchPic(x, y, w, h, 1.f, 1.f, 1.f, 1.f, boxColor, WhiteMaterial); // bg 
	R_AddCmdDrawStretchPic(x, y - thickness, w, thickness, 1, 1, 1, 1, borderColor, WhiteMaterial); // top bar
	R_AddCmdDrawStretchPic(x, y + h, w, thickness, 1, 1, 1, 1, borderColor, WhiteMaterial); // bottom bar

	if (sides) {
		R_AddCmdDrawStretchPic(x - thickness, y - thickness, thickness, h + (thickness * 2), 1, 1, 1, 1, borderColor, WhiteMaterial); // left bar
		R_AddCmdDrawStretchPic(x + w, y - thickness, thickness, h + (thickness * 2), 1, 1, 1, 1, borderColor, WhiteMaterial); // right bar
	}
}

void DrawCenteredText(char* text, float x, float y, float width, int font, float fontScale, Vector4& color) {
	int textLength = ((R_TextWidth(0, text, 0x7FFFFFFF, font) / 2) * fontScale);

	R_AddCmdDrawText(text, 0x7FFFFFFF, font, (x + (width / 2) - textLength), y, fontScale, fontScale, 0, color, 0);
}

void DrawLeftSidedText(char* text, float x, float y, float width, int font, float fontScale, Vector4& color) {
	int textLength = ((R_TextWidth(0, text, 0x7FFFFFFF, font) / 2) * fontScale);

	R_AddCmdDrawText(text, 0x7FFFFFFF, font, ((x + width) - textLength - 20), y, fontScale, fontScale, 0, color, 0);
}

void DrawRightSidedText(char* text, float x, float y, int font, float fontSize, Vector4& Color) {
	R_AddCmdDrawText(text, 0x7FFFFFFF, font, (x - (R_TextWidth(0, text, 0x7FFFFFFF, font) / 1) * fontSize), y, fontSize, fontSize, 0, Color, 0);
}

void DrawGradient(float X, float Y, float W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText(char* text, float x, float y, int font, float fontScale, Vector4& color) {
	R_AddCmdDrawText(text, 0x7FFFFFFF, font, x, y, fontScale, fontScale, 0, color, 0);
}

void DrawText(CONST PCHAR Text, float X, float Y, DWORD index, float addy, DWORD Font, Vector4& Color, float fontXSize = 1.0, float fontYSize = 1.0) {
	CL_DrawTextPhysical(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, 0, Color, 0);
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_BO2->refdef.ViewOrg, Position);

	Transform.x = DotProduct(Position, cg_BO2->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_BO2->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_BO2->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_BO2->refdef.Width * 0.5f; Center.y = cg_BO2->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_BO2->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_BO2->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine(float X1, float Y1, float X2, float Y2, float Width, Vector4& Color) {
	float X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (float)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), X, Y, L2, Width, Angle, Color, WhiteMaterial);
}

void MakeLines(Vector3 Origin, float X1, float Y1, float Z1, float X2, float Y2, float Z2, Vector4& Color, int Size = 1) {
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(float X, float Y, BOOL Checked = FALSE) {
	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom
}

void DrawShader(float x, float y, float width, float height, DWORD material, Vector4& color) {
	R_AddCmdDrawStretchPic(x, y, width, height, 1.0f, 1.0f, 1.0f, 1.0f, color, material);
}

void DrawShaderWithOutline(float x, float y, float w, float h, Vector4& backgroundColor, Vector4& outlineColor, float lineWidth) {
	DrawShader(x, y, lineWidth, h, WhiteMaterial, outlineColor);
	DrawShader(x + lineWidth, y, w - lineWidth, lineWidth, WhiteMaterial, outlineColor);
	DrawShader(x + lineWidth, y + h - lineWidth, w - (lineWidth * 2), lineWidth, WhiteMaterial, outlineColor);
	DrawShader(x + w - lineWidth, y + lineWidth, lineWidth, h - lineWidth, WhiteMaterial, outlineColor);

	DrawShader(x + lineWidth, y + lineWidth, w - (lineWidth * 2), h - (lineWidth * 2), WhiteMaterial, backgroundColor);
}

float CenterX, CenterY;

void RenderMenu() {

	Vector4 ActiveTextColor = GetColorCode(colorBlue);
	Vector4 InActiveTextColor = Vector4(1, 1, 1, 0.9);
	Vector4 MenuLinesColor = GetColorCode(Vector4(112, 224, 0, 255));

	CenterX = ((1280 / 2) - (menuWidth / 2));
	CenterY = ((720 / 2) - (((menuHeight) + 22) / 2));

	int MenuFont = SmallFont;

	/* Options Background*/
	DrawOutlinedRect(CenterX, CenterY, menuWidth, menuHeight, 2, GetColorCode(Vector4(48, 48, 48, 255)), MenuLinesColor, true); //Background

	//Draw Menu Names
	for (int i = 0; i < 7; i++) {
		/* Submenu Text & Background */
		DrawOutlinedRect(CenterX, CenterY - 54.0f, menuWidth, 25.0f, 2, GetColorCode(Vector4(51, 53, 51, 255)), MenuLinesColor, true); //Title header
		DrawCenteredText("XeLive Engine", CenterX, CenterY - 35.0f, menuWidth, MenuFont, 0.75f, InActiveTextColor); // Title Header Text

		DrawOutlinedRect(CenterX + (i * 64.28), CenterY - 27.0f, 64.28, 25.0f, 2, GetColorCode(Vector4(48, 48, 48, 225)), MenuLinesColor, true); //Submenu Header
		DrawCenteredText(MenuNames[i], CenterX + (i * 64.28), CenterY - 8.0f, 64.28f, MenuFont, 0.75f, (scroll == i) ? ActiveTextColor : InActiveTextColor); //Submenu Text
	}

	for (int i = 0; i < SubMenuOptions[scroll]; i++) {
		DrawOutlinedRect(CenterX, CenterY + subScroll * 22, menuWidth, 20, 2, GetColorCode(Vector4(0, 0, 0, 0)), ActiveTextColor, true);

		if (subMenu == FALSE) {
			DrawText(SubMenuNames[i][scroll], CenterX + 5.0f, CenterY + 17.0f, i, 22, MenuFont, (subScroll == i) ? ActiveTextColor : InActiveTextColor, 0.75f, 0.75f);
		}
		else if (subMenu == TRUE && clientMenu == FALSE) {
			//DrawOutlinedRect(CenterX, CenterY + subScroll * 22, menuWidth, 20, 2, GetColorCode(Vector4(0, 0, 0, 0)), ActiveTextColor, false);
			DrawText(SubMenuNames[i][scroll], CenterX + 5.0f, CenterY + 17.0f, i, 22, MenuFont, (subScroll == i) ? ActiveTextColor : InActiveTextColor, 0.75f, 0.75f);

		}
		else if (subMenu == TRUE && clientMenu == TRUE && i == 0) {
			int Tabindex = -1;
			for (int i = 0; i < MenuOptions; i++) {
				if (!strcmp(MenuNames[MENU_CLIENTS], MenuNames[i])) {
					Tabindex = i;
					break;
				}
			}
		}

		if (clientMenu)
			continue;

		if (strstr(SubMenuNames[i][scroll], "?"))
			continue;

		if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5) {
			DrawShaderWithOutline(CenterX + menuWidth - 20.0f, CenterY + (i * 22.0f) + 5.0f, 10.0f, 10.0f, GetColorCode(Vector4(0, 0, 0, 0)), OptionEnabled[i][scroll] ? ActiveTextColor : Vector4(1, 0, 0, 1), 1);
		}
		else if (strstr(SubMenuNames[i][scroll], ":")) {
			DrawRightSidedText(GetMenuOptionString((eMenuNames)scroll, SubMenuNames[i][scroll]), CenterX + menuWidth - 5.0f, (CenterY + 17.0f) + (i * 22.0f), MenuFont, 0.75f, InActiveTextColor);
		}
	}
}

float ToDegrees(float radian)
{
	return radian * (180 / 3.14159265359);
}

float ToRadians(float degrees)
{
	return degrees * (3.14159265359 / 180);
}

float Vec3DistanceSq(const float *p1, const float *p2)
{
	return ((p2[0] - p1[0]) * (p2[0] - p1[0]) +
		((p2[2] - p1[2]) * (p2[2] - p1[2]) +
		((p2[1] - p1[1]) * (p2[1] - p1[1]))));
}

void RotateXY(float cosAngle, float sinAngle, float pivotX, float pivotY, float x, float y, float *outX, float *outY)
{
	*outX = (float)((float)((float)(x - pivotX) * cosAngle) + pivotX) - (float)((float)(y - pivotY) * sinAngle);
	*outY = (float)((float)((float)(y - pivotY) * cosAngle) + pivotY) + (float)((float)(x - pivotX) * sinAngle);
}

PCHAR EngineText = NULL;
void DrawAd()
{
	FLOAT X, Y;
	if(EngineText == NULL)
		EngineText = ("XELIVE ENGINE");
	X = (FLOAT)cgs_BO2->Width * 0.78125;
	Y = (FLOAT)cgs_BO2->Height * 0.2;
	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth(1, EngineText, 0x7FFFFFFF, SmallFont) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;

	CL_DrawTextPhysical(EngineText, 0x7FFFFFFF, SmallFont, X, Y, 1, 1, rotation, Vector4(1, 0, 0, 0.75), 0);
}

BOOL ShouldUpdateTagCache()
{
	if (UserInGame() && !LobbyInit)
	{
		LobbyInit = TRUE;
		return TRUE;
	}
	else if (!UserInGame() && LobbyInit)
	{
		LobbyInit = FALSE;
		return FALSE;
	}

	return FALSE;
}

short GetTagIndex(PCHAR Tag, BOOL ShouldCache)
{
	// Cache the tag index's
	if (ShouldCache)
	{
		// Cache our bone array index's
		for (int i = 0; i < ARRAYSIZE(BoneCache); i++)
		{
			BoneIndexs[i] = SL_GetString_BO2(BoneCache[i], 0);
		}

		DbgPrint("Precached Player Tags!");
	}

	short index = 0;
	for (index = 0; index < ARRAYSIZE(BoneCache); index++)
	{
		if (!strcmp(BoneCache[index], Tag))
			return BoneIndexs[index];

	}
	return BoneIndexs[index];
}

bool GetTagPosistion(pcentity_s_BO2 centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	//char Temp[0x100];
	short TagIndex;
	int Dobj;

	//for (int i = 0; i <= strlen(SelectedTag); i++)
	//	Temp[i] = tolower(SelectedTag[i]);

	TagIndex = GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString_BO2(Temp, 0);
	Dobj = Com_GetClientDObj_BO2(ClientIndex, 0);

	if (Dobj == 0)
		return false;

	return ((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_DObjGetWorldTagPos))(centity_s, Dobj, TagIndex, OutPos) != 0;
}

short TagToShort(PCHAR Tag)
{
	char Temp[0x100];

	for (int i = 0; i <= strlen(Tag); i++)
		Temp[i] = tolower(Tag[i]);

	return SL_GetString_BO2(Temp, 0);
}

void DropTarget(ClientInformation_BO2 PlayerInfo) {
	PlayerInfo.Alive = FALSE;
	PlayerInfo.Distance = 900000;
	PlayerInfo.Name = NULL;
	PlayerInfo.OnTeam = FALSE;
	PlayerInfo.SelectedBone = "j_head";
	PlayerInfo.TagPosistion = NULL;
	PlayerInfo.Team = 69;
	PlayerInfo.ThreatLevel = 0;
	PlayerInfo.Visible = FALSE;
	PlayerInfo.VisibleESP = FALSE;
	PlayerInfo.Wallbangable = FALSE;
	PlayerInfo.ClientNumber = 420;
	PlayerInfo.CurrentTarget = FALSE;
	PlayerInfo.GVisible = FALSE;
	PlayerInfo.GVisibleESP = FALSE;
	PlayerInfo.PrimaryShield = FALSE;
	PlayerInfo.AutowallPower = -0.0f;
}

void StealNames()
{
	char Output[0x200];

	int RandomClient = rand() % 17;
	if (UserInGame() && (cg_BO2 != NULL))
	{
		if (strcmp(cg_BO2->clientInfo[RandomClient].Name, "") && (RandomClient != cg_BO2->ClientNumber) && (cg_BO2->clientInfo[RandomClient].Name != NULL))
		{
			char xuid[20];
			_ui64toa(GetXUIDFromNetInfo(cg_BO2->ClientNumber, TRUE), xuid, 0x10);
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO2->clientInfo[RandomClient].Name, xuid);
			//Cbuf_AddText_BO2(0, Output);
			CL_AddReliableCommand(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			//sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(RandomClient), GetXUIDFromNetInfo(cg_BO2->ClientNumber));
			//Cbuf_AddText_BO2(0, Output);
			//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName() {
	char Output[0x200], Color[10];
	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (UserInGame()) {
		char xuid[20];
		_ui64toa(GetXUIDFromNetInfo(cg_BO2->ClientNumber, TRUE), xuid, 0x10);
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(cg_BO2->ClientNumber), Color, xuid);
		CL_AddReliableCommand(0, Output);
	}
}

void PrestigeDisco() {
	int RandomPrestige = rand() % 16;
	if (!UserInGame(TRUE)) {
		*(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset) = RandomPrestige;
	}
}

void SpoofGamertag() {
	char Output[0x200];
	wchar_t wcGT[0x1F];
	XOVERLAPPED XOver;
	ZeroMemory(&XOver, sizeof(XOVERLAPPED));
	XShowKeyboardUI(0, VKBD_HIGHLIGHT_TEXT, wcOldGT, L"Spoof Gamertag", L"Type In New Gamertag!", wcGT, 0x1E, &XOver);

	while (!XHasOverlappedIoCompleted(&XOver)) Sleep(25);

	Sleep(2000);

	if (wcGT[0] != 0 && wcGT[1] != 0 && UserInGame()) {
		wcscpy_s(wcOldGT, wcGT);
		wcstombs(OriginalGT, wcGT, 0x1F);

		char xuid[20];
		_ui64toa(GetXUIDFromNetInfo(cg_BO2->ClientNumber, TRUE), xuid, 0x10);
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, xuid);
		CL_AddReliableCommand(0, Output);
	}

	ExitThread(ERROR_SUCCESS);
}

int FindFOVSize()
{
	int FOVSize = 80;
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "80.0")
		FOVSize = 80;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "90.0")
		FOVSize = 90;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "100.0")
		FOVSize = 100;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "120.0")
		FOVSize = 120;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "140.0")
		FOVSize = 140;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "180.0")
		FOVSize = 180;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "220.0")
		FOVSize = 220;

	return FOVSize;
}

int FindFieldOfViewSize()
{
	int FieldOfViewSize = 65;
	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "Normal")
		FieldOfViewSize = 65;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "70.0")
		FieldOfViewSize = 70;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "75.0")
		FieldOfViewSize = 75;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "80.0")
		FieldOfViewSize = 80;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "90.0")
		FieldOfViewSize = 90;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "100.0")
		FieldOfViewSize = 100;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "110.0")
		FieldOfViewSize = 110;

	return FieldOfViewSize;
}

int FindYSpinSpeed()
{
	int YSpinSpeed = 10;
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "10.0")
		YSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "20.0")
		YSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "30.0")
		YSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "40.0")
		YSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "50.0")
		YSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "60.0")
		YSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "70.0")
		YSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "Random")
		YSpinSpeed = rand() % 71;

	return YSpinSpeed;
}

int FindZSpinSpeed()
{
	int ZSpinSpeed = 10;
	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "10.0")
		ZSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "20.0")
		ZSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "30.0")
		ZSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "40.0")
		ZSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "50.0")
		ZSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "60.0")
		ZSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "70.0")
		ZSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "Random")
		ZSpinSpeed = rand() % 71;

	return ZSpinSpeed;
}

int FindFakeLagAmount()
{
	int FakeLagAmount = 5;
	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "5.0")
		FakeLagAmount = 5;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "10.0")
		FakeLagAmount = 10;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "15.0")
		FakeLagAmount = 15;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "20.0")
		FakeLagAmount = 20;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "25.0")
		FakeLagAmount = 25;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "30.0")
		FakeLagAmount = 30;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "Random")
		FakeLagAmount = rand() % 25;

	return FakeLagAmount;
}

int FindReactionDelayAmount()
{
	int ReactionDelayAmount = 20;
	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "2.0")
		ReactionDelayAmount = 20;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "3.0")
		ReactionDelayAmount = 30;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "4.0")
		ReactionDelayAmount = 40;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "5.0")
		ReactionDelayAmount = 50;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "6.0")
		ReactionDelayAmount = 60;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "7.0")
		ReactionDelayAmount = 70;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "Random")
		ReactionDelayAmount = rand() % 80;

	return ReactionDelayAmount;
}

BYTE FindPrestigeValue()
{
	BYTE Prestige = *(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset);

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "N/A")
		Prestige = 0;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "0th")
		Prestige = 0;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "1st")
		Prestige = 1;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "2nd")
		Prestige = 2;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "3rd")
		Prestige = 3;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "4th")
		Prestige = 4;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "5th")
		Prestige = 5;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "6th")
		Prestige = 6;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "7th")
		Prestige = 7;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "8th")
		Prestige = 8;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "9th")
		Prestige = 9;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "10th")
		Prestige = 10;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "11th")
		Prestige = 11;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "12th")
		Prestige = 12;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "13th")
		Prestige = 13;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "14th")
		Prestige = 14;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "15th")
		Prestige = 15;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "16th")
		Prestige = 16;

	return Prestige;
}

int FindPredicationScale()
{
	int PredicationScale = 0;
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "2.0")
		PredicationScale = 2;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "5.0")
		PredicationScale = 5;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "10.0")
		PredicationScale = 10;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "15.0")
		PredicationScale = 15;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "20.0")
		PredicationScale = 20;

	return PredicationScale;
}

void LogGamertagAndIP()
{
	//DbgPrint("Starting IP Log");
	if (!IPBuffer)
	{
		IPBuffer = new MemoryBuffer(0x8000);
		if (!IPBuffer)
		{
			DbgPrint("Failed to allocate IP buffer");
			return;
		}
	}

	memset(IPBuffer->GetData(), 0, 0x8000);
	IPBuffer->Rewind();

	BOOL exists = FileExists("XELIVE:\\XeLive Cheat\\BO2\\IPs.log");

	if (exists && !CReadFile("XELIVE:\\XeLive Cheat\\BO2\\IPs.log", *IPBuffer))
	{
		DbgPrint("Failed to read IP Log file");
		return;
	}

	for (int i = 0; i < 18; i++)
	{
		char buffer2[0x400];
		char* Name = GetNameFromNetInfo(i);
		BYTE* IPAddress = GetIPFromNetInfo(i);

		if (Name[0] == NULL || IPAddress[0] == 0)
			continue;

		if (exists && strstr((CONST PCHAR)IPBuffer->GetData(), Name))
			continue;

		sprintf(buffer2, "Gamertag: %s IP Address: %d.%d.%d.%d\r\n", Name, IPAddress[0], IPAddress[1], IPAddress[2], IPAddress[3]);
		IPBuffer->Add(buffer2, strlen(buffer2));
	}

	if (!exists)
	{
		BOOL ye = WriteFileAlt("XELIVE:\\XeLive Cheat\\BO2\\IPs.log", IPBuffer->GetData(), IPBuffer->GetDataLength()); // XELIVE:\\XeLive Cheat\BO2\\IPs.log
		if (ye)
			DbgPrint("Writing New IP Log File");
		else
			DbgPrint("Failed To Write IP Log File");
		return;
	}

	WriteFileAlt("XELIVE:\\XeLive Cheat\\BO2\\IPs.log", IPBuffer->GetData(), IPBuffer->GetDataLength());
	//DbgPrint("IP Log Update Complete");
}

void HandleToggleOptions(pcgs_t_BO2 cgs_BO2)
{
	// Make sure the menu is ready
	if (!menuReady) return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RADAR))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset) = 0x419A0060;
	}

	// Handle No Recoil
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x48461341;
	}

	// Handle No Sway
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY)) {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x60000000;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x60000000;
	}
	else {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x4BFFE975;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x4BFFFA85;
	}

	// Handle Show Ping
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING) && UserInGame()) {
		char* Ping = va_BO2("Ping: %i", cg_BO2->snap->ping);
		DrawText(Ping, 50.0f, 50.0f, 0, 0, SmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Hear All Players
	if (GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame()) {
		if (!HearEveryoneValue) {
			HearEveryoneValue = TRUE;
			Cbuf_AddText_BO2(0, "cg_everyoneHearsEveryone 1");
		}
	}
	else if(!GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame()) {
		if (HearEveryoneValue) {
			HearEveryoneValue = FALSE;
			Cbuf_AddText_BO2(0, "cg_everyoneHearsEveryone 0");
		}
	}

	// Handle Never Host
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NEVERHOST)) {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NetSpeedOffset) = 1000;
	}
	else {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NetSpeedOffset) = 10000000;
	}

	// Handle Show Host
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST)) {
		if (UserInGame()) {
			char* HostName = (char*)*(int*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.HostNameStruct)) + 0x18);
			if (strcmp(HostName, "")) {
				char HostOut[0x100];
				sprintf(HostOut, "Host: %s", HostName);
				DrawText(HostOut, 50.0f, 25.0f, 0, 0, SmallFont, Vector4(1, 1, 1, 1));
			}
		}
	}

	// Handle Custom Field Of View
	if (UserInGame()) { *(float*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.FOVStruct)) + 0x18) = FindFieldOfViewSize(); }

	// Handle End Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME) && UserInGame())
	{
		char* EndGameString;
		EndGameString = va_BO2(("mr %i %i endround"), *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), -1);

		Cbuf_AddText_BO2(0, EndGameString);
		SetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME, 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME)) {
		Cbuf_AddText_BO2(0, "disconnect");
		SetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME, 0);
	}

	// Handle Ragebot Preferred Settings
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS)) {
		// Enable these
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY, 1);

		// Disable These
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX, 0);
	}

	// Handle Legitbot Preferred Settings
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS))
	{
		// Enable these
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 1);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE, 1);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE, 3);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE, 4);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_AIMKEY, 1);

		// Disable These
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH, 0);
	}

	// Handle Custom Spinbot Settings
	if (GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS)) {
		// Enable These
		SetMenuOptionValue(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 1);
		SetMenuOptionValue(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 1);
	}

	// Handle Ragebot Enabled
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
	{
		// Disable These
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0);

		// Enable these
		SetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY, 1);
	}

	// Handle Legitbot Enabled
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		// Disable These
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT, 0);
	}

	// Handle Name ESP
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
	{
		// Disable over head names
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.DisableOverheadNames) = 0x60000000;
	}
	else
	{
		// Enable over head names
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.DisableOverheadNames) = 0x4BFFEC35;
	}

	// Handle Wallhack
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS)) {
		*(int*)CheatPatches->CoDPatches.DrawThroughWalls = 0x34C0FFFF;
	}
	else {
		*(int*)CheatPatches->CoDPatches.DrawThroughWalls = 0x7FA6EB78;
	}

	// Handle Steal Gamertags
	if (GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS)) {
		if (NameStealDelay > 40)
		{
			NameStealDelay = 0;
			StealNames();
		}
		else
		{
			NameStealDelay++;
		}
	}

	// Handle Rainbow Gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RAINBOWGT))
	{
		//if (cgs_BO2 == NULL)
		//	return;

		if (RainbowGTDelay > 40)
		{
			RainbowGTDelay = 0;
			RainbowName();
		}
		else
		{
			RainbowGTDelay++;
		}
	}

	// Crosshair
	if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) != "Off" && UserInGame())
	{
		if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "+")
		{
			// + styled crosshair
			DrawLine((cg_BO2->refdef.Width / 2) - 10, cg_BO2->refdef.Height / 2, (cg_BO2->refdef.Width / 2) + 10, cg_BO2->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine(cg_BO2->refdef.Width / 2, (cg_BO2->refdef.Height / 2) - 10, cg_BO2->refdef.Width / 2, (cg_BO2->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "x")
		{
			// X styled crosshair
			DrawLine((cg_BO2->refdef.Width / 2) - 8, (cg_BO2->refdef.Height / 2) - 8, (cg_BO2->refdef.Width / 2) + 8, (cg_BO2->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine((cg_BO2->refdef.Width / 2) - 8, (cg_BO2->refdef.Height / 2) + 8, (cg_BO2->refdef.Width / 2) + 8, (cg_BO2->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "o")
		{
			DrawLine((cg_BO2->refdef.Width / 2) - 10, cg_BO2->refdef.Height / 2, (cg_BO2->refdef.Width / 2) + 10, cg_BO2->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine(cg_BO2->refdef.Width / 2, (cg_BO2->refdef.Height / 2) - 10, cg_BO2->refdef.Width / 2, (cg_BO2->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine((cg_BO2->refdef.Width / 2) - 8, (cg_BO2->refdef.Height / 2) - 8, (cg_BO2->refdef.Width / 2) + 8, (cg_BO2->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine((cg_BO2->refdef.Width / 2) - 8, (cg_BO2->refdef.Height / 2) + 8, (cg_BO2->refdef.Width / 2) + 8, (cg_BO2->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Prestige Disco
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_PRESTIGEDISCO))
	{
		if (PrestigeDiscoDelay > 40)
		{
			PrestigeDiscoDelay = 0;
			PrestigeDisco();
		}
		else
		{
			PrestigeDiscoDelay++;
		}
	}

	// Set level 55
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL55))
	{
		*(int*)0x843491BC = 0x4C0F1300; // Rank XP
		*(BYTE*)0x843491B6 = 0x37; // Rank
		SetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL55, 0);
	}

	// Spoof gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT))
	{
		SetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT, 0);

		ExCreateThread(0, 0, 0, 0, (LPTHREAD_START_ROUTINE)SpoofGamertag, 0, 0);
	}

	// Handle Third Person If we aren't dead
	if (UserInGame())
	{
		// Do this in a seperate check so it doesn't try to read a invalid Entities/CG pointer and crash
		if ((*(int*)((DWORD)&Entities_BO2[*(int*)cg_BO2] + 0x36C) & 0x40000000) != 0)
		{
			if (GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON))
			{
				*(BYTE*)((DWORD)cg_BO2 + 0x4809C) = 1;
			}
			else
			{
				*(BYTE*)((DWORD)cg_BO2 + 0x4809C) = 0;
			}
		}
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX) && UserInGame() && !menuOpen && GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		int FOVSize = FindFOVSize();
		DrawRect((cg_BO2->refdef.Width / 2) - (FOVSize / 2), (cg_BO2->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH)) {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset1) = 0x60000000;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset2) = 0x60000000;
	}
	else {
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset1) = 0x4BFFF95D;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset2) = 0x4BFFAC41;
	}

	// Log IP's
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LOGIPS))
	{
		if (UpdateIPLogDelay > 1000)
		{
			UpdateIPLogDelay = 0;
			LogGamertagAndIP();
		}
		else
		{
			UpdateIPLogDelay++;
		}
	}

	// Handle Set GT To Players IP
	if (ClientOptionEnabled[4][selectedClient] && UserInGame())
	{
		char Output[0x200];
		BYTE* IP = GetIPFromNetInfo(selectedClient);
		char* ClientName = GetNameFromNetInfo(selectedClient);

		BYTE* PlayerXUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);
		if (strcmp(ClientName, ""))
		{
			if (PlayerXUID[0] == 0x0 && PlayerXUID[1] == 0x09 && ClientName[0] != 0)
			{
				char* IPName = va_BO2("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", IPName, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
				CL_AddReliableCommand(0, Output);
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}

	// Handle Spinbot Preferred Settings
	if (GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS)) {
		// Disable these
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		// Enable these
		SetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT, 1);
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 1);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 1);
	}

	// Handle Set Prestige
	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) != "N/A") {
		BYTE Prestige = FindPrestigeValue();
		*(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset) = Prestige;
	}

	// Handle Player Body Color
	if (GetMenuOptionString(MENU_DRAWING, OPTION_PLAYERCOLOR) != "Normal") {
		memcpy((VOID*)getCryptedCoD(CheatPatches->CoDPatches.PlayerColorOffset), PlayerColorTable[OptionArrayCurrent[3][4]], sizeof(DWORD) * 3);
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_PLAYERCOLOR) == "Normal") {
		memcpy((VOID*)getCryptedCoD(CheatPatches->CoDPatches.PlayerColorOffset), PlayerColorTable[0], sizeof(DWORD) * 3);
	}

	// Handle Cartoon Mode
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode1) = 0x00020202;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode1) = 0x00030403;
	}

	// Disable Auto Aim If No Spread Is Enabled
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD) && GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM))
	{
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
	}

	// Freeze All Consoles
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_FREEZEGUN))
	{
		if (UserInGame())
		{
			char Output[0x200];
			PCHAR Hax = "^H";

			char xuid[20];
			_ui64toa(GetXUIDFromNetInfo(cg_BO2->ClientNumber, TRUE), xuid, 0x10);
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO2->clientInfo[cg_BO2->ClientNumber].Name, Hax, xuid);
			CL_AddReliableCommand(0, Output);
		}

		SetMenuOptionValue(MENU_SETTINGS, OPTION_FREEZEGUN, 0);
	}
}

bool WorldToScreen(Vector3 World, Vector3 &Screen) {
	float viewaxis[3][3];
	float vieworg[3];
	float width;
	float height;
	float tanHalfFovX;
	float tanHalfFovY;

	//Get the enemy position
	float Position[3];
	float Transform[3];

	memcpy(viewaxis, cg_BO2->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_BO2->refdef.ViewOrg, sizeof(vieworg));
	width = (float)cg_BO2->refdef.Width;
	height = (float)cg_BO2->refdef.Height;
	tanHalfFovX = cg_BO2->refdef.Fov.x;
	tanHalfFovY = cg_BO2->refdef.Fov.y;

	VectorSubtract(World, vieworg, Position);

	//Get the Dot Products from the View Angles of the player
	Transform[0] = DotProduct(Position, viewaxis[1]);
	Transform[1] = DotProduct(Position, viewaxis[2]);
	Transform[2] = DotProduct(Position, viewaxis[0]);

	//Make sure the enemy is in front of the player. If not, return.
	if (Transform[2] < 0.1f)
		return false;

	//Calculate the center of the screen
	float Center[2] = { width * 0.5f, height * 0.5f };

	//Calculates the screen coordinates
	Screen.x = Center[0] * (1 - (Transform[0] / tanHalfFovX / Transform[2]));
	Screen.y = Center[1] * (1 - (Transform[1] / tanHalfFovY / Transform[2]));

	return true;
}

float GetDistance(Vector3 c1, Vector3 c2)
{
	float dx = c2.x - c1.x;
	float dy = c2.y - c1.y;
	float dz = c2.z - c1.z;

	return sqrt((float)((dx * dx) + (dy * dy) + (dz * dz)));
}

float InverseNorm(float input)
{
	if (input < 0)
		return (360.0f + input);
	return input;
}

float GetBoxWidth(int value)
{
	if (value & 0x8)
		return 35;
	else return 17;
}

float GetBoxHeight(int value)
{
	if (value & 0x8)
		return 25;
	else if (value & 0x4)
		return 55;
	else return 70;
}

void getWeaponAspectRatio(DWORD killIconType, float *aspectWidth, float *aspectHeight)
{
	float width = 1.4f, height = 1.4f;
	if (killIconType == 0) //1:1
	{
		width = 1.4f;
		height = 1.4f;
	}
	else if (killIconType == 1) //2:1
	{
		width = 2.8f;
		height = 1.4f;
	}
	else if (killIconType == 2) //3:1
	{
		width = 2.8f;
		height = 0.7f;
	}
	width *= 20.0f;
	height *= 20.0f;
	*aspectWidth = width;
	*aspectHeight = height;
}

int GetWeaponIcon(int Weapon, float *aspectX, float *aspectY)
{
	int weaponDef = BG_GetWeaponDef_BO2(Weapon);

	if (weaponDef) {
		DWORD weapDef2 = ((DWORD(*)(...))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef2))(Weapon);
		if (weapDef2) {
			DWORD killIconType = *(DWORD*)(*(DWORD*)(weapDef2 + 0x8) + 0x664);

			getWeaponAspectRatio(killIconType, aspectX, aspectY);
			return *(int*)(weaponDef + 0x3AC);
		}
	}
	return 0;
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_BO2->refdef.Width / 2.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Bottom")
		xy2[1] = (float)cg_BO2->refdef.Height;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Center")
		xy2[1] = (float)cg_BO2->refdef.Height / 2.0f;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Top")
		xy2[1] = (float)cg_BO2->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Top")
		LinePos = Entities_BO2[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_BO2[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	if (WorldToScreen(LinePos, xy1) == TRUE)
	{
		DrawLine((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_BO2[Index].Origin, Entities_BO2[cg_BO2->ClientNumber].Origin, Output);

	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawGrenadePointer))(0, cg_BO2->refdef.Width / 2.0f, cg_BO2->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPBO2) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_BO2[Index], Index, bonesESPBO2[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_BO2[Index], Index, bonesESPBO2[b + 1], (float*)&TagPos[1]))
			return;

		if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
			DrawLine(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
	}
}

void CalcBoundingBox(int index, Vector4 color)
{
	float corner2X, corner2Y,
		corner3X, corner3Y,
		corner4X, corner4Y,
		corner5X, corner5Y,
		corner6X, corner6Y,
		corner7X, corner7Y,
		minsX, minsY,
		maxesX, maxesY;

	Vector3 Mins(999999.0f, 999999.0f, 999999.0f), Maxes(-999999.0f, -999999.0f, -999999.0f);

	Vector3 BonePos[14];
	for (int t = 0; t < ARRAYSIZE(ThreeDBoxesBones) - 1; t++)
	{
		Vector3* Bones;
		GetTagPosistion(&Entities_BO2[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
		Bones = &BonePos[t];
		if (Bones->x < Mins.x)
			Mins.x = Bones->x;
		if (Bones->y < Mins.y)
			Mins.y = Bones->y;
		if (Bones->z < Mins.z)
			Mins.z = Bones->z;
		if (Bones->x > Maxes.x)
			Maxes.x = Bones->x;
		if (Bones->y > Maxes.y)
			Maxes.y = Bones->y;
		if (Bones->z > Maxes.z)
			Maxes.z = Bones->z;
	}

	PlayerInfo[index].bBox.crnr2.setAngles(Maxes.x, Mins.y, Mins.z);
	PlayerInfo[index].bBox.crnr3.setAngles(Maxes.x, Mins.y, Maxes.z);
	PlayerInfo[index].bBox.crnr4.setAngles(Mins.x, Mins.y, Maxes.z);
	PlayerInfo[index].bBox.crnr5.setAngles(Mins.x, Maxes.y, Maxes.z);
	PlayerInfo[index].bBox.crnr6.setAngles(Mins.x, Maxes.y, Mins.z);
	PlayerInfo[index].bBox.crnr7.setAngles(Maxes.x, Maxes.y, Mins.z);
	PlayerInfo[index].bBox.mins.setAngles(Mins);
	PlayerInfo[index].bBox.maxes.setAngles(Maxes);

	int checksPassed = 0;
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr2, &corner2X, &corner2Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr3, &corner3X, &corner3Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr4, &corner4X, &corner4Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr5, &corner5X, &corner5Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr6, &corner6X, &corner6Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr7, &corner7X, &corner7Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.mins, &minsX, &minsY);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.maxes, &maxesX, &maxesY);

	if (checksPassed == 0)
	{
		//mins to 2, 4, and 6
		DrawLine(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Corners")
	{
		if (!GetTagPosistion(&Entities_BO2[Index], Index, "j_head", (float*)&HeadPos))
			return;

		if (WorldToScreen(Entities_BO2[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (*(int*)((DWORD)&Entities_BO2[Index] + 0x1D4) & 0x08)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawRect((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawRect((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawRect((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawRect((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawRect((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawRect((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawRect((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawRect((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D" || GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_BO2[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (*(int*)((DWORD)&Entities_BO2[Index] + 0x1D4) & 0x08)
			BoxHeight = 17;
		else if (*(int*)((DWORD)&Entities_BO2[Index] + 0x1D4) & 0x04)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "2D Filled")
			{
				DrawLine((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Pyramid")
	{
		Vector3 Center = Entities_BO2[Index].Origin;

		float W = GetBoxWidth(*(int*)((DWORD)&Entities_BO2[Index] + 0x1D4)), H = GetBoxHeight(*(int*)((DWORD)&Entities_BO2[Index] + 0x1D4));

		W *= 1.2;
		H *= 1.0;

		//bottom
		MakeLines(Center, -W, -W, 0, W, -W, 0, color, 1);
		MakeLines(Center, -W, -W, 0, -W, W, 0, color, 1);
		MakeLines(Center, W, W, 0, W, -W, 0, color, 1);
		MakeLines(Center, W, W, 0, -W, W, 0, color, 1);
		//middle
		MakeLines(Center, -W, -W, 0, 0, 0, H, color, 1);
		MakeLines(Center, -W, W, 0, 0, 0, H, color, 1);
		MakeLines(Center, W, -W, 0, 0, 0, H, color, 1);
		MakeLines(Center, W, W, 0, 0, 0, H, color, 1);
	}
}

void DrawMenuControlsText() {
	char Output[0x200];
	if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS)) {
		if (!UserInGame()) {
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY);
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			sprintf(Output, "Press ^BXENONButtondpadL^ & ^BXENONButtontrigL^ To %s ^7XeLive Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText(Output, (X - 360), Y - 20, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.80, 0.80);
		}
		else {
			sprintf(Output, "Press ^BXENONButtondpadL^ & ^BXENONButtontrigL^ To %s ^7XeLive Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText(Output, (cg_BO2->refdef.Width - 1200), cg_BO2->refdef.Height - 30, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.80, 0.80);
		}
	}
}

void GetSpread_BO2(int* seed, float* x, float* y, int playerStatePointer)
{
	float min, max, spread, fx, fy;
	int randSeed = *seed;
	BG_GetSpreadForWeapon_BO2(playerStatePointer, Entities_BO2[cg_BO2->ClientNumber].WeaponIndex, &min, &max);
	spread = (max - min) * (*(FLOAT*)((DWORD)cg_BO2 + 0x80B80) * 0.00392156862) + min;
	fx = 0.01745329238474369 * RandomBulletDir_BO2(&randSeed) * 360;
	sub_82697FC0(&randSeed);
	fy = RandomBulletDir_BO2(&randSeed);
	*x = cos(fx) * fy * spread;
	*y = sin(fx) * fy * spread;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_BO2->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_BO2->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_BO2->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_BO2->ViewAngles.y + smoothdiff[1];
	if (FinishedAngles[0] > 180)  FinishedAngles[0] -= 360;
	if (FinishedAngles[1] > 180)  FinishedAngles[1] -= 360;
	if (FinishedAngles[0] < -180) FinishedAngles[0] += 360;
	if (FinishedAngles[1] < -180) FinishedAngles[1] += 360;
	return FinishedAngles;
}

signed char ClampChar(int i)
{
	if (i < -128)
		return -128;

	if (i > 127)
		return 127;

	return i;
}

void FixTextForDistance(CONST PCHAR Text, float* x, float* y, DWORD Font, float scale, Vector3 WorldPos, float* OutDist)
{
	float distanceSq = cg_BO2->refdef.ViewOrg.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth(1, Text, 0x7FFFFFFF, Font) * R_NormalizedTextScale_BO2(Font, scale * distScale)) * 0.8) - *x) + 0.8));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.8));
}

bool ClientWorld2SCreen(float* ClientHeadOrigin, float* ClientViewAngles)
{
	D3DXVECTOR3 ViewAxis[3];
	AnglesToAxis(ClientViewAngles, (float*)ViewAxis);
	Vector3 vLocal;
	VectorSubtract(cg_BO2->refdef.ViewOrg, ClientHeadOrigin, vLocal);
	float flZ = DotProduct(ViewAxis[0], vLocal);
	float flX = DotProduct(ViewAxis[1], vLocal);
	float flY = DotProduct(ViewAxis[2], vLocal);
	if (flZ >= 0.0f)
		return true;

	return false;
}

bool IsWeaponDualWield()
{
	return (*(BYTE*)(BG_GetWeaponDef_BO2(Entities_BO2[cg_BO2->ClientNumber].WeaponIndex) + 0x64F) == (BYTE)0x1);
}

void CalcPrediction(int clientIndex, Vector3 BonePos, int Scale)
{
	Vector3 EnemyVelocityTemp;
	Vector3 EnemyVelocity;

	VectorSubtract(*(Vector3*)((DWORD)&Entities_BO2[clientIndex] + 0x168), *(Vector3*)((DWORD)&Entities_BO2[clientIndex] + 0x1E8), EnemyVelocityTemp);

	EnemyVelocity.x = EnemyVelocityTemp[0];
	EnemyVelocity.y = EnemyVelocityTemp[1];
	EnemyVelocity.z = EnemyVelocityTemp[2];

	Vector3 CalcVec = EnemyVelocity.Normalize();

	CalcVec.x *= Scale;
	CalcVec.y *= Scale;
	CalcVec.z *= Scale;

	BonePos[0] += CalcVec.x;
	BonePos[1] += CalcVec.y;
}

bool IsPrimaryShield(int ClientIndex)
{
	Vector3 HeadPos, HeadPosOut;
	GetTagPosistion(&Entities_BO2[ClientIndex], ClientIndex, "j_head", (float*)&HeadPos);

	float pointlessScreenOrigin[3];
	Vector3 ClientAngles = *(Vector3*)((DWORD)cg_BO2 + 0x69F54 + (ClientIndex * 0x808));
	if ((Entities_BO2[ClientIndex].WeaponIndex == CG_GetWeaponIndexForName_BO2("riotshield_mp")) && ClientWorld2SCreen((float*)&HeadPos, (float*)&ClientAngles))
	{
		return true;
	}

	return false;
}

bool IsSecondaryShield(int ClientIndex)
{
	Vector3 HeadPos, HeadPosOut;
	GetTagPosistion(&Entities_BO2[ClientIndex], ClientIndex, "j_head", (float*)&HeadPos);

	float pointlessScreenOrigin[3];
	Vector3 ClientAngles = *(Vector3*)((DWORD)cg_BO2 + 0x69F54 + (ClientIndex * 0x808));
	if ((*(int*)((DWORD)&Entities_BO2[ClientIndex] + 0x1B4) == CG_GetWeaponIndexForName_BO2("riotshield_mp")) && !ClientWorld2SCreen((float*)&HeadPos, (float*)&ClientAngles))
	{
		return true;
	}

	return false;
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos, Start;

	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
		return false;

	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS) && UsingShield)
		TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_BO2[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;
	CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
	
	BulletFireParams bulletFireParams;
	BulletTraceResults BTR;
	bulletFireParams.ignoreEntIndex = cg_BO2->ClientNumber;
	bulletFireParams.weaponEntIndex = 1022;
	bulletFireParams.damageMultiplier = 1.0f;
	bulletFireParams.methodOfDeath = 2;
	VectorCopy(MyPos, bulletFireParams.origStart);
	VectorCopy(MyPos, bulletFireParams.start);
	VectorCopy(TagPos, bulletFireParams.end);
	VectorSubtract(TagPos, MyPos, dir);
	Angles = VectorToAngles(dir);
	
	AngleVectors(Angles, &Hax[0], NULL, NULL);
	VectorCopy(Hax[0], bulletFireParams.dir);

	// Disable the bullet traces
	*(QWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x386000004E800020; // DynEntCl_DynEntImpactEvent
	*(QWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x386000004E800020; // CG_BulletHitEvent
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.UnkBulletTracePatch) = 0x48000218;

	// Disable BG_HasPerk's check for FMJ to fix some spots
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix1) = 0x38600000;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix2) = 0x38600000;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix3) = 0x38600000;

	// Allow 360 degree autowall checks
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.Autowall360) = 0x38600001;

	// Disable the games DrawBulletImpacts call
	*(DWORD*)0x82259B40 = 0x60000000;

	FireBulletPenetrate_BO2(0, &bulletFireParams, Entities_BO2[cg_BO2->ClientNumber].WeaponIndex, &Entities_BO2[cg_BO2->ClientNumber], MyPos, 0, 1, &BTR);

	// Re-enable the bullet traces
	*(QWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x7D8802A69181FFF8; // DynEntCl_DynEntImpactEvent
	*(QWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x7D8802A6486BDA55; // CG_BulletHitEvent
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.UnkBulletTracePatch) = 0x419A00AC;

	// Re-enable BG_HasPerk's check for FMJ
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix1) = 0x4BFA1B15;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix2) = 0x4BFA184D;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix3) = 0x4BFA17ED;

	// Unpatch 360 degree autowall check
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.Autowall360) = 0x4BFF4301;

	// Unpatch the games DrawBulletImpacts call
	*(DWORD*)0x82259B40 = 0x4BFFF669;

	return (bulletFireParams.ignoreEntIndex == Index);
}

void CorrectMovement(usercmd_s_BO2 *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_BO2->ViewAngles);

	int forward = usercommand->movement[0];
	int side = usercommand->movement[1];

	if (forward >= 128)
	{
		forward = -(256 - forward);
	}

	if (side >= 128)
	{
		side = -(256 - side);
	}

	int fixedforward = (cos(0.01745329238474369 * Fix.y) * forward) + (cos(0.01745329238474369 * Fix.y + 90) * side);
	int fixedside = (sin(0.01745329238474369 * Fix.y) * forward) + (sin(0.01745329238474369 * Fix.y + 90) * side);

	usercommand->movement[0] = ClampChar(fixedforward);
	usercommand->movement[1] = ClampChar(fixedside);
}

bool TriggerBot(int Index)
{
	Vector3 HeadOrigin, Hitboxes;
	Vector3 FeetOrigin = Entities_BO2[Index].Origin;
	GetTagPosistion(&Entities_BO2[Index], Index, "j_head", (float*)&HeadOrigin);
	FeetOrigin.z -= 5;
	HeadOrigin.z += 10;
	Vector2 FeetLocation, HeadLocation;
	if (WorldPosToScreenPos((float*)&FeetOrigin, &FeetLocation) && WorldPosToScreenPos((float*)&HeadOrigin, &HeadLocation))
	{
		float fHeight = FeetLocation.y - HeadLocation.y;
		float fWidth = fHeight / 1.5;
		Vector2 ScreenPos;
		for (int b = 0; b < 14; b++)
		{
			GetTagPosistion(&Entities_BO2[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_BO2->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_BO2->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_BO2->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_BO2->refdef.Height / 2))
				{

					return true;
				}
			}
		}
	}

	return false;
}

float FindSmoothScale()
{
	float SmoothScale = 0.5f;
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "0.5")
		SmoothScale = 0.5f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "1.0")
		SmoothScale = 1.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "2.0")
		SmoothScale = 2.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "3.0")
		SmoothScale = 3.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "4.0")
		SmoothScale = 4.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "5.0")
		SmoothScale = 5.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "10.0")
		SmoothScale = 10.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "15.0")
		SmoothScale = 15.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "20.0")
		SmoothScale = 20.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "25.0")
		SmoothScale = 25.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "30.0")
		SmoothScale = 30.0f;

	return SmoothScale;
}

bool FOVCheck(int index, PCHAR Tag)
{
	Vector2 ScreenPos; float Output[3];
	int FOVSize = FindFOVSize();
	GetTagPosistion(&Entities_BO2[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_BO2->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_BO2->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_BO2->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_BO2->refdef.Height / 2) + FOVSize))
		{
			return true;
		}
	}

	return false;
}

float ScaleRadarX(float X, float ScreenWidth)
{
	return (X * (640.0f / ScreenWidth));
}

float ScaleRadarY(float Y, float ScreenHeight)
{
	return (Y * (480.0f / ScreenHeight));
}

//void WorldToCompass_BO2(unkRadarStruct* radarHud, Vector2& size, Vector3& World, Vector2& Compass, Vector2& widthHeight)
//{
//	Vector4 RadarSize;
//	CG_CompassCalcDimensions_BO2(0, cg_BO2, size, radarHud, &RadarSize.x, &RadarSize.y, &RadarSize.z, &RadarSize.a);
//
//	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
//	float y2 = (RadarSize.a * 0.5) + RadarSize.y;
//
//	Vector2 out, outClipped, Yaw;
//
//	CG_CompassUpYawVector_BO2(cg_BO2, Yaw);
//
//	CG_WorldPosToCompass_BO2(0, cg_BO2, RadarSize.z, RadarSize.a, Yaw, Entities_BO2[cg_BO2->ClientNumber].Origin, World, out, outClipped);
//
//	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
//	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;
//}


void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD))
	{
		if (Event == 23)
		{
			short ClientReloadIndex = *(short*)(entityState_t + 0x2C2);
			if (ClientReloadIndex == (short)cg_BO2->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 29)
		{
			short ClientSwitchIndex = *(short*)(entityState_t + 0x2C2);
			if (WasReloading && yyCount == 1)
				YButtonPressed = FALSE;
		}
	}

	CG_EntityEventDetour->CallOriginal(localClient, entityState_t, Event, eventParam);
}

DWORD NetDll_XNetGetTitleXnAddrHook(XNCALLER_TYPE xnc, XNADDR *pxna)
{
	DWORD retVal = NetDll_XNetGetTitleXnAddrDetour->CallOriginal(xnc, pxna);

	pxna->ina.S_un.S_un_b.s_b1 = 13;
	pxna->ina.S_un.S_un_b.s_b2 = 37;
	pxna->ina.S_un.S_un_b.s_b3 = 13;
	pxna->ina.S_un.S_un_b.s_b4 = 37;

	pxna->ina.S_un.S_addr = 13371337;

	return retVal;
}

DWORD XInputGetStateHook(DWORD dwUserIndex, PXINPUT_STATE pState)
{
	if (dwUserIndex != 0)
		return XInputGetStateDetour->CallOriginal(dwUserIndex, pState);

	DWORD Result = XInputGetStateDetour->CallOriginal(dwUserIndex, pState);
	if (FAILED(Result) || !pState)
		return Result;

	// Handle Auto Reload Cancel
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD) && UserInGame())
	{
		if (WasReloading)
		{
			if (!YButtonPressed)
			{
				if (!((pState->Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y))
					pState->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
				YButtonPressed = TRUE;
				yyCount++;
			}
			if (yyCount > 1)
			{
				WasReloading = FALSE;
				yyCount = 0;
				YButtonPressed = FALSE;
			}
		}
	}

	// Auto Aim
	if ((GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOAIM) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT)) && (GClosestClient != -1))
		pState->Gamepad.bLeftTrigger = 100;

	// Remove game input when menu is open
	if (menuOpen)
		pState->Gamepad.wButtons &= ~(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B);

	// Rapid Fire
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE))
	{
		if (pState->Gamepad.bRightTrigger > 0)
		{
			TriggerValue = pState->Gamepad.bRightTrigger;
			pState->Gamepad.bRightTrigger = 0;
		}
		else
		{
			TriggerValue = 0;
		}
	}

	return Result;
}

void GetRadarXY(float *x, float *y)
{
	*x = ScaleRadarX(12, screenMaxX);
	*y = ScaleRadarY(12, screenMaxY);
}

struct CRectDef
{
	float x; //0x0
	float y; //0x4
	float w; //0x8
	float h; //0xC
	DWORD horizAlign; //0x10
	DWORD vertAlign;
	CRectDef()
	{
		x = 0.0f;
		y = 0.0f;
		w = 0.0f;
		h = 0.0f;
		horizAlign = 0;
		vertAlign = 0;
	}
	CRectDef(float xIn, float yIn, float wIn, float hIn, DWORD horizAlignIn, DWORD vertAlignIn = 0)
	{
		x = xIn;
		y = yIn;
		w = wIn;
		h = hIn;
		horizAlign = horizAlignIn;
		vertAlign = vertAlignIn;
	}
};

void GetRectDef(float radarX, float radarY, CRectDef *rectDef)
{
	rectDef->x = radarX;
	rectDef->y = radarY;
	rectDef->w = 102;
	rectDef->h = 102;
	rectDef->horizAlign = 0x08080000;
}

BOOL CalcCompassDimensions(DWORD cg_t, DWORD CompassType, Vector2& size, DWORD rectDef, Vector4& radarSize)
{
	return ((BOOL(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassCalcDimensions))(CompassType, cg_t, &size, rectDef, &radarSize.x, &radarSize.y, &radarSize.z, &radarSize.a);
}
vec3 out;
BOOL WorldPosToCompass(DWORD cg_t, DWORD CompassType, Vector4& RadarSize, Vector2& Yaw, Vector3& refOrigin, vec3 *worldPos, Vector2& outClipped)
{
	return ((BOOL(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_WorldPosToCompass))(CompassType, cg_t, &RadarSize, &Yaw, &refOrigin, worldPos, &out, &outClipped);
}

void CompassUpYawVector(DWORD cg_t, Vector2& Yaw)
{
	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassUpYawVector))(cg_t, &Yaw);
}

void WorldToCompass_Internal(DWORD cg_t, vec3 *World, Vector2& Compass)
{
	if (screenMaxX == NULL || screenMaxY == NULL)
	{
		float aspect;
		((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_GetScreenDimensions))(&screenMaxX, &screenMaxY, &aspect);
	}

	Vector4 RadarSize;
	float radarX = 0, radarY = 0;
	GetRadarXY(&radarX, &radarY);

	CRectDef rectDef;
	GetRectDef(radarX, radarY, &rectDef);

	Vector2 size(radarX, radarY);
	CalcCompassDimensions(cg_t, LastCompassType, size, (DWORD)&rectDef, RadarSize);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 Yaw(0, 0);
	CompassUpYawVector(cg_t, Yaw);

	Vector2 outClipped(0, 0);
	//vec3 myOrigin = GameFuncs::getLocalViewOrigin(&myOrigin);
	int playerStatePtr = cg_t + 0x480A8;
	Vector3 MyPos;
	CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
	WorldPosToCompass(cg_t, LastCompassType, RadarSize, Yaw, MyPos, World, outClipped);

	Vector2 widthHeight(16, 16);
	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y - 5.0f;
}
void WorldToCompass(vec3 *World, Vector2& Compass)
{
	int cg_t = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		WorldToCompass_Internal(cg_t, World, Compass);
}

void WorldToCompass(Vector3& World, Vector2 &Compass)
{
	WorldToCompass((vec3*)&World.x, Compass);
}

DWORD getRefDef()
{
	int cg_t = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		return cg_t + 0x4D880;
	return 0;
}
int getViewAxis_ptr()
{
	DWORD refDef = getRefDef();
	if (refDef)
		return refDef + 0x48;//0x401A90
	return 0;
}

vec3 getRefDefViewAngles()
{
	int viewAxis_ptr = getViewAxis_ptr();
	if (viewAxis_ptr)
	{
		vec3_t axisAngles[3];
		//float axisAngles[9];
		for (int i = 0; i<3; i++)
		{
			axisAngles[0][i] = *(float*)(viewAxis_ptr + (i * 0x04));
			axisAngles[1][i] = *(float*)(viewAxis_ptr + 0xc + (i * 0x04));
			axisAngles[2][i] = *(float*)(viewAxis_ptr + 0x18 + (i * 0x04));
		}

		float newAngles[3];
		AxisToAngles((Vector3*)axisAngles, newAngles);
		return vec3(newAngles);
	}
	return vec3(0, 0, 0);
}

void DrawCustomRadar()
{
	// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
	for (int i = 18; i < 0x7FF; i++)
	{
		// Draw Dropped Items
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
		{
			BOOL IsWeapon = TRUE;

			// Make sure it's a dropped item
			if ((*(int*)((DWORD)&Entities_BO2[i] + 0x36C) & 0x40000000) != 0 && Entities_BO2[i].Type == 3)
			{
				// Make sure it's a scavenger pack
				if ((Entities_BO2[i].WeaponNum != CG_GetWeaponIndexForName_BO2("scavenger_item_mp")))
					IsWeapon = FALSE;

				// Set color based on weapon/pack
				Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 1) : Vector4(0.51f, 1, 1, 1);
				if (IsWeapon)
				{
					if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
					{
						Vector2 compass;
						WorldToCompass(Entities_BO2[i].Origin, compass);
						((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.UI_DrawHandlePic))(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), compass.x, compass.y, 20.0f, 10.0f, 0x8, 0x8, 0.0f, &color, ScavMaterial);
					}
				}
			}
		}
	}


	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
	{
		// Loop for all clients
		for (int i = 0; i < 18; i++)
		{
			// Make sure the client is not us
			if (Entities_BO2[i].ClientNum != cg_BO2->ClientNumber && cg_BO2->ClientNumber != i)
			{
				// Entity check
				if (Entities_BO2[i].Type != 1 && Entities_BO2[i].Type != 16)
					continue;

				// Alive and is player
				if ((*(int*)((DWORD)&Entities_BO2[i] + 0x36C) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO2[i] + 0x1D4) & 0x40000))
				{
					// Invalid player
					continue;
				}

				//if (LastCompassType == 1)
				//	goto skip;

				int centity = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer);

				if (centity)
				{
					centity = centity + (i * 0x374);

					vec3 entityView = *(vec3*)(centity + 0x18c);
					vec3 myView = getRefDefViewAngles();

					float yaw = entityView.y;

					float rotation = (360.0f - yaw);
					if (LastCompassType == 0)
						rotation += myView.y;
					//else if (currentTitle == MW3_TITLE_ID)
					//	rotation += 90.0f;

					float w = 20.0, h = 20.0;
					Vector2 Compass;
					WorldToCompass(Entities_BO2[i].Origin, Compass);
					((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.UI_DrawHandlePic))(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), Compass.x, Compass.y, w, h, 0x8, 0x8, rotation, PlayerInfo[i].OnTeam ? (float*)&Vector4(0, 1, 0, 0.75f) : (float*)&Vector4(1, 0, 0, 0.75f), ArrowIcon);
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}

	skip:
		if (LastCompassType == 0) //draw our own pointer
		{
			Vector2 Compass;
			DWORD cg_t = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
			if (cg_t)
			{
				int playerStatePtr = cg_t +0x480A8;
				Vector3 MyPos;
				CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
				WorldToCompass(MyPos, Compass);
				float w = 20.0f, h = 20.0f;
				((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.UI_DrawHandlePic))(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), Compass.x, Compass.y, w, h, 0x8, 0x8, 0.0f, &GetColorCode(colorWhite), ArrowIcon);
			}
		}
	}
}

void DrawESP()
{
	LocalClient_BO2 = (pclientActive_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu18
	Entities_BO2 = (pcentity_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu18
	cg_BO2 = (pcg_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu18
	cgs_BO2 = (pcgs_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu18

	// If in-game, start our ESP
	if (UserInGame() && menuReady && (cgs_BO2 != NULL))
	{
		// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES))
			{
				// Make sure it's a explosive
				if ((*(int*)((DWORD)&Entities_BO2[i] + 0x36C) & 0x40000000) != 0 && Entities_BO2[i].Type == 4)
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					//CG_GetEntityBModelBounds(&Entities_BO2[i], (float*)&Mins, (float*)&Maxs);
					TopLocation = Entities_BO2[i].Origin; //TopLocation.z -= Mins.z;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						Vector4 color = Vector4(1, 1, 0, 1);

						float aspectX, aspectY;
						int ExplosiveIcon = GetWeaponIcon(Entities_BO2[i].WeaponNum, &aspectX, &aspectY);
						if (ExplosiveIcon != 0 && MmIsAddressValid((DWORD*)ExplosiveIcon))
						{
							CG_DrawRotatedPicPhysical(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - (aspectX / 2), TopLocationOut.y - (aspectY / 2), aspectX, aspectY, 0.0, GetColorCode(colorYellow), ExplosiveIcon);
						}
					}
				}
			}

			// Draw Dropped Items
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
			{
				BOOL IsWeapon = TRUE;

				// Make sure it's a dropped item
				if ((*(int*)((DWORD)&Entities_BO2[i] + 0x36C) & 0x40000000) != 0 && Entities_BO2[i].Type == 3)
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					//CG_GetEntityBModelBounds(&Entities_BO2[i], (float*)&Mins, (float*)&Maxs);
					TopLocation = Entities_BO2[i].Origin;// TopLocation.z -= Mins.z;

					BOOL onScreen = WorldToScreen(TopLocation, TopLocationOut);

					if (onScreen)
					{
						// Make sure it's not a scavenger pack
						if ((Entities_BO2[i].WeaponNum != CG_GetWeaponIndexForName_BO2("scavenger_item_mp")))
							IsWeapon = FALSE;

						// Set color based on weapon/pack
						Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 1) : Vector4(0.51f, 1, 1, 1);
						FLOAT distance = GetDistance(cg_BO2->refdef.ViewOrg, Entities_BO2[i].Origin);
						if (!IsWeapon)
						{
							if (onScreen)
							{
								float aspectX, aspectY;
								int DroppedIcon = GetWeaponIcon(Entities_BO2[i].WeaponNum, &aspectX, &aspectY);
								if (DroppedIcon != 0 && MmIsAddressValid((DWORD*)DroppedIcon) && distance < 1500.0f)
								{
									CG_DrawRotatedPicPhysical(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - (aspectX / 2), TopLocationOut.y - (aspectY / 2), aspectX, aspectY, 0.0, color, DroppedIcon);
								}
							}
						}
						else
						{
							if (distance < 1500.0f && onScreen)
							{
								CG_DrawRotatedPicPhysical(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - 30, TopLocationOut.y - 10, 50, 25, 0.0, color, ScavMaterial);
							}
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < 1024; i++)
		{
			// Make sure the client is not us
			if (Entities_BO2[i].ClientNum != cg_BO2->ClientNumber && cg_BO2->ClientNumber != i)
			{
				// Entity check
				if (Entities_BO2[i].Type != 1 && Entities_BO2[i].Type != 16)
					continue;

				// Alive and is player
				if ((*(int*)((DWORD)&Entities_BO2[i] + 0x36C) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO2[i] + 0x1D4) & 0x40000))
				{
					// Invalid player
					continue;
				}

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_BO2[i].ClientNum;
				PlayerInfo[i].Name = cg_BO2->clientInfo[i].Name;

				// Team check
				if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
				{
					if ((cg_BO2->clientInfo[cg_BO2->ClientNumber].Team == cg_BO2->clientInfo[i].Team) && (cg_BO2->clientInfo[cg_BO2->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO2->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
							continue;
					}
					else
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
							continue;

						PlayerInfo[i].Team = cg_BO2->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				}
				else
				{
					// Reverse team check
					if ((cg_BO2->clientInfo[cg_BO2->ClientNumber].Team == cg_BO2->clientInfo[i].Team) && (cg_BO2->clientInfo[cg_BO2->ClientNumber].Team != 0))
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
							continue;

						PlayerInfo[i].Team = cg_BO2->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
					else
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO2->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
							continue;
					}
				}

				// Zombies
				if (i > 18)
					PlayerInfo[i].OnTeam = FALSE;

				// Dogs
				if (Entities_BO2[i].Type == 16)
				{
					if (cg_BO2->clientInfo[cg_BO2->ClientNumber].Team == Entities_BO2[i].TeamDog)
					{
						PlayerInfo[i].OnTeam = TRUE;
					}
					else
					{
						PlayerInfo[i].OnTeam = FALSE;
					}
				}

				// Visible / Wallbang Check
				if (!PlayerInfo[i].OnTeam)
				{
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO2(0, &Entities_BO2[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_BO2[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					BYTE CL_TraceResult[100];
					CG_LocationalTraceBO2((void*)CL_TraceResult, &cg_BO2->refdef.ViewOrg, &PlayerInfo[i].TagPosistion, i, 0x803003, 0, 0);

					// If the trace fraction is >= 0.97 let's say they are visible
					float traceFraction = *(float*)(CL_TraceResult + 0x10);
					PlayerInfo[i].VisibleESP = ((traceFraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO2(0, &Entities_BO2[i], TagToShort("j_head"));

					if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
						PlayerInfo[i].Wallbangable = FALSE;
					//else
					//	PlayerInfo[i].Wallbangable = Autowall(i, "j_head");
					PlayerInfo[i].GVisibleESP = (PlayerInfo[i].Wallbangable || PlayerInfo[i].VisibleESP);
				}

				// If Whitelisted
				if (ClientOptionEnabled[2][i])
					PlayerInfo[i].OnTeam = TRUE;

				// Setup the colors for ESP
				Vector4 Color = PlayerInfo[i].OnTeam ? Vector4(0.51f, 1, 1, 0.75f) : GetColorCode(colorOrange); // Vector4(0.51f, 1, 1, 0.50f)

				// If Blacklisted
				if (ClientOptionEnabled[1][i])
					Color = Vector4(1, 1, 1, 0.50f);

				if (PlayerInfo[i].GVisibleESP)
				{
					if (!PlayerInfo[i].OnTeam)
					{
						if (!ClientOptionEnabled[1][i])
							Color = Vector4(1, 1, 0, 0.50f);

						if (PlayerInfo[i].Wallbangable && !PlayerInfo[i].VisibleESP)
							Color = Vector4(0.502f, 0, 0.502f, 0.50f);
					}
				}

				// ESP Snaplines
				if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Off")
				{
					DrawSnapLine(Color, i, PlayerInfo[i].OnTeam);
				}

				// ESP Arrows
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DIRECTIONALARROWS))
				{
					DrawArrows((float*)&Color, i);
				}

				// ESP Bones
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWBONES))
				{
					DrawBones(Color, i);
				}

				// ESP Boxes
				if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "Off")
				{
					DrawESPBox(Color, i);
				}

				// ESP Weapon 
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWWEAPON) && Entities_BO2[i].Type == 1)
				{
					Vector3 Pos[3], xy1[2], FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_BO2[i], i, "j_helmet", (float*)&Pos[1]);
					FeetPos = Entities_BO2[i].Origin;

					if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					{
						float aspectX, aspectY;
						int WeaponIcon = GetWeaponIcon(Entities_BO2[i].WeaponIndex, &aspectX, &aspectY);
						if (WeaponIcon != 0 && MmIsAddressValid((DWORD*)WeaponIcon))
						{
							CG_DrawRotatedPicPhysical(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), xy1[0].x - (aspectX / 2), xy1[0].y - (aspectY / 2) - 10, aspectX, aspectY, 0.0, Color, WeaponIcon);
						}

						//PCHAR WeaponName = GetWeaponHudName(Entities_BO2[i].CurrentWeapon);
						//if (MmIsAddressValid((PCHAR*)WeaponName))
						//	DrawText_BO2(WeaponName, (xy1[0].x - (R_TextWidth_BO2(WeaponName, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), (xy1[0].y), 0, 0, SmallFont, Vector4(1, 1, 1, 0.50f), 0.5f, 0.5f);
					}
				}

				// ESP Name
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_BO2[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_BO2[i].Origin;

					if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					{
						if (MmIsAddressValid((PCHAR*)cg_BO2->clientInfo[i].Name))
						{
							float DisanceOut; char* Name;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							if (Entities_BO2[i].Type == 1)
								Name = cg_BO2->clientInfo[i].Name;
							else if (Entities_BO2[i].Type == 16 && Zombies)
								Name = "Zombie";
							else if (Entities_BO2[i].Type == 16 && !Zombies)
								Name = "Dog";

							FixTextForDistance(Name, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText(Name, (FeetPosOutOG.x - (R_TextWidth(1, Name, 0x7FFFFFFF, SmallFont) / 2) * 0.8f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.8f, 0.8f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText("Visible Enemy!", ((cg_BO2->refdef.Width / 2) - (R_TextWidth(1, "Visible Enemy!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_BO2->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_BO2[i], i, "j_head", OutPos);
						if ((GetDistance(cg_BO2->refdef.ViewOrg, OutPos) / 100 < 5))
						{
							DrawText("Proximity Warning!", ((cg_BO2->refdef.Width / 2) - (R_TextWidth(1, "Proximity Warning!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_BO2->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
						}
					}
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}
	}
}

void R_EndFrameHook() {
	LocalClient_BO2 = (pclientActive_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu18
	Entities_BO2 = (pcentity_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu18
	cg_BO2 = (pcg_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu18
	cgs_BO2 = (pcgs_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu18

	// Setup the menu
	if (!menuReady) {
		SetupMenu();
	}

	// Monitor button presses for the menu
	HandleInput();

	// Draw our menu
	if (menuOpen) RenderMenu();

	HandleToggleOptions(cgs_BO2);

	if (!UserInGame() && menuReady) {
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEXeLiveTEXT)) {
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)CheatPatches->CoDPatches.CG_S_XY;
			Y = (FLOAT)*(DWORD*)(CheatPatches->CoDPatches.CG_S_XY + 0x4);
			DrawText("XeLive Engine v1.0", ((X - 130) - (R_TextWidth(1, "XeLive Engine v1.0", 0x7FFFFFFF, SmallFont) / 2) * 0.7f), (Y / 2) - 330, 0, 0, SmallFont, Vector4(0.102f, 1, 0.102f, 0.50f));
		}
	}
	else if (UserInGame() && menuReady)
	{
		// XeLive Engine v1.0
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEXeLiveTEXT))
			DrawAd();
	}

	// Draw our menu controls
	DrawMenuControlsText();

	bool inGame = UserInGame();
	// Update anything once we enter the game once
	if (inGame && !OnInit)
	{
		OnInit = TRUE;

		// Check for zombie map
		if (strstr(cgs_BO2->Mapname, "zm_"))
			Zombies = TRUE;
		else
			Zombies = FALSE;
	}
	else if (!inGame && OnInit)
	{
		OnInit = FALSE;
	}

	if(inGame && OnInit)
		DrawCustomRadar();

	// Call the original function
	R_EndFrameDetour->CallOriginal(0);
}

void Menu_PaintAllHook(int r3, int r4)
{
	LocalClient_BO2 = (pclientActive_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu18
	Entities_BO2 = (pcentity_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu18
	cg_BO2 = (pcg_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu18
	cgs_BO2 = (pcgs_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu18

	// Draw our ESP
	DrawESP();

	// Call the original
	return Menu_PaintAllDetour->CallOriginal(r3, r4);
}

int FindBestBoneByDamage(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	float totalDamage = 0;
	DWORD BoneIndex = 0;
	BYTE CL_TraceResult[100];
	float traceFraction;

	// Our hit location indexs
	BYTE HitLocationIndexs[7] = { HITLOC_HEAD, HITLOC_TORSO_LWR, HITLOC_TORSO_UPR, HITLOC_L_ARM_UPR, HITLOC_R_ARM_UPR, HITLOC_L_FOOT, HITLOC_R_FOOT };
	BYTE HitLocTest[13] = { HITLOC_HEAD,  HITLOC_NECK, HITLOC_TORSO_UPR, HITLOC_TORSO_MID, HITLOC_TORSO_LWR, HITLOC_R_ARM_UPR, HITLOC_L_ARM_UPR, HITLOC_R_HAND, HITLOC_L_HAND, HITLOC_R_LEG_UPR, HITLOC_L_LEG_UPR, HITLOC_R_FOOT, HITLOC_L_FOOT};
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO2[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
		CG_LocationalTraceBO2((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult + 0x10);
		if (traceFraction >= 0.95f)
			return 5;

		return -1;
	}

	for (int i = 0; i < 13; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO2[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
		CG_LocationalTraceBO2((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult + 0x10);
		if (traceFraction >= 0.95f)
		{
			// Get weapon def
			DWORD WeaponDef = BG_GetWeaponDef_BO2(Entities_BO2[cg_BO2->ClientNumber].WeaponIndex);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			float tempDamage = (float)*(int*)(WeaponDef + 0x3E4) * G_GetWeaponHitLocationMultiplier(HitLocTest[i], Entities_BO2[cg_BO2->ClientNumber].WeaponIndex);
			if (tempDamage > totalDamage)
			{
				totalDamage = tempDamage;
				BoneIndex = i;
			}
		}
	}

	if (totalDamage > 0)
		return BoneIndex;

	return -1;
}

int FindBestVisibleBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	BYTE CL_TraceResult[100];
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;
	float TraceFraction;

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO2[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
		CG_LocationalTraceBO2((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult + 0x10);
		if (TraceFraction >= 0.95f)
			return 6;

		return -1;
	}

	for (int i = 0; i < 13; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO2[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
		CG_LocationalTraceBO2((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult + 0x10);
		if (TraceFraction >= 0.95f)
			return i;

		continue;
	}

	return -1;
}

int FindBestAutoWallBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	for (int i = 0; i < 7; i++)
	{
		BOOL Wallbangable = FALSE;

		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		Wallbangable = Autowall(EntIndex, AutoWallBones[i], HasRiotShield);
		if (Wallbangable)
			return i;

		continue;
	}

	return -1;
}

BOOL IsPlayerEnemy(int ClientIndex, int myClientNum)
{
	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return FALSE;

	// Zombies / Dogs
	if (Entities_BO2[ClientIndex].Type == 16 && !Zombies)
	{
		if (Entities_BO2[ClientIndex].TeamDog == cg_BO2->clientInfo[myClientNum].Team)
			return FALSE;
		else
			return TRUE;
	}
	else if (Entities_BO2[ClientIndex].Type == 16 && Zombies)
	{
		return TRUE;
	}

	// Handle Normal / Inverted Team Check
	if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
	{
		if ((cg_BO2->clientInfo[myClientNum].Team == cg_BO2->clientInfo[ClientIndex].Team) && (cg_BO2->clientInfo[myClientNum].Team != 0))
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return FALSE;
			else
				return TRUE;
		}
	}
	else
	{
		// Reverse team check
		if ((cg_BO2->clientInfo[myClientNum].Team == cg_BO2->clientInfo[ClientIndex].Team) && (cg_BO2->clientInfo[myClientNum].Team != 0))
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return TRUE;
			else
				return FALSE;
		}
	}


	// We shouldn't get here
	return FALSE;
}

BOOL CanKillTarget(int ClientIndex)
{
	if ((*(int*)((DWORD)&Entities_BO2[ClientIndex] + 0x1B8) == 0x2 && *(int*)((DWORD)&Entities_BO2[ClientIndex] + 0x224) == 0x0))
		return false;
}

BOOL ValidPlayer(int ClientIndex, int myClientNum, BOOL UsingRagebot)
{
	// If it's us
	if (ClientIndex == myClientNum)
		return false;

	// Entity check
	if (Entities_BO2[ClientIndex].Type != 1 && Entities_BO2[ClientIndex].Type != 16)
		return false;

	// If zombie is dying
	if (Zombies && Entities_BO2[ClientIndex].Type == 16 && *(byte*)((DWORD)&Entities_BO2[ClientIndex] + 0x155) == 0x4)
		return false;

	// If alive
	if ((*(int*)((DWORD)&Entities_BO2[ClientIndex] + 0x36C) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO2[ClientIndex] + 0x1D4) & 0x40000))
		return false;
	
	// If killable
	//if (!CanKillTarget(ClientIndex) && Entities_BO2[ClientIndex].Type == 16)
	//	return false;

	// If enemy
	if (!IsPlayerEnemy(ClientIndex, myClientNum))
		return false;

	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return false;

	return true;
}

DWORD FindBestTarget(BOOL UsingRagebot)
{
	DWORD dwClosestClient = -1;
	FLOAT Closest = 900000;
	eMenuNames AimbotMenu = UsingRagebot ? MENU_RAGEBOT : MENU_LEGITBOT;

	// Grab cg to get playerstate pointer
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;

	// Loop for all players
	for (int i = 0; i < 1024; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO2->ClientNumber, UsingRagebot))
			continue;

		// Check the user for a shield
		if (Entities_BO2[i].WeaponIndex == CG_GetWeaponIndexForName_BO2("riotshield_mp"))
			UsingShield = TRUE;

		// If we want to ignore shields
		if (!GetMenuOptionValue(AimbotMenu, OPTION_TARGETSHIELDS) && UsingShield && !Zombies)
		{
			PlayerInfo[i].Wallbangable = FALSE; // Do this here since it doesn't get updated if we continue
			continue;
		}

		// Get our bone
		if (GetMenuOptionString(AimbotMenu, OPTION_AIMBONE) == "Best")
		{
			AimTag = "j_head";
			int BoneIndex = FindBestVisibleBoneIndex(i, UsingShield);
			if (BoneIndex == -1)
			{
				Visible = FALSE;
			}
			else
			{
				Visible = TRUE;
				AimTag = HitboxVisCheckBones[BoneIndex];
			}
		}
		else
		{
			AimTag = GetMenuOptionString(AimbotMenu, OPTION_AIMBONE);
		}

		
		// Get the players posistion for our visible check
		if (!Visible)
		{
			Vector3 VisPos, MyPos;
			GetTagPosistion(&Entities_BO2[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			BYTE CL_TraceResult[100];
			CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
			CG_LocationalTraceBO2((void*)CL_TraceResult, &MyPos, &VisPos, i, 0x803003, 0, 0);

			float traceFraction = *(float*)(CL_TraceResult + 0x10);

			// Check our trace fraction
			Visible = ((traceFraction >= 0.95f) ? TRUE : FALSE);
			if (!Visible && UsingRagebot)
			{
				// If they are prioritized, check multiple bones
				if (ClientOptionEnabled[1][i] && (GetMenuOptionString(MENU_RAGEBOT, OPTION_AIMBONE) == "Best"))
				{
					// Wallbang check 8 bones for prioritized users
					int AWallBoneIndex = FindBestAutoWallBoneIndex(i, UsingShield);
					if (AWallBoneIndex == -1)
					{
						Wallbangable = FALSE;
					}
					else
					{
						AimTag = AutoWallBones[AWallBoneIndex];
						Wallbangable = TRUE;
					}
		
				}
				else
				{
					// If they aren't visible, try to wallbang them
					Wallbangable = Autowall(i, AimTag, UsingShield);
					if (GetMenuOptionString(MENU_RAGEBOT, OPTION_AIMBONE) == "Best")
					{
						//if (!Wallbangable)
						//	Wallbangable = Autowall(i, "j_mainroot", UsingShield);
						//if (Wallbangable)
						//	AimTag = "j_mainroot";
					}
				}

				// Check a 2nd bone because the head is missed in various places
				/*if (!Wallbangable)
					Wallbangable = Autowall(i, "j_mainroot", UsingShield);
				if (Wallbangable)
					AimTag = "j_mainroot";

				// If they have anti-aim, lets check another bone
				if (ClientOptionEnabled[5][i])
				{
					if (!Wallbangable)
						Wallbangable = Autowall(i, "j_spineupper", UsingShield);
					if (Wallbangable)
						AimTag = "j_spineupper";
				}*/

				PlayerInfo[i].Wallbangable = Wallbangable;
			}
		}

		// Verify all were true
		if (UsingRagebot)
		{
			if (!Visible && !Wallbangable)
				continue;
		}
		else
		{
			// Make sure they are within our FOV for Legitbot
			BOOL WithinFOV = FOVCheck(i, AimTag);

			Visible = (Visible && WithinFOV);

			if (!Visible)
				continue;
		}

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO2[i].Origin, (float*)&cg_BO2->refdef.ViewOrg));

		// For now just make them seem closest
		if (ClientOptionEnabled[1][i])
			TempDistance = 5.0f;

		// Update our closest player info
		if (TempDistance < Closest)
		{
			dwClosestClient = i;
			Closest = TempDistance;
		}
	}

	return dwClosestClient;
}

DWORD FindBestAntiAimTarget()
{
	DWORD dwAAClosestClient = -1;
	FLOAT ClosestAA = 900000;

	// Loop for all players
	for (int i = 0; i < cgs_BO2->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO2->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO2[i].Origin, (float*)&cg_BO2->refdef.ViewOrg));

		// For now just make them seem closest
		if (ClientOptionEnabled[1][i])
			TempDistance = 5.0f;

		// Update our closest player info
		if (TempDistance < ClosestAA)
		{
			dwAAClosestClient = i;
			ClosestAA = TempDistance;
		}
	}

	return dwAAClosestClient;
}

VOID Ragebot(pusercmd_s_BO2 usercommand, pusercmd_s_BO2 oldusercommand, pusercmd_s_BO2 nextusercommand, float movementsave)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, SpreadAngles;
	PCHAR AimbotTag = "j_head";
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;
	XINPUT_STATE State;
	int randomVal = rand() % 11;

	// Get our best target
	DWORD BestTarget = FindBestTarget(TRUE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Handle Primary Shielders
	if (IsPrimaryShield(BestTarget) && GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS) && Entities_BO2[BestTarget].Type != 16)
		AimTag = (randomVal > 5) ? "j_ankle_ri" : "j_ankle_le";

	// Handle Anti-Aim Shielders
	if (IsSecondaryShield(BestTarget) && Entities_BO2[BestTarget].Type != 16)
	{
		// Set the tag to left/right knee
		AimTag = (randomVal > 5) ? "j_knee_le" : "j_hip_ri";

		// If they are crouching, we can only hit the feet
		if (*(int*)((DWORD)&Entities_BO2[BestTarget] + 0x1D4) & 0x04)
			AimTag = (randomVal > 5) ? "j_ankle_ri" : "j_knee_le";
	}

	// Get the best targets posistion
	GetTagPosistion(&Entities_BO2[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);

	// Calculate prediction if we want it
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) != "Off")
		CalcPrediction(BestTarget, EnemyPosistion, FindPredicationScale());

	// Get our view origin
	CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO2->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Apply our aimbot angles
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Silent")
	{
		usercommand->ViewAngles[0] = ANGLE2SHORT(FinalAngles.x);
		usercommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y);

		// Fix our movement
		CorrectMovement(usercommand, FinalAngles);
	}
	else if(GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Normal")
	{
		// Set our angles 
		LocalClient_BO2->ViewAngles[0] = FinalAngles.x;
		LocalClient_BO2->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
	{
		// Get our seed from the current server time
		int Seed = nextusercommand->serverTime;

		// Transform the seed
		TransformSeed_BO2(&Seed);

		// Calculate our no spread
		GetSpread_BO2(&Seed, &SpreadAngles.x, &SpreadAngles.y, playerStatePtr);

		// Apply our no spread
		usercommand->ViewAngles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->ViewAngles[1] -= ANGLE2SHORT(-SpreadAngles.x);
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT) && (Entities_BO2[cg_BO2->ClientNumber].WeaponIndex != CG_GetWeaponIndexForName_BO2("riotshield_mp")))
	{
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
		{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			if (IsWeaponDualWield())
				usercommand->Buttons |= 0x00100280;
		}
		else
		{
			// If not using no spread, wait until aimed in before shooting
			if (cg_BO2->ZoomState >= 0.5f)
			{
				nextusercommand->Buttons |= 0x80000000;
				if (usercommand->Buttons & 0x80000000)
				{
					usercommand->Buttons &= 0x7FFFFFFFu;
				}

				if (IsWeaponDualWield())
					usercommand->Buttons |= 0x00100280;
			}
		}
	}

	// Hit shots without autoshoot
	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT) && (State.Gamepad.bRightTrigger > 35))
	{
		nextusercommand->Buttons |= 0x80000000;
		if (usercommand->Buttons & 0x80000000)
		{
			usercommand->Buttons &= 0x7FFFFFFFu;
		}
	}
}

VOID Legitbot(pusercmd_s_BO2 usercommand, pusercmd_s_BO2 nextusercommand)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, LegitAngles;
	BOOL DelayDone = FALSE;
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;
	XINPUT_STATE State;

	// Get our best target
	DWORD BestTarget = FindBestTarget(FALSE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Use Right Ankle for shield users
	if (Entities_BO2[BestTarget].WeaponIndex == CG_GetWeaponIndexForName_BO2("riotshield_mp") && GetMenuOptionValue(MENU_LEGITBOT, OPTION_TARGETSHIELDS))
		AimTag = "j_ankle_ri";

	// Get the best targets posistion
	GetTagPosistion(&Entities_BO2[BestTarget], BestTarget, GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE), (float*)&EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO2->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Get our smooth angles
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) != "Off")
		LegitAngles = SmoothAimbotAngles(FinalAngles, FindSmoothScale());
	else
		LegitAngles = FinalAngles;

	// Apply our smooth angles if we are aimed in
	DWORD ReactionDelay = FindReactionDelayAmount();
	PCHAR AimKeyVal = GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMKEY);
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_REACTIONDELAY) != "Off")
	{
		// If we want a reaction delay
		if ((GReactionDelay > ReactionDelay) && cg_BO2->ZoomState >= 0.3f)
		{
			// Set our angles
			if (AimKeyVal == "None" ||
				AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
				AimKeyVal == "RT" && (usercommand->Buttons & 0x80000000) ||
				AimKeyVal == "LT & RT" && (usercommand->Buttons & 0x80000000 && State.Gamepad.bLeftTrigger > 35) ||
				AimKeyVal == "DPAD UP" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ||
				AimKeyVal == "DPAD RIGHT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ||
				AimKeyVal == "DPAD DOWN" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ||
				AimKeyVal == "DPAD LEFT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ||
				AimKeyVal == "A" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_A) ||
				AimKeyVal == "B" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_B) ||
				AimKeyVal == "X" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_X) ||
				AimKeyVal == "Y" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ||
				AimKeyVal == "RB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
				AimKeyVal == "LB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ||
				AimKeyVal == "BACK" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ||
				AimKeyVal == "RS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ||
				AimKeyVal == "LS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
			{
				LocalClient_BO2->ViewAngles[0] = LegitAngles.x;
				LocalClient_BO2->ViewAngles[1] = LegitAngles.y;
			}
		}
		else
		{
			GReactionDelay++;
			return;
		}
	}
	else
	{
		// Set our angles
		if (AimKeyVal == "None" ||
			AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "RT" && (usercommand->Buttons & 0x80000000) ||
			AimKeyVal == "LT & RT" && (usercommand->Buttons & 0x80000000 && State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "DPAD UP" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ||
			AimKeyVal == "DPAD RIGHT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ||
			AimKeyVal == "DPAD DOWN" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ||
			AimKeyVal == "DPAD LEFT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ||
			AimKeyVal == "A" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_A) ||
			AimKeyVal == "B" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_B) ||
			AimKeyVal == "X" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_X) ||
			AimKeyVal == "Y" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ||
			AimKeyVal == "RB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
			AimKeyVal == "LB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ||
			AimKeyVal == "BACK" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ||
			AimKeyVal == "RS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ||
			AimKeyVal == "LS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
		{
			LocalClient_BO2->ViewAngles[0] = LegitAngles.x;
			LocalClient_BO2->ViewAngles[1] = LegitAngles.y;
		}
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT) || ((GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT) && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		if (cg_BO2->ZoomState >= 0.5f)
		{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			if (IsWeaponDualWield())
				usercommand->Buttons |= 0x00100280;
		}
	}

	// Hit shots without autoshoot
	if (!GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT) && (State.Gamepad.bRightTrigger > 35) && (cg_BO2->ZoomState >= 0.3f))
	{
		nextusercommand->Buttons |= 0x80000000;
		if (usercommand->Buttons & 0x80000000)
		{
			usercommand->Buttons &= 0x7FFFFFFFu;
		}
	}
}

VOID Spinbot(usercmd_s_BO2 *UserCommand)
{
	int RandomAdd = rand() % 71;
	int RandomUpDownSpeed = rand() % 71;
	int RandomLeft = rand() % 360;
	int RandomRight = rand() % -360;
	int cg = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	int playerStatePtr = cg + 0x480A8;
	//Vector3 RefDefAngles = GetRefDefViewAngles();

	// Y Right
	SpinbotSpin = SpinbotSpin - FindYSpinSpeed();
	if (SpinbotSpin < -360)
		SpinbotSpin = 0;

	// Y Left
	SpinbotSpinFlip = SpinbotSpinFlip + FindYSpinSpeed();
	if (SpinbotSpinFlip > 360)
		SpinbotSpinFlip = 0;

	// Z Left
	SpinbotSpinZ = SpinbotSpinZ - FindZSpinSpeed();
	if (SpinbotSpinZ < -360)
		SpinbotSpinZ = 0;

	// Z Right
	SpinbotSpinZR = SpinbotSpinZR + FindZSpinSpeed();
	if (SpinbotSpinZR > 360)
		SpinbotSpinZR = 0;

	// If we aren't prone, not mantling, not dead, not climbing a ladder, not pressing LB or RB
	if ((*(BYTE*)((DWORD)cg_BO2 + 0x18F) & 0x8) || (cg_BO2->Health <= 0) || UserCommand->weaponNum == 0 || UserCommand->Buttons & 0x00010000 || UserCommand->Buttons & 0x00020000)
		return;

	Vector3 MyPosAA;
	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 MyPos; //= *(Vector3*)GetViewOriginPointer();
	Vector3 EnemyPos, Angles, VectorAngles, FinalAngles;

	CG_GetPlayerViewOrigin_BO2(0, playerStatePtr, MyPos);
	GetTagPosistion(&Entities_BO2[BestAAClient], BestAAClient, "j_mainroot", (float*)&EnemyPos);

	// Calculate aimbot angles
	VectorSubtract(EnemyPos, MyPos, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO2->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	int AAAngleX = UserCommand->ViewAngles[0];
	int AAAngleY = UserCommand->ViewAngles[1];

	// Anti-Aim X Look down
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_BO2->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO2->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-45 - LocalClient_BO2->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}

	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Fake Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		if (flip)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_BO2->SpawnAngles.x);
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO2->SpawnAngles.x);
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO2->SpawnAngles.x);

		// If we took damage or they are wallbangable / visible
		if ((cg_BO2->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->ViewAngles[0] = ANGLE2SHORT(rand() % 65 - LocalClient_BO2->SpawnAngles.x);
			SpinbotFlip = !SpinbotFlip;
		}

		// Update our health
		OurHealth = cg_BO2->Health;

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (SpinbotFlip)
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(InverseNorm(FinalAngles.y - 180.0f + RandomLeft));
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(InverseNorm(FinalAngles.y - 180.0f - RandomRight));
			SpinbotFlip = TRUE;
		}

	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Face Away")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (SpinbotFlip)
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(EnemyPos.y - 180.0f + rand() % 30);
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(EnemyPos.y - 180.0f - rand() % 30);
			SpinbotFlip = TRUE;
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Jitter")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (BestAAClient != -1)
		{
			if (flip)
				AAAngleY -= ANGLE2SHORT(270.0f);

			else
				AAAngleY -= ANGLE2SHORT(90.0f);
		}
		else
		{
			if (flip)
				AAAngleY = ANGLE2SHORT(270.0f);

			else
				AAAngleY = ANGLE2SHORT(90.0f);
		}

		UserCommand->ViewAngles[1] = AAAngleY;
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Jitter Synced")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (LocalClient_BO2->CommandNumber % 3)
			UserCommand->ViewAngles[1] = UserCommand->ViewAngles[1] + ANGLE2SHORT(200.0);
		else
		{
			if (flip)
			{
				if (flip2)
					UserCommand->ViewAngles[1] += ANGLE2SHORT(90.0f);

				else
					UserCommand->ViewAngles[1] -= ANGLE2SHORT(90.0f);
			}
			else
			{
				UserCommand->ViewAngles[1] += ANGLE2SHORT(180.0f);
			}
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Sideways")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 100.0f);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		// If we took damage or they are wallbangable / visible
		if ((cg_BO2->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->ViewAngles[1] += ANGLE2SHORT(rand() % 360);
			SpinbotFlip = !SpinbotFlip;
		}
		else
		{
			if (SpinbotFlip)
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + RandomLeft);
				SpinbotFlip = FALSE;
			}
			else
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - RandomLeft);
				SpinbotFlip = TRUE;
			}
		}

		// Update our health
		OurHealth = cg_BO2->Health;
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Angel Backwards")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		//int originalAngle = UserCommand->ViewAngles[1];
		UserCommand->ViewAngles[1] = (FinalAngles.y - 180.0f) + ANGLE2SHORT(36000180.0);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Static")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (flip)
		{
			if (flip2)
				UserCommand->ViewAngles[1] += ANGLE2SHORT(90.0f);

			else
				UserCommand->ViewAngles[1] -= ANGLE2SHORT(90.0f);
		}
		else
		{
			UserCommand->ViewAngles[1] += ANGLE2SHORT(180.0f);
		}
	}

	// Anti-Aim X Look Up (Riot Shield)
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		UserCommand->ViewAngles[0] = ANGLE2SHORT(-15 - LocalClient_BO2->SpawnAngles.x);
		//UserCommand->ViewAngles[0] = ANGLE2SHORT(-62.99 - LocalClient_BO2->SpawnAngles.x);
	}

	// Anti-Aim Y 180 Flip From Enemy (Riot Shield)
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[1] = ANGLE2SHORT(InverseNorm(AAX - 175.0f));
		UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f);
	}

	// X Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_BO2->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-62.99 - LocalClient_BO2->SpawnAngles.x);
	}

	// Y Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Left")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpinFlip);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Right")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpin);
	}
}

int CL_SendCmdHook(int r3)
{
	LocalClient_BO2 = (pclientActive_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu18
	Entities_BO2 = (pcentity_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu18
	cg_BO2 = (pcg_s_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu18
	cgs_BO2 = (pcgs_t_BO2)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu18
	XINPUT_STATE State;

	// Update our input state
	if (!menuReady)
		goto Return;

	if (UserInGame() && (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT) || GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT)))
	{
		// Do our server time 
		pusercmd_s_BO2 OldUserCommand = LocalClient_BO2->GetUserCommand(LocalClient_BO2->GetUserCommandNumber() - 1);
		pusercmd_s_BO2 CurrentUserCommand = LocalClient_BO2->GetUserCommand(LocalClient_BO2->GetUserCommandNumber());
		pusercmd_s_BO2 NextUserCommand = LocalClient_BO2->GetUserCommand(LocalClient_BO2->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		CurrentUserCommand->serverTime -= 1;
		LocalClient_BO2->CommandNumber += 1;

		// Save our angle for movement fix
		float MovementSave = SHORT2ANGLE(CurrentUserCommand->ViewAngles[1]);

		// Do our spinbot
		if (GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT))
			Spinbot(NextUserCommand);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_BO2->MaxClients; Target++)
			{
				DropTarget(PlayerInfo[Target]);
			}

			DroppedAllTargets = TRUE;
		}

		// If Ragebot aimbot mode
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
			Ragebot(CurrentUserCommand, OldUserCommand, NextUserCommand, MovementSave);

		// If Legitbot aimbot mode
		if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
			Legitbot(CurrentUserCommand, NextUserCommand);

		// Rapid Fire
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE))
		{
			if (FAILED(XInputGetState(0, &State)))
				goto Return;

			if (TriggerValue > 25)
			{
				if (RapidFireCtr > 2)
					RapidFireCtr = 0;

				if (RapidFireCtr == 0)
					CurrentUserCommand->Buttons |= 0x80000000;
			}

			RapidFireCtr++;
		}

		// Handle fake lag
		if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) != "Off")
		{
			FakeLagCount++;
			if (FakeLagCount >= PacketSkipAmount || (CurrentUserCommand->Buttons != 0) || (OldUserCommand->Buttons != 0) || (NextUserCommand->Buttons != 0))
			{
				PacketSkipAmount = FindFakeLagAmount();
				FakeLagCount = 0;
			}
		}
		else
		{
			FakeLagCount = 0;
		}

	}
	else
	{
		ShouldSendPacket = TRUE;
		DroppedAllTargets = FALSE;
		FakeLagCount = 0;
	}

Return:
	{
		if (FakeLagCount == 0)
			return CL_SendCmdDetour->CallOriginal(r3);
		else return FALSE;
	}
}

void PlayLocalSoundAliasByNameHook(char *aliasname, int a0, float soundLevel, int r6, int r7, int r8, int r9) {
	PlayLocalSoundAliasByNameDetour->CallOriginal(aliasname, a0, soundLevel, r6, r7, r8, r9);
}

void EngineDrawRadarHook(DWORD localClient, DWORD mapType, DWORD size, DWORD rectDef, DWORD shader, const float *color, DWORD unk1, DWORD unk2, DWORD unk3, float f1, float f2, float f3, float f4, float f5, float f6)
{
	bool inGame = UserInGame();
	if (inGame && mapType == 0 && GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR))
	{
		LastCompassType = mapType;
		DrawCompassTimer = GetTickCount() + 30;

		*(DWORD*)0x823E0F08 = 0x60000000; // DrawFriendlies
		*(DWORD*)0x823E0F24 = 0x60000000; // DrawEnemies
		*(DWORD*)0x823E0FC0 = 0x60000000; // DrawPlayer
	}
	else
	{
		*(DWORD*)0x823E0F08 = 0x4BFFC0D1; // DrawFriendlies
		*(DWORD*)0x823E0F24 = 0x4BFFC95D; // DrawEnemies
		*(DWORD*)0x823E0FC0 = 0x4BFFE609; // DrawPlayer
	}

	EngineDrawRadarDetour->CallOriginal(localClient, mapType, size, rectDef, shader, color, unk1, unk2, unk3, f1, f2, f3, f4, f5, f6);
}

void XMemCpyHook(void *Dst, const void *Source, int Size)
{
	DWORD rAddr;
	__asm mflr rAddr
	if (rAddr == 0x82717C8C && !UserInGame())
		return;


	if (Size >= INT_MAX || Size <= 0)
		return;
	
	return XMemCpyDetour->CallOriginal(Dst, Source, Size);
}

void DynEntCl_DestroyEvent_Hook(int localClientNum, unsigned __int16 dynEntId, int drawType, Vector3& hitPos, Vector3& hitDir) {
	if (UserInGame()) {
		int** someShitBoi = (int**)0x83798218;
		if (!someShitBoi[drawType] || !someShitBoi[drawType][dynEntId])
			return;
	}

	DynEntCl_DestroyEventDetour->CallOriginal(localClientNum, dynEntId, drawType, hitPos, hitDir);
}

DWORD sub_821D8A18Hook(int localclientnum, char* msg, const char* idklol, int* idklol2) {
	if (UserInGame()) {
		if (strstr(msg, "^HO") || strstr(msg, "^I") || strstr(msg, "^i") || strstr(msg, "^H"))
			msg[0] = '\0';
	}

	return sub_821D8A18Detour->CallOriginal(localclientnum, msg, idklol, idklol2);
}

DWORD PreGameAntiFreezeHook(const char** r3, DWORD* r4, DWORD r5, DWORD r6, DWORD r7, DWORD r8) {
	int NewR8;

	if (r8 == 0)
		return PreGameAntiFreezeDetour->CallOriginal(r3, r4, r5, r6, r7, (DWORD)&NewR8);

	return PreGameAntiFreezeDetour->CallOriginal(r3, r4, r5, r6, r7, r8);
}

DWORD GameMessageOneHook(DWORD localclient, DWORD Message, DWORD duration) {
	if (Message != 0) {
		const char* boldMessage = (const char *)Message;

		if (strstr(boldMessage, "^H"))
		{
			return 0;
		}
	}
	return GameMessageOneDetour->CallOriginal(localclient, Message, duration);
}

DWORD GameMessageTwoHook(DWORD localclient, DWORD Message, DWORD duration)
{
	if (Message != 0)
	{
		const char* boldMessage = (const char *)Message;
		//printf("GameMessageTwo: %s \n", boldMessage);

		//RemoveBadChars((PCHAR)boldMessage);

		if (strstr(boldMessage, "^H"))
		{
			return 0;
		}
	}
	return GameMessageTwoDetour->CallOriginal(localclient, Message, duration);
}

void SetupBO2Offsets()
{
	va_BO2 = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))getCryptedCoD(CheatPatches->CoDPatches.VA);//tu18
	CG_LocationalTraceBO2 = (void(_cdecl*)(void *, Vector3 *, Vector3*, int, int, int, int))getCryptedCoD(CheatPatches->CoDPatches.CG_LocationalTrace); //tu18
	DB_FindXAssetHeader_BO2 = (DWORD(__cdecl *)(XAssetType_BO2, CONST PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DB_FindXAssetHeader);//tu18
	R_AddCmdDrawText = (void(_cdecl*)(char*, int, int, float, float, float, float, float, Vector4&, int))0x828B8BA0;
	R_TextWidth = (DWORD(__cdecl *)(int, CONST PCHAR, DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_TextWidth);//tu18
	CL_DrawTextPhysical = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CL_DrawTextPhysical);//tu18
	R_AddCmdDrawStretchPic = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_AddCmdDrawStretchPic);//tu18
	CG_DrawRotatedPicPhysical = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical);//tu18
	BG_GetWeaponDef_BO2 = (DWORD(__cdecl *)(DWORD))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef);//tu18
	Cbuf_AddText_BO2 = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.Cbuf_AddText);//tu18
	CL_AddReliableCommand = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.CL_AddReliableCommand);//tu18
	SL_GetString_BO2 = (short(__cdecl*)(PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.SL_GetString);//tu18
	Com_GetClientDObj_BO2 = (DWORD(__cdecl *)(DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.Com_GetClientDObj);//tu18
	CG_GetWeaponIndexForName_BO2 = (DWORD(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.CG_GetWeaponIndexForName);//tu18
	PlayLocalSoundAliasByName = (void(__cdecl*)(char *, int, float, int, int, int, int))getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName);//tu18
	R_NormalizedTextScale_BO2 = (int(__cdecl*)(int, float))getCryptedCoD(CheatPatches->CoDPatches.R_NormalizedTextScale);//tu18
	CG_CalcNamePosition = (bool(__cdecl*)(int, Vector3*, float *, float *))getCryptedCoD(CheatPatches->CoDPatches.CG_CalcNamePos);//tu18
	CL_SetStance = (VOID(__cdecl*)(int localClientNum, int stance))getCryptedCoD(CheatPatches->CoDPatches.CL_SetStace);
	G_GetWeaponHitLocationMultiplier = (float(__cdecl*)(int, int))getCryptedCoD(CheatPatches->CoDPatches.G_GetWeaponHitLocationMultiplier);//TU18
	AnglesToAxis = (int(__cdecl*)(float*, float*))getCryptedCoD(CheatPatches->CoDPatches.AnglesToAxis);//TU18 

	// D3D Functions
	D3DDevice_SetRenderState_BO2 = (void(__cdecl*)(D3DDevice *, _D3DRENDERSTATETYPE, unsigned int))0x823873C0;//TU8 // 823992C8
	D3DDevice_SetTexture_BO2 = (void(__cdecl*)(D3DDevice *, unsigned int, D3DBaseTexture*, unsigned __int64))0x820B13D8;//TU8
	D3DDevice_DrawIndexedVertices_BO2 = (VOID(__cdecl*)(D3DDevice*, DWORD, DWORD, DWORD, DWORD))0x820AF1D0;//TU8

	// No Spread Functions
	CG_GetPlayerViewOrigin_BO2 = (bool(__cdecl*)(int, int, Vector3&))getCryptedCoD(CheatPatches->CoDPatches.CG_GetPlayerViewOrigin);
	BG_GetSpreadForWeapon_BO2 = (VOID(__cdecl*)(int, int, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.BG_GetSpreadForWeapon);
	TransformSeed_BO2 = (DWORD(__cdecl*)(int*))getCryptedCoD(CheatPatches->CoDPatches.TransformSeed);
	RandomBulletDir_BO2 = (float(__cdecl*)(int*))getCryptedCoD(CheatPatches->CoDPatches.RandomBulletDir);
	sub_82697FC0 = (VOID(__cdecl*)(int*))getCryptedCoD(CheatPatches->CoDPatches.sub_82697FC0);

	// Radar Functions
	DrawEngineRadar_BO2 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))0x823DAA88;//tu18
	DrawEnemies_BO2 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))0x823DD880;//tu18
	DrawFriendlies_BO2 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))0x823DCFD8;//tu18
	DrawVehicles_BO2 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))0x823DE768;//tu18
	DrawPlayer_BO2 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))0x823DF5C8;//tu18
	CG_DrawRotatedPic_BO2 = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))0x821C8030; //tu18
	AngleNormalize360_BO2 = (float(__cdecl*)(float))getCryptedCoD(CheatPatches->CoDPatches.AngleNormalize360);//tu18

	// AutoWall Functions
	FireBulletPenetrate_BO2 = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, pcentity_s_BO2, Vector3&, DWORD, bool, BulletTraceResults*))getCryptedCoD(CheatPatches->CoDPatches.FireBulletPenetrate);//TU18
}

HRESULT GenerateTexture(D3DDevice* Device, D3DTexture** Texture, DWORD Color) {
	if (FAILED(Device->CreateTexture(4, 4, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, Texture, NULL)))
		return E_FAIL;

	WORD Color16 = ((WORD)((Color >> 28) & 0xF) << 12) | (WORD)(((Color >> 20) & 0xF) << 8) | (WORD)(((Color >> 12) & 0xF) << 4) | (WORD)(((Color >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*Texture)->LockRect(0, &d3dlr, 0, 0);

	WORD* Dst16 = (WORD*)d3dlr.pBits;

	for (INT xy = 0; xy < 8 * 8; xy++)
		*Dst16++ = Color16;

	(*Texture)->UnlockRect(0);

	return S_OK;
}

void D3DDevice_DrawIndexedVerticesBO2(D3DDevice* pDevice, D3DPRIMITIVETYPE primitiveType, INT baseVertexIndex, UINT startIndex, UINT indexCount) {
	return ((void(*)(...))0x8216B1D0)(pDevice, primitiveType, baseVertexIndex, startIndex, indexCount);
}

D3DTexture* RedTexture = NULL;

void R_DrawIndexedPrimitive(int IDK, D3DDevice* pDevice, INT Args) {
	D3DDevice* Device = *(D3DDevice**)(pDevice);
	UINT startIndex = *(int*)(Args + 0x8);
	UINT indexCount = (*(int*)(Args + 0x4)) + ((*(int*)(Args + 0x4)) << 1);

	GenerateTexture(pDevice, &RedTexture, D3DCOLOR_RGBA(255, 0, 0, 255));

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDevice->SetTexture(0, RedTexture);

	return D3DDevice_DrawIndexedVerticesBO2(Device, D3DPT_TRIANGLELIST, 0, startIndex, indexCount);
}

void InitializeBO2Hooks()
{
	SetupBO2Offsets();
	
	Menu_PaintAllDetour = new Detour<void>;
	XInputGetStateDetour = new Detour<DWORD>;
	CL_SendCmdDetour = new Detour<int>;
	CL_CreateNewCommandsDetour = new Detour<void>;
	PlayLocalSoundAliasByNameDetour = new Detour<void>;
	R_EndFrameDetour = new Detour<void>;
	NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;
	CG_EntityEventDetour = new Detour<void>;
	R_DrawXModelSkinnedCachedDetour = new Detour<void>;
	DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	EngineDrawRadarDetour = new Detour<void>;
	PreGameAntiFreezeDetour = new Detour<DWORD>;
	GameMessageOneDetour = new Detour<DWORD>;
	GameMessageTwoDetour = new Detour<DWORD>;
	ParseKillFeedTextDetour = new Detour<DWORD>;
	sub_821D8A18Detour = new Detour<DWORD>;
	DynEntCl_DestroyEventDetour = new Detour<void>;
	sub_82267520Detour = new Detour<DWORD>;
	XMemCpyDetour = new Detour<void>;

	Menu_PaintAllDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.Menu_PaintAll), (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.XInputGetState), (PVOID)XInputGetStateHook);
	CL_SendCmdDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CL_SendCmd), (PVOID)CL_SendCmdHook);

	R_EndFrameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.R_EndFrame), (PVOID)R_EndFrameHook);
	CG_EntityEventDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_EntityEvent), (PVOID)CG_EntityEventHook);
	EngineDrawRadarDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_CompassDrawPlayerMap), (PVOID)EngineDrawRadarHook);
	PreGameAntiFreezeDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.PreGameAntiFreezeDetour), (PVOID)PreGameAntiFreezeHook);
	GameMessageOneDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.GameMessage1), (PVOID)GameMessageOneHook);
	GameMessageTwoDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.GameMessage2), (PVOID)GameMessageTwoHook);
	sub_821D8A18Detour->SetupDetour(0x821D8A18, (PVOID)sub_821D8A18Hook);
	DynEntCl_DestroyEventDetour->SetupDetour(0x822DDE20, (PVOID)DynEntCl_DestroyEvent_Hook);
	XMemCpyDetour->SetupDetour(0x827D33F0, (PVOID)XMemCpyHook);

	

	R_DrawIndexedPrimitiveDetour->SetupDetour(0x82903D28, R_DrawIndexedPrimitive);

	//0x82903D28

	// Disable probations
	*(int*)getCryptedCoD(CheatPatches->CoDPatches.DisableProbation1) = 0x60000000;
	*(int*)getCryptedCoD(CheatPatches->CoDPatches.DisableProbation2) = 0x4800007C;

	// Patch the anti-rapid fire (Thanks James)
	*(short*)getCryptedCoD(CheatPatches->CoDPatches.AntiRapidFire) = 0x4800;

	// Disable Cheat protection
	*(int*)getCryptedCoD(CheatPatches->CoDPatches.CheatProtection) = 0x60000000;

	// Unlimited Class Items
	*(int*)getCryptedCoD(CheatPatches->CoDPatches.UnlimitedClassItems) = 0x3B40CBE7;

	// Remove the redbox perk check & respawn delay check
	*(int*)0x821F608C = 0x60000000;
	*(int*)0x821F609C = 0x60000000;

	// Patch GetTagPos so it doesn't throw errors
	*(int*)0x8219F3CC = 0x60000000;
	*(int*)0x821A1C04 = 0x60000000;

	// Disable the ability to be EMP'd
	*(int*)0x82281D94 = 0x38600000; // HUD
	*(int*)0x821BFF20 = 0x38600000; // Screen Effects
	*(int*)0x821D2868 = 0x38600000; // Killfeed
}

void UninitializeBO2Cheater()
{
	//Detours.unload();
	Menu_PaintAllDetour->TakeDownDetour();
	XInputGetStateDetour->TakeDownDetour();
	CL_SendCmdDetour->TakeDownDetour();
	CL_CreateNewCommandsDetour->TakeDownDetour();
	PlayLocalSoundAliasByNameDetour->TakeDownDetour();
	R_EndFrameDetour->TakeDownDetour();
	CG_EntityEventDetour->TakeDownDetour();
	R_DrawXModelSkinnedCachedDetour->TakeDownDetour();
	DrawIndexedPrimitiveDetour->TakeDownDetour();
	R_DrawIndexedPrimitiveDetour->TakeDownDetour();
	EngineDrawRadarDetour->TakeDownDetour();
	PreGameAntiFreezeDetour->TakeDownDetour();
	GameMessageOneDetour->TakeDownDetour();
	GameMessageTwoDetour->TakeDownDetour();
	ParseKillFeedTextDetour->TakeDownDetour();
	sub_821D8A18Detour->TakeDownDetour();
	DynEntCl_DestroyEventDetour->TakeDownDetour();
	NetDll_XNetGetTitleXnAddrDetour->TakeDownDetour();
	sub_82267520Detour->TakeDownDetour();
	XMemCpyDetour->TakeDownDetour();

	delete Menu_PaintAllDetour;
	delete XInputGetStateDetour;
	delete CL_SendCmdDetour;
	delete CL_CreateNewCommandsDetour;
	delete PlayLocalSoundAliasByNameDetour;
	delete R_EndFrameDetour;
	delete CG_EntityEventDetour;
	delete R_DrawXModelSkinnedCachedDetour;
	delete DrawIndexedPrimitiveDetour;
	delete EngineDrawRadarDetour;
	delete PreGameAntiFreezeDetour;
	delete GameMessageOneDetour;
	delete GameMessageTwoDetour;
	delete ParseKillFeedTextDetour;
	delete sub_821D8A18Detour;
	delete DynEntCl_DestroyEventDetour;
	delete NetDll_XNetGetTitleXnAddrDetour;
	delete sub_82267520Detour;
	delete XMemCpyDetour;

	Sleep(1000);
	
}