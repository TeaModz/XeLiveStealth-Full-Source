#include "stdafx.h"

DWORD ScnGuideInfo::OnInit(XUIMessageInit* pInitData, BOOL& bHandled) {
	InitializeChildren();
	return ERROR_SUCCESS;
}

HRESULT ScnGuideInfo::OnRender(XUIMessageRender* pRenderData, BOOL& bHandled) {
	InitializeChildren();

	if (XeLive.bAccountBanned) {
		cat_top.SetText(Utils::vaw("Status:"));
		val_top.SetText(Utils::vaw("Account banned!"));

		cat_middle.SetText(Utils::vaw("Reason:"));
		val_middle.SetText(Utils::vaw("%s", XeLive.szAccountBannedMessage));
		return ERROR_SUCCESS;
	}

	if (!XeLive.bConnectedToServerInit) {
		cat_top.SetText(Utils::vaw("Status:"));
		val_top.SetText(Utils::vaw("Not connected to XeLive!"));
		return ERROR_SUCCESS;
	}

	cat_top.SetText(Utils::vaw("XeLive:"));
	val_top.SetText(Utils::vaw("Free Forever"));

	cat_middle.SetText(Utils::vaw("KV Stats:"));
	val_middle.SetText(Utils::vaw("%i day%s | Hash: %X", XeLive.iTotalDaysOnKV, XeLive.iTotalDaysOnKV == 1 ? "" : "s", Keyvault::GetKeyvaultHash()));

	cat_bottom.SetText(Utils::vaw("Challenge Stats:"));
	val_bottom.SetText(Utils::vaw("%i total | %i on current KV", XeLive.iTotalChallengesPassed, XeLive.iTotalChallengesPassedOnThisKV));

	return ERROR_SUCCESS;
}

DWORD ScnGuideInfo::InitializeChildren() {
	GetChildById(L"cat_top", &cat_top);
	GetChildById(L"cat_middle", &cat_middle);
	GetChildById(L"cat_bottom", &cat_bottom);

	GetChildById(L"val_top", &val_top);
	GetChildById(L"val_middle", &val_middle);
	GetChildById(L"val_bottom", &val_bottom);

	cat_top.SetText(L"");
	cat_middle.SetText(L"");
	cat_bottom.SetText(L"");

	val_top.SetText(L"");
	val_middle.SetText(L"");
	val_bottom.SetText(L"");
	return ERROR_SUCCESS;
}