#pragma once

struct XUIElementPropVal2 {
	XUI_ELEMENT_PROPERTY_TYPE type;
	union {
		BOOL bVal;
		int nVal;
		unsigned int uVal;
		float fVal;
		WCHAR* szVal;
		D3DVECTOR vecVal;
		XUIQuaternion quatVal;
		IXUIPropObj* pObj;
		struct {
			void* pvCustomData;
			XUICustomPropHandler* pHandler;
		} CustomVal;
	};
#ifdef __cplusplus
	XUIElementPropVal2() {
		((void(*)(...))(Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10079)))(this);
	}
	~XUIElementPropVal2() {
		((void(*)(...))(Utils::ResolveFunction((HMODULE)XeLive.dHandle, 10080)))(this);
	}
#endif
};
struct XUIElementPropDef2 {
	DWORD dwFlags;
	DWORD dwId;
	DWORD dwOffset;
	DWORD dwExtra;
	LPCWSTR szPropName;
	XUI_ELEMENT_PROPERTY_TYPE Type;
	const XUIElementPropVal* pDefaultVal;
	XUICustomPropHandler* pCustomHandler;
	PFN_GETPROP pfnGetProp;
	PFN_SETPROP pfnSetProp;
	PFN_GETPROPDEF pfnGetPropDef;
	PFN_GETCOUNT pfnGetCount;

#ifndef _XBOX
	LPCWSTR szPropertyEditor;
#endif
};
struct XUIClass2 {
	DWORD cbSize;
	LPCWSTR szClassName;
	LPCWSTR szBaseClassName;
	LPCWSTR szClassDetails;
	XUIObjectMethods Methods;
	const XUIElementPropDef* pPropDefs;
	DWORD dwPropDefCount;
	BOOL bExtensionClass;
};