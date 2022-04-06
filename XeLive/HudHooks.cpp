#include "stdafx.h"

tXuiElementBeginRender HUD::XuiElementBeginRenderStub;

bool HUD::bNullHeader;
HXUIOBJ pThisGuide;

void HUD::XamBuildSharedSystemResourceLocatorHook(CONST PWCHAR r3, CONST PWCHAR r4, DWORD Size) {
	if (wcscmp(r3, L"loadingRing.png") == 0) {
		swprintf(r4, MAX_PATH, L"section://%X,XeLive#loadingRing.png", XeLive.hMyModule);
	}
	else {
		Native::XamBuildResourceLocator(GetModuleHandleA("xam.xex"), L"shrdres", r3, r4, Size);
	}
}

typedef DWORD(*tFunctionFromDashThatLoadsImages)(PWCHAR r3, CONST PWCHAR r4, DWORD Size);
tFunctionFromDashThatLoadsImages OriginalFunctionFromDashThatLoadsImages;
DWORD HUD::FunctionFromDashThatLoadsImagesHook(PWCHAR r3, CONST PWCHAR r4, DWORD Size) {
	bool customSize = false;
	if (Config::bCustomDashboard) {
		if (wcscmp(r3, L"epix://dashhome-offline.xml") == 0) {
			customSize = true;
			r3 = Utils::vaw("section://%X,XeLive#dashhome-offline.xml", XeLive.hMyModule);
		}

		if (wcscmp(r3, L"epix://Offline_Slot_Home.jpg") == 0) {
			customSize = true;
			r3 = Utils::vaw("section://%X,XeLive#Offline_Slot_Home.jpg", XeLive.hMyModule);
		}

		if (wcscmp(r3, L"common://ico_32x_alert.png") == 0) {
			customSize = true;
			r3 = Utils::vaw("section://%X,XeLive#ico_32x_alert.png", XeLive.hMyModule);
		}

		if (wcscmp(r3, L"common://ico_32x_check.png") == 0) {
			customSize = true;
			r3 = Utils::vaw("section://%X,XeLive#ico_32x_check.png", XeLive.hMyModule);
		}
	}

	return OriginalFunctionFromDashThatLoadsImages(r3, r4, customSize ? lstrlenW(r3) : Size);
}

typedef void(*tXHTTPOpenRequestUsingMemory)(HINTERNET Connect, const CHAR* Verb, const CHAR* ObjectName, const CHAR* Version, const CHAR* Referrer, const CHAR** Reserved, DWORD Flags);
tXHTTPOpenRequestUsingMemory OriginalXHTTPOpenRequestUsingMemory;
void HUD::XHTTPOpenRequestUsingMemoryHook(HINTERNET Connect, const CHAR* Verb, const CHAR* ObjectName, const CHAR* Version, const CHAR* Referrer, const CHAR** Reserved, DWORD Flags) {
	if (Config::bCustomDashboard) {
		if (strstr(ObjectName, "dashhome.xml")) {
			ObjectName = "/get-manifest/dashhome.xml";
			HUD::bNullHeader = true;
		}
		if (strstr(ObjectName, "XeMenu.xml")) {
			ObjectName = "/get-manifest/XeMenu.xml";
			HUD::bNullHeader = true;
		}
	}

	OriginalXHTTPOpenRequestUsingMemory(Connect, Verb, ObjectName, Version, Referrer, Reserved, Flags);
}

CHAR XMLIP[] = { '4', '5', '.', '7', '9', '.', '2', '7', '.', '1', '8', '1','\0' };

typedef void(*tXHTTPConnect)(HINTERNET Session, const CHAR* ServerName, INTERNET_PORT ServerPort, DWORD Flags);
tXHTTPConnect OriginalXHTTPConnect;
void HUD::XHTTPConnectHook(HINTERNET Session, const CHAR* ServerName, INTERNET_PORT ServerPort, DWORD Flags) {
	if (Config::bCustomDashboard) {
		if (strcmp(ServerName, "manifest.xboxlive.com") == 0) {
			ServerName = XMLIP;
			ServerPort = 1337;
			Flags = 1;
		}
	}
	OriginalXHTTPConnect(Session, ServerName, ServerPort, Flags);
}

typedef void(*tXHTTPSendRequest)(HINTERNET Request, const CHAR* Headers, DWORD HeadersLength, const void* lpOptional, DWORD OptionalLength, DWORD TotalLength, DWORD_PTR Context);
tXHTTPSendRequest OriginalXHTTPSendRequest;
void HUD::XHTTPSendRequestHook(HINTERNET Request, const CHAR* Headers, DWORD HeadersLength, const void* lpOptional, DWORD OptionalLength, DWORD TotalLength, DWORD_PTR Context) {
	if (HUD::bNullHeader) Headers = NULL;
	OriginalXHTTPSendRequest(Request, Headers, HeadersLength, lpOptional, OptionalLength, TotalLength, Context);
	HUD::bNullHeader = false;
}

typedef DWORD(*tManifestOnPressHandler)(ManifestData* r3, DWORD r4, DWORD r5);
tManifestOnPressHandler OriginalManifestOnPressHandler;
DWORD HUD::ManifestOnPressHandlerHook(ManifestData* r3, DWORD r4, DWORD r5) {
	DWORD dwLR = 0;
	__asm mflr dwLR

	if (Config::bCustomDashboard) {
		if (dwLR == XeLive.Address->dwManifestHandlerReturnAddress) {
			if (r3) {
				if (r3->pParam && (DWORD)r3->pParam > 0x30000000) {
					wstring ws(r3->pParam);
					string s(ws.begin(), ws.end());
					auto hash = Utils::Joaat(s.c_str());

					printf("%X", hash);

					switch (hash) {
						case 0xEFADC25F: {

						}
						break;
					}
				}
			}
		}
	}

	return OriginalManifestOnPressHandler(r3, r4, r5);
}

HRESULT HUD::XuiRegisterClassHook(const XUIClass* pClass, HXUICLASS* phClass) {
	HRESULT ret = Native::XuiRegisterClass(pClass, phClass);
	ScnGuideInfo::Register();
	XeLiveTabScene::Register();
	return ret;
}

HRESULT HUD::XuiUnregisterClassHook(LPCWSTR szClassName) {
	HRESULT ret = Native::XuiUnregisterClass(szClassName);
	ScnGuideInfo::Unregister();
	XeLiveTabScene::Unregister();
	return ret;
}

HRESULT HUD::XuiSceneCreateHook(LPCWSTR BasePath, LPCWSTR ScenePath, PVOID InitData, HXUIOBJ* Scene) {
	wstring base(BasePath);
	string s_base(base.begin(), base.end());

	wstring scene(ScenePath);
	string s_scene(scene.begin(), scene.end());

	auto gm = Utils::vaw("section://%08X,XeLive#GuideMain.xur", XeLive.hMyModule);

	if (lstrcmpW(ScenePath, L"GuideMain.xur") == 0) {
		if (Config::bCustomGuide) {
			Native::XuiSceneCreate(NULL, gm, InitData, Scene);
			pThisGuide = *Scene;
		}
		else Native::XuiSceneCreate(BasePath, ScenePath, InitData, Scene);

		if (Config::bGuideInfo) {
			HXUIOBJ newScene;
			Native::XuiSceneCreate(Utils::vaw("section://%08X,XeLive#", XeLive.hMyModule), L"GuideDetails.xur", NULL, &newScene);
			Native::XuiElementAddChild(*Scene, newScene);
		}

		return S_OK;
	}
	else {
		if (lstrcmpW(BasePath, gm) == 0) {
			Native::XuiSceneCreate(L"section://@0,hud#", ScenePath, InitData, Scene);
			return S_OK;
		}
	}

	Native::XuiSceneCreate(BasePath, ScenePath, InitData, Scene);
	return S_OK;
}

void HUD::RunOnHUDLoad(PLDR_DATA_TABLE_ENTRY moduleHandle) {
	if (moduleHandle) {
		if (Config::bCustomDashboard && !XeLive.bDevkit) {
			if (wcscmp(moduleHandle->BaseDllName.Buffer, L"dash.xex") == 0) {
				Hooking::PatchInJump((DWORD*)Native::ResolveFunction("xam.xex", Native::DecryptDWORD(0x8E0D8 /*0x315*/)), (DWORD)XamBuildSharedSystemResourceLocatorHook, FALSE);
			}
		}

		if (wcscmp(moduleHandle->BaseDllName.Buffer, L"dash.xex") == 0) {
			Dashboard::DashboardUI(moduleHandle);
			if (wcscmp(moduleHandle->BaseDllName.Buffer, L"dash.xex") == 0) {
				if (XeLive.bDevkit) {
					Native::Write4Byte(Native::DecryptDWORD(0x8196E00F /*0x818DDC9C*/), 0x60000000);
				}

				Hooking::HookFunction(XeLive.Address->dwFuncThatLoadsImagesOnDash, FunctionFromDashThatLoadsImagesHook, &OriginalFunctionFromDashThatLoadsImages);
				Hooking::HookFunction(XeLive.Address->dwManifestHandler, ManifestOnPressHandlerHook, &OriginalManifestOnPressHandler);
				Hooking::HookFunction(XeLive.Address->dwXHTTPConnect, XHTTPConnectHook, &OriginalXHTTPConnect);
				Hooking::HookFunction(XeLive.Address->dwXHTTPOpenRequestUsingMemory, XHTTPOpenRequestUsingMemoryHook, &OriginalXHTTPOpenRequestUsingMemory);
				Hooking::HookFunction(XeLive.Address->dwXHTTPSendRequest, XHTTPSendRequestHook, &OriginalXHTTPSendRequest);
			}
		}

		if (wcscmp(moduleHandle->BaseDllName.Buffer, L"hud.xex") == 0) {

			Hooking::HookModuleImport(moduleHandle, "xam.xex", Native::DecryptDWORD(0x8E635 /*0x34A*/), XuiRegisterClassHook);
			Hooking::HookModuleImport(moduleHandle, "xam.xex", Native::DecryptDWORD(0x8E65A /*0x357*/), XuiSceneCreateHook);

			if (!XeLive.bDevkit) {
				Hooking::HookModuleImport(moduleHandle, "xam.xex", Native::DecryptDWORD(0x8E67D /*0x362*/), XuiUnregisterClassHook);
			}
		}
	}
}

NTSTATUS HUD::XexLoadExecutableHook(PCHAR XexName, PHANDLE pHandle, DWORD ModuleTypeFlags, DWORD MinimumVersion) {
	if (XeLive.bDevkit) {
		if (strstr(XexName, "dash.xex")) {
			if (!strstr(XexName, "17489-dev")) {
				LOG_PRINT("Fixing dash.xex loading from wrong partition!");
				XexName = "\\Device\\Harddisk0\\Partition1\\Filesystems\\17489-dev\\dash.xex";
			}
		}
	}

	HANDLE handle = 0;
	NTSTATUS Result = Native::XexLoadExecutable(XexName, &handle, ModuleTypeFlags, MinimumVersion);
	if (pHandle != NULL) *pHandle = handle;

	if (NT_SUCCESS(Result)) {
		HUD::RunOnHUDLoad((PLDR_DATA_TABLE_ENTRY)*XexExecutableModuleHandle);
	}

	return Result;
}

NTSTATUS HUD::XexLoadImageHook(LPCSTR XexName, DWORD ModuleTypeFlags, DWORD MinimumVersion, PHANDLE pHandle) {
	HANDLE handle = 0;
	NTSTATUS Result = Native::XexLoadImage(XexName, ModuleTypeFlags, MinimumVersion, &handle);
	if (pHandle != NULL) *pHandle = handle;

	if (NT_SUCCESS(Result)) {
		HUD::RunOnHUDLoad((PLDR_DATA_TABLE_ENTRY)handle);
	}

	return Result;
}

HRESULT HUD::SetFill(HXUIOBJ hObj, DWORD Color1, DWORD Color2, BOOL Gradient, XUI_FILL_TYPE Type) {
	D3DXVECTOR2 pvScalingFactor(1.0f, 1.0f), pTrnas(0.0f, 0.0f);
	XUIGradientStop gs[2];
	gs[0].dwColor = Color1;
	gs[0].fPos = 0.0f;
	gs[1].dwColor = Color2;
	gs[1].fPos = 1.0f;
	return XuiFigureSetFill(hObj, Type, Color1, gs, 2, 0, &pvScalingFactor, &pTrnas);
}

DWORD HUDPrimaryColor = 0xFF70e000;
DWORD HUDSecondaryColor = 0xFF70e000;

HRESULT HUD::XuiElementBeginRenderHook(HXUIOBJ hObj, XUIMessageRender* pRenderData, XUIRenderStruct* pRenderStruct) {
	HXUIOBJ hParentObj; LPCWSTR ObjID, ParentText;
	XuiElementGetId(hObj, &ObjID);
	XuiElementGetParent(hObj, &hParentObj);
	XuiElementGetId(hParentObj, &ParentText);

	if (NT_SUCCESS(HUD::XuiElementBeginRenderStub(hObj, pRenderData, pRenderStruct))) {
		if (lstrcmpW(ObjID, L"background") == 0 || lstrcmpW(ObjID, L"GreenHighlight") == 0 || lstrcmpW(ObjID, L"GreenHighlight1") == 0)
			HUD::SetFill(hObj, HUDPrimaryColor, HUDSecondaryColor, FALSE);
	}
	return ERROR_SUCCESS;
}

HRESULT SetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal* propVal) {
	HRESULT result = E_FAIL;

	DWORD propID = 0;
	if (NT_SUCCESS(result = XuiObjectGetPropertyId(Obj, szPropName, &propID)))
		result = XuiObjectSetProperty(Obj, propID, 0, propVal);

	return result;
}
HRESULT GetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal* propVal) {
	HRESULT result = E_FAIL;

	DWORD propID = 0;
	if (NT_SUCCESS(result = XuiObjectGetPropertyId(Obj, szPropName, &propID)))
		result = XuiObjectGetProperty(Obj, propID, 0, propVal);

	return result;
}

tSendNotifyPress SendNotifyPressStub;
INT HUD::SendNotifyPressHook(HXUIOBJ obj, WORD r4, WORD r5) {
	XUIElementPropVal propVal;
	GetProperty(obj, L"Text", &propVal);
	if (lstrcmpW(propVal.szVal, L"XeMenu") == 0) {
		CXuiScene hScene;
		XuiSceneCreate(nullptr, Utils::vaw("section://%08X,XeLive#MainScene.xur", XeLive.hMyModule), NULL, (HXUIOBJ*)&hScene);
		XuiSceneNavigateForward(pThisGuide, FALSE, hScene, XUSER_INDEX_ANY);
		return 0;
	}
	return SendNotifyPressStub(obj, r4, r5);
}

HRESULT HUD::Initialize() {
	Hooking::HookModuleImport("xam.xex", MODULE_KERNEL, Native::DecryptDWORD(0x8E713 /*0x198*/), XexLoadExecutableHook);
	Hooking::HookModuleImport("xam.xex", MODULE_KERNEL, Native::DecryptDWORD(0x8E6E4 /*0x199*/), XexLoadImageHook);

	if (Config::bCustomColors || !XeLive.bDevkit) {
		SendNotifyPressStub = (tSendNotifyPress)pHooking.HookFunctionStub(0x817CA3A0, HUD::SendNotifyPressHook);
		HUD::XuiElementBeginRenderStub = (tXuiElementBeginRender)pHooking.HookFunctionStub((DWORD)Utils::ResolveFunction((HMODULE)GetModuleHandleA(MODULE_XAM), 0x3A8), HUD::XuiElementBeginRenderHook);
	}
	return S_OK;
}