#pragma once
class Dashboard {
public:
	static HRESULT Initialize();
	static void DashboardUI(PLDR_DATA_TABLE_ENTRY moduleHandle);
	static HRESULT XuiElementBeginRenderHook(HXUIOBJ hObj, XUIMessageRender* pRenderData, XUIRenderStruct* pRenderStruct);
	static HRESULT SetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal2* propVal);
	static HRESULT GetProperty(HXUIOBJ Obj, LPCWSTR szPropName, XUIElementPropVal2* propVal);
	static HRESULT XuiElementSetShow(HXUIOBJ hObj, BOOL bShow);
	static HRESULT SetFill(HXUIOBJ hObj, DWORD Color1, DWORD Color2, BOOL Gradient, XUI_FILL_TYPE Type = XUI_FILL_SOLID);

	static DWORD(*XamShowGuideUI)(DWORD arg0);
private:
	static HRESULT(*XuiElementGetId)(HXUIOBJ hObj, LPCWSTR* pszId);
	static HRESULT(*XuiElementGetChildById)(HXUIOBJ hObj, LPCWSTR szId, HXUIOBJ* phChild);
	static HRESULT(*XuiElementAddChild)(HXUIOBJ hObj, HXUIOBJ hChild);
	static HRESULT(*XuiElementGetParent)(HXUIOBJ hObj, HXUIOBJ* phParent);
	static HRESULT(*XuiFigureSetFill)(HXUIOBJ hObj, XUI_FILL_TYPE nFillType, DWORD dwFillColor, XUIGradientStop* pStops, int nNumStops, float fGradientAngle, const D3DXVECTOR2* pvScale, const D3DXVECTOR2* pvTrans);
	static HRESULT(*XuiCreateObject)(LPCWSTR szClassName, HXUIOBJ* phObj);
	static HRESULT(*XuiElementSetBounds)(HXUIOBJ hObj, float fWidth, float fHeight);
	static HRESULT(*XuiElementSetPosition)(HXUIOBJ hObj, const D3DXVECTOR3* pvPos);
	static HRESULT(*XUIElementPropVal_SetString)(XUIElementPropVal2* pThis, LPCWSTR val);
	static HRESULT(*XuiObjectGetProperty)(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, XUIElementPropVal2* pVal);
	static HRESULT(*XuiObjectSetProperty)(HXUIOBJ hObj, DWORD dwPropId, DWORD dwIndex, const XUIElementPropVal2* pVal);
	static HRESULT(*XuiObjectGetPropertyId)(HXUIOBJ hObj, LPCWSTR szPropName, DWORD* pdwId);
#ifdef DASH_HACKS
	static HRESULT(*XuiSceneCreate)(LPCWSTR szBasePath, LPCWSTR szScenePath, void* pvInitData, HXUIOBJ* phScene);
	static HRESULT(*XuiSendMessage)(HXUIOBJ hObj, XUIMessage* pMessage);
	static HRESULT(*XuiSceneNavigateForward)(HXUIOBJ hCur, BOOL bStayVisible, HXUIOBJ hFwd, BYTE UserIndex);
	static HRESULT(*XuiSceneNavigateFirst)(HXUIOBJ hParent, HXUIOBJ hFirst, BYTE UserIndex);
	static bool(*XuiHandleIsValue)(HXUIOBJ hObj);
	static HRESULT(*XuiDrawShape)(HXUIDC hDC, HXUISHAPE hShape);
	static HRESULT(*XuiTextElementSetText)(HXUIOBJ hObj, LPCWSTR szText);
#endif // DASH_HACKS

};

