#include "stdafx.h"

Menu pMenu;

void Menu::Initialize() {
	CreateDirectoryA(GAME_PATH, NULL);

	if (!pUtils.FileExists(GAME_PATH "Gamertags.txt")) {
		ofstream file(GAME_PATH "Gamertags.txt");
		if (file.good()) {
			file << "xbLive Engine" << endl;
			file << "HvH time?" << endl;
			file.close();
		}
	}

	vCenter = Vector2<float>(1280 / 2, 720 / 2);
	iMaxOptions = 20;
	pSubmenuHandler.Init();
}

void Menu::Render() {
	if (!bOpen) return;
	pRenderer.Render();
	pSubmenuHandler.Update();
}

void Menu::ASyncUpdate() {
	if (IsOpenPressed() && !IsOpen()) {
		ToggleOpen();
	}

	MenuControls();
}

bool Menu::IsSelected(int op) {
	return iCurrentRenderingScroll == (op + 1);
}