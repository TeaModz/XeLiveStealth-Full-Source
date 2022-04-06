#include "stdafx.h"

DWORD XeLiveMainScene::OnInit(XUIMessageInit* pInitData, BOOL& bHandled) {

    return ERROR_SUCCESS;
}

DWORD XeLiveMainScene::ButtonHandler(HXUIOBJ hObjPressed, BOOL& bHandled) {
    if (hObjPressed) {
        if (hObjPressed == ButtonConfigEditor) {
            Notify("Nigger").Message();
        }
    }
    return ERROR_SUCCESS;
}

DWORD XeLiveMainScene::InitializeChildren() {
    this->GetChildById(L"ButtonConfigEditor", &ButtonConfigEditor);
    return ERROR_SUCCESS;
}
