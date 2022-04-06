#include "stdafx.h"

tXuiElementBeginRender XuiElementBeginRenderStub;

tXuiRenderInitShared tXuiRenderInitSharedStub;

HRESULT(*Dashboard::XuiElementGetId)(HXUIOBJ hObj, LPCWSTR* pszId);
HRESULT(*Dashboard::XuiElementGetChildById)(HXUIOBJ hObj, LPCWSTR szId, HXUIOBJ* phChild);
HRESULT(*Dashboard::XuiElementAddChild)(HXUIOBJ hObj, HXUIOBJ hChild);
HRESULT(*Dashboard::XuiElementGetParent)(HXUIOBJ hObj, HXUIOBJ* phParent);
HRESULT(*Dashboard::XuiFigureSetFill)(HXUIOBJ hObj, XUI_FILL_TYPE nFillType, DWORD dwFillColor, XUIGradientStop* pStops, int nNumStops, float fGradientAngle, const D3DXVECTOR2* pvScale, const D3DXVECTOR2* pvTrans);
HRESULT(*Dashboard::XuiCreateObject)(LPCWSTR szClassName, HXUIOBJ* phObj);
HRESULT(*Dashboard::XuiElementSetBounds)(HXUIOBJ hObj, float fWidth, float fHeight);
HRESULT(*Dashboard::XuiElementSetPosition)(HXUIOBJ hObj, const D3DXVECTOR3* pvPos);
HRESULT(*Dashboard::XUIElementPropVal_SetString)(XUIElementPropVal2* pThis, LPCWSTR val);
HRESULT(*Dashboard::XuiObjectGetProperty)(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, XUIElementPropVal2* pVal);
HRESULT(*Dashboard::XuiObjectSetProperty)(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, const XUIElementPropVal2* pVal);
HRESULT(*Dashboard::XuiObjectGetPropertyId)(HXUIOBJ hObj, LPCWSTR szPropName, DWORD* pdwId);
DWORD(*Dashboard::XamShowGuideUI)(DWORD arg0);
#ifdef DASH_HACKS
HRESULT(*Dashboard::XuiSceneCreate)(LPCWSTR szBasePath, LPCWSTR szScenePath, void* pvInitData, HXUIOBJ* phScene);
HRESULT(*Dashboard::XuiSendMessage)(HXUIOBJ hObj, XUIMessage* pMessage);
HRESULT(*Dashboard::XuiSceneNavigateForward)(HXUIOBJ hCur, BOOL bStayVisible, HXUIOBJ hFwd, BYTE UserIndex);
HRESULT(*Dashboard::XuiSceneNavigateFirst)(HXUIOBJ hParent, HXUIOBJ hFirst, BYTE UserIndex);
HRESULT(*Dashboard::XuiDrawShape)(HXUIDC hDC, HXUISHAPE hShape);
bool(*Dashboard::XuiHandleIsValue)(HXUIOBJ hObj);
HRESULT(*Dashboard::XuiTextElementSetText)(HXUIOBJ hObj, LPCWSTR szText);
HXUIOBJ HandleFunction;
#endif // DASH_HACKS

HRESULT Dashboard::SetFill(HXUIOBJ hObj, DWORD Color1, DWORD Color2, BOOL Gradient, XUI_FILL_TYPE Type) {
	D3DXVECTOR2 pvScalingFactor(1.0f, 1.0f), pTrnas(0.0f, 0.0f);
	XUIGradientStop gs[2];
	gs[0].dwColor = Color1;
	gs[0].fPos = 0.5f;
	gs[1].dwColor = Color2;
	gs[1].fPos = 1.0f;
	return XuiFigureSetFill(hObj, Type, Color1, gs, 8, 0.5f, &pvScalingFactor, &pTrnas);
}

HRESULT Dashboard::SetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal2* propVal) {
	HRESULT result = E_FAIL;
	DWORD propID = 0;
	if (NT_SUCCESS(result = XuiObjectGetPropertyId(Obj, szPropName, &propID)))
		result = XuiObjectSetProperty(Obj, propID, 0, propVal);
	return result;
}
HRESULT Dashboard::GetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal2* propVal) {
	HRESULT result = E_FAIL;
	DWORD propID = 0;
	if (NT_SUCCESS(result = XuiObjectGetPropertyId(Obj, szPropName, &propID)))
		result = XuiObjectGetProperty(Obj, propID, 0, propVal);
	return result;
}

HRESULT Dashboard::XuiElementSetShow(HXUIOBJ hObj, BOOL bShow) {
	XUIMessage msg;
	XUIMessageShowstate msgData;
	XuiMessageSetShowstate(&msg, &msgData, bShow);
	return XuiSendMessage(hObj, &msg);
}

/* Dashboard Bar*/
HXUIOBJ DashBanner, DashBannerChild;

DWORD PrimaryColor = 0xFF70e000;
DWORD SecondaryColor = 0xFF70e000;

HRESULT Dashboard::XuiElementBeginRenderHook(HXUIOBJ hObj, XUIMessageRender* pRenderData, XUIRenderStruct* pRenderStruct) {
	HXUIOBJ hParentObj; LPCWSTR ObjID, ParentText;
	XuiElementGetId(hObj, &ObjID);
	XuiElementGetParent(hObj, &hParentObj);
	XuiElementGetId(hParentObj, &ParentText);

	if (NT_SUCCESS(XuiElementBeginRenderStub(hObj, pRenderData, pRenderStruct))) {

		if (lstrcmpW(ObjID, L"Front") == 0) SetFill(hObj, PrimaryColor, SecondaryColor, TRUE, XUI_FILL_RADIAL_GRADIENT);

		if (lstrcmpW(ObjID, L"Background") == 0 || lstrcmpW(ObjID, L"GreenHighlight") == 0 || lstrcmpW(ObjID, L"_Background") == 0) {
			SetFill(hObj, PrimaryColor, SecondaryColor, TRUE, XUI_FILL_LINEAR_GRADIENT); // Buttons
		}
		else if (lstrcmpW(ObjID, L"figSearchBox") == 0 ||
			lstrcmpW(ParentText, L"DescriptionGrid") == 0 ||
			lstrcmpW(ParentText, L"HubSlotXzpScene") == 0 ||
			lstrcmpW(ParentText, L"Back") == 0 ||
			lstrcmpW(ParentText, L"Color") == 0 ||
			lstrcmpW(ParentText, L"_SceneHost") == 0 ||
			lstrcmpW(ObjID, L"labBackground") == 0 ||
			lstrcmpW(ObjID, L"HeaderShader") == 0 ||
			lstrcmpW(ObjID, L"gameList") == 0 ||
			lstrcmpW(ObjID, L"titleList") == 0 ||
			lstrcmpW(ObjID, L"achievementList") == 0 ||
			lstrcmpW(ObjID, L"SlotContainerButton") == 0 ||
			lstrcmpW(ObjID, L"HeaderBackground") == 0 ||
			lstrcmpW(ObjID, L"graphic_metapane") == 0 ||
			lstrcmpW(ObjID, L"HeaderShader") == 0 ||
			lstrcmpW(ObjID, L"graphic_memsilver") == 0 ||
			lstrcmpW(ObjID, L"gamercard") == 0 ||
			lstrcmpW(ObjID, L"gamercardsmbkg") == 0 ||
			lstrcmpW(ObjID, L"gamercardbg") == 0 ||
			lstrcmpW(ObjID, L"graphic_metapane") == 0 ||
			lstrcmpW(ObjID, L"headerPanel") == 0 ||
			lstrcmpW(ObjID, L"figGraphicMeta") == 0 ||
			lstrcmpW(ObjID, L"ShaderFigure") == 0 || 
			lstrcmpW(ObjID, L"baseMetaPane") == 0)
			if (true) SetFill(hObj, PrimaryColor, SecondaryColor, TRUE, XUI_FILL_SOLID);
		//ringGroup loading ring
		if (lstrcmpW(ParentText, L"Back") == 0) {
			XuiElementGetParent(hParentObj, &DashBanner);

			if (!XuiHandleIsValue(DashBannerChild)) {
				if (NT_SUCCESS(XuiSceneCreate(nullptr, Utils::vaw("section://%08X,XeLive#DashBanner.xur", XeLive.hMyModule), NULL, &DashBannerChild))) {
					XuiElementAddChild(DashBanner, DashBannerChild);
					XuiSceneNavigateFirst(DashBannerChild, hObj, XUSER_INDEX_ANY);
				}
			}
		}

		/* Dash Bar Text */
		if (lstrcmpW(ObjID, L"txtKVStats") == 0) {
			XuiTextElementSetText(hObj, Utils::vaw("KV Stats: %i day%s | Hash: %X", XeLive.iTotalDaysOnKV, XeLive.iTotalDaysOnKV == 1 ? "" : "s", Keyvault::GetKeyvaultHash()));
		}
		else if (lstrcmpW(ObjID, L"txtStatus") == 0) {
			XuiTextElementSetText(hObj, Utils::vaw("Status: %s", XeLive.bConnectedToServerInit ? "Connected to XeLive" : "Not connected to XeLive!"));
		}
		else if (lstrcmpW(ObjID, L"txtChallengeStats") == 0) {
			XuiTextElementSetText(hObj, Utils::vaw("Challenge Stats: %i total | %i on current KV", XeLive.iTotalChallengesPassed, XeLive.iTotalChallengesPassedOnThisKV));
		}

		/* Custom UI Elements*/
		if (lstrcmpW(ParentText, L"XeLive_BackgroundUnderline") == 0 || lstrcmpW(ParentText, L"XeLive_InfoBoxBorderLeft") == 0 || lstrcmpW(ParentText, L"XeLive_InfoBoxBorderBottom") == 0) {
			SetFill(hObj, PrimaryColor, SecondaryColor, TRUE, XUI_FILL_RADIAL_GRADIENT);
		}

		return ERROR_SUCCESS;
	}

	return E_FAIL;
}

void Dashboard::DashboardUI(PLDR_DATA_TABLE_ENTRY moduleHandle) {
	wchar_t szFilePath[MAX_PATH];
	wchar_t szSkinPath[MAX_PATH];

	HANDLE xuiHandle = 0;
	if (NT_SUCCESS(Native::XexLoadImage("\\SystemRoot\\huduiskin.xex", 0x8, 0, &xuiHandle))) {
		for (DWORD i = 0; i < _ARRAYSIZE(szVisuals); i++) Native::XuiFreeVisuals(szVisuals[i]);
		swprintf(szFilePath, MAX_PATH, L"section://%08X,skin#skin.xur", xuiHandle);

		if (Config::bCustomColors) {
			if (wcscmp(moduleHandle->BaseDllName.Buffer, L"dash.xex") == 0) {
				XeLive.dHandle = (HANDLE)moduleHandle;

				// Patches dash
				memset((PVOID)XeLive.Address->dwHudColor[7], 0, 0x13);

				Dashboard::Initialize();

				if (Config::bCustomColors || !XeLive.bDevkit) {
					XuiElementBeginRenderStub = (tXuiElementBeginRender)pHooking.HookFunctionStub((DWORD)Utils::ResolveFunction((HMODULE)moduleHandle, 0x28D3), XuiElementBeginRenderHook);
				}
			}
		}
		if (NT_SUCCESS(Native::XuiLoadVisualFromBinary(szFilePath, 0))) {
			Native::XuiVisualSetBasePath(L"skin://", 0);
		}

		Native::XexUnloadImage(xuiHandle);
	}

	if (Config::bCustomNotify) {
		Native::XuiFreeVisuals(L"scr_Notification");
		swprintf(szFilePath, MAX_PATH, L"section://%08X,XeLive#Notify.xur", XeLive.hMyModule);
		swprintf(szSkinPath, MAX_PATH, L"section://%08X,XeLive#Theme.xur", XeLive.hMyModule);
		Native::XuiLoadVisualFromBinary(szFilePath, 0);
		Native::XuiLoadVisualFromBinary(szSkinPath, 0);
	}
}

HRESULT Dashboard::Initialize() {
	XuiElementGetId = (HRESULT(*)(HXUIOBJ, LPCWSTR*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10176);
	XuiElementGetChildById = (HRESULT(*)(HXUIOBJ, LPCWSTR, HXUIOBJ*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10177);
	XuiElementAddChild = (HRESULT(*)(HXUIOBJ, HXUIOBJ))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10155);
	XuiElementGetParent = (HRESULT(*)(HXUIOBJ, HXUIOBJ*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10157);
	XuiFigureSetFill = (HRESULT(*)(HXUIOBJ, XUI_FILL_TYPE, DWORD, XUIGradientStop*, int, float, const D3DXVECTOR2*, const D3DXVECTOR2*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10210);
	XuiCreateObject = (HRESULT(*)(LPCWSTR, HXUIOBJ*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10028);
	XuiElementSetBounds = (HRESULT(*)(HXUIOBJ, float, float))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10165);
	XuiElementSetPosition = (HRESULT(*)(HXUIOBJ, const D3DXVECTOR3*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10167);
	XUIElementPropVal_SetString = (HRESULT(*)(XUIElementPropVal2*, LPCWSTR))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10094);
	XuiObjectGetProperty = (HRESULT(*)(HXUIOBJ, DWORD, DWORD, XUIElementPropVal2*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10047);
	XuiObjectSetProperty = (HRESULT(*)(HXUIOBJ, DWORD, DWORD, const XUIElementPropVal2*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10048);
	XuiObjectGetPropertyId = (HRESULT(*)(HXUIOBJ, LPCWSTR, DWORD*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10044);
	XamShowGuideUI = (DWORD(*)(DWORD))(DWORD)Utils::ResolveFunction((HMODULE)GetModuleHandleA("xam.xex"), 772);
	#ifdef DASH_HACKS
		XuiSceneCreate = (HRESULT(*)(LPCWSTR, LPCWSTR, void*, HXUIOBJ*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10252);
		XuiHandleIsValue = (bool(*)(HXUIOBJ))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10030);
		XuiSendMessage = (HRESULT(*)(HXUIOBJ, XUIMessage*))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10033);
		XuiSceneNavigateForward = (HRESULT(*)(HXUIOBJ, BOOL, HXUIOBJ, BYTE))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10254);
		XuiSceneNavigateFirst = (HRESULT(*)(HXUIOBJ, HXUIOBJ, BYTE))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10253);
		HandleFunction = ((HXUIOBJ(*)(...))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10681))();
		XuiDrawShape = ((HRESULT(*)(HXUIDC, HXUISHAPE))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10418));
		XuiTextElementSetText = ((HRESULT(*)(HXUIOBJ, LPCWSTR))(DWORD)Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10218));
	#endif // DASH_HACKS
	return S_OK;
}