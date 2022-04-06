#include "stdafx.h"
#include "MainMenu.h"

SubmenuHandler pSubmenuHandler;

void SubmenuHandler::Init() {
	pMainSubmenu = pCurrentSubmenu = MainMenu::GetInstance();
}

void SubmenuHandler::Update() {
	pCurrentSubmenu->MenuUpdate();
	if (pNextSubmenu != nullptr) {
		pMenu.iCurrentOption = iNextCurrentOption;
		pMenu.iScrollOffset = iNextScrollOffset;
		pCurrentSubmenu = pNextSubmenu;
		pNextSubmenu = nullptr;
		pCurrentSubmenu->UpdateOnce();
	}
}

void SubmenuHandler::ASyncUpdate() {
	for (auto i = 0; i < pSubmenus.size(); i++) {
		pSubmenus.at(i)->FeatureUpdate();
	}
}

void SubmenuHandler::SetSubmenu(Submenu* submenu) {
	pCurrentSubmenu->iOldScrollOffset = pMenu.iScrollOffset;
	pCurrentSubmenu->iOldCurrentOption = pMenu.iCurrentOption;
	iNextCurrentOption = 0;
	iNextScrollOffset = 0;
	pNextSubmenu = submenu;
}

void SubmenuHandler::SetSubmenuMain() {
	pNextSubmenu = pMainSubmenu;
	iNextCurrentOption = 0;
	iNextScrollOffset = 0;
}

void SubmenuHandler::SetSubmenuPrevious(bool resetPos) {
	if (pCurrentSubmenu->GetParentSubmenu() == nullptr) {
		SetSubmenuMain();
		pMenu.iSelectedTab = 0;
		pMenu.iCurrentOption = 0;
		pMenu.iScrollOffset = 0;
		pMenu.iBreakScroll = 0;
		pMenu.iCurrentRenderingScroll = 0;
		pMenu.Close();
	}
	else {
		pNextSubmenu = pCurrentSubmenu->GetParentSubmenu();
		iNextCurrentOption = resetPos ? 0 : pNextSubmenu->iOldCurrentOption;
		iNextScrollOffset = resetPos ? 0 : pNextSubmenu->iOldScrollOffset;
	}
}