#include "stdafx.h"

DWORD XeLiveTabScene::OnEnterTab(BOOL& bHandled) {
	InitializeChildren();

	btn1.SetText(L"XeMenu");
	btn2.SetText(L"Reboot Console");
	return ERROR_SUCCESS;
}

DWORD XeLiveTabScene::OnInit(XUIMessageInit* pInitData, BOOL& bHandled) {
	InitializeChildren();
	return ERROR_SUCCESS;
}

DWORD XeLiveTabScene::OnPress(HXUIOBJ hObjPressed, BOOL& bHandled) {
	if (hObjPressed) {
		if (hObjPressed == btn1) {
			HXUIOBJ hScene;
			Native::XuiSceneCreate(Utils::vaw("section://%08X,XeLive#", XeLive.hMyModule), Utils::vaw("MainScene.xur"), 0, &hScene);

			XuiSceneNavigateForward(hScene, TRUE, pThisGuide, XUSER_INDEX_ANY);
			Notify("XeLive - XeMenu Opened!").Message();
		}
		if (hObjPressed == btn2) {
			Native::HalReturnToFirmware(HalFatalErrorRebootRoutine);
		}
	}

	return ERROR_SUCCESS;
}

DWORD XeLiveTabScene::InitializeChildren() {
	GetChildById(L"btn_1", &btn1);
	GetChildById(L"btn_2", &btn2);

	if (XeLive.bAccountBanned) {
		btn1.SetEnable(FALSE);
		btn2.SetEnable(FALSE);
	}

	this->SetCurSel(1);

	return ERROR_SUCCESS;
}