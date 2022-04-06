#include "stdafx.h"
#include "aimbotMenu.h"

using namespace AimbotMenuVars;

namespace AimbotMenuVars {
	Vars vars;

	void OnAimbot() {

	}
}


void AimbotMenu::Init() {
	SetName("Aimbot");

	addOption(ToggleOption("Enable Aimbot")
		.addToggle(vars.bAimbot)
		.addFunction(OnAimbot).canBeSaved("Aimbot")
		.addTooltip("Aimbot will only run if this is enabled."));
}

/*Update once when submenu is opened*/
void AimbotMenu::UpdateOnce() {}

/*Update while submenu is active*/
void AimbotMenu::Update() {}

/*Background update*/
void AimbotMenu::FeatureUpdate() {}

/*Singleton*/
AimbotMenu* _instance;
AimbotMenu* AimbotMenu::GetInstance() {
	if (_instance == nullptr) {
		_instance = new AimbotMenu();
		_instance->Init();
		pSubmenuHandler.AddSubmenu(_instance);
	}
	return _instance;
}
AimbotMenu::~AimbotMenu() { delete _instance; }