#pragma once
#include "submenu.h"

class AimbotMenu : public Submenu {
public:
	static AimbotMenu* GetInstance();
	void Init() override;
	void UpdateOnce() override;
	void Update() override;
	void FeatureUpdate() override;

	AimbotMenu() : Submenu() {}
	~AimbotMenu();
};

namespace AimbotMenuVars {
	struct Vars {
		bool bAimbot;
	};
}
