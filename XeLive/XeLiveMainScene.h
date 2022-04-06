#pragma once
class XeLiveMainScene : CXuiSceneImpl {
public:
	XeLiveMainScene() {}
	~XeLiveMainScene() {}

	static XeLiveMainScene& getInstance() {
		static XeLiveMainScene singleton;
		return singleton;
	}

	CXuiControl ButtonConfigEditor;

	XUI_IMPLEMENT_CLASS(XeLiveMainScene, L"XeLiveMainScene", XUI_CLASS_SCENE)
		XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_NOTIFY_PRESS(ButtonHandler)
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_ENTER_TAB(OnEnterTab)
		XUI_END_MSG_MAP()

		DWORD OnEnterTab(BOOL& bHandled);
	DWORD OnInit(XUIMessageInit* pInitData, BOOL& bHandled);
	DWORD ButtonHandler(HXUIOBJ hObjPressed, BOOL& bHandled);
	DWORD InitializeChildren();
};

