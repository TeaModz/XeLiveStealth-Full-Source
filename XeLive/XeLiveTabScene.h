#pragma once

class XeLiveTabScene : CXuiSceneImpl {
public:
	XeLiveTabScene() {}
	~XeLiveTabScene() {}

	static XeLiveTabScene& getInstance() {
		static XeLiveTabScene singleton;
		return singleton;
	}

	CXuiControl btn1;
	CXuiControl btn2;

	XUI_IMPLEMENT_CLASS(XeLiveTabScene, L"XeLiveTabScene", XUI_CLASS_SCENE)
		XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_NOTIFY_PRESS(OnPress)
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_ENTER_TAB(OnEnterTab)
		XUI_END_MSG_MAP()

		DWORD OnEnterTab(BOOL& bHandled);
	DWORD OnInit(XUIMessageInit* pInitData, BOOL& bHandled);
	DWORD OnPress(HXUIOBJ hObjPressed, BOOL& bHandled);
	DWORD InitializeChildren();
};