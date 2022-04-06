#include "stdafx.h"

map<DWORD, bool> Config::Cheats;
map<DWORD, bool> Config::Bypasses;

bool Config::bBypassAllUI;
bool Config::bCustomColors;
bool Config::bCustomDashboard;
bool Config::bCustomGuide;
bool Config::bGuideInfo;
bool Config::bCustomNotify;
bool Config::bUsingNoKV;
char Config::szXShellEmail[0xC];
char Config::szXShellPassword[0xB];

void Config::UpdateConfig() {
	IniParse parse("XELIVE:\\XeLive.ini");
	if (parse.IsGood()) {
		FILE* file = fopen("XELIVE:\\XeLive.ini", "w+");
		if (file) {
			parse.SetBool("User Interface", "Bypass all", bBypassAllUI);
			parse.SetBool("User Interface", "Custom Colors", bCustomColors);
			parse.SetBool("User Interface", "Custom Dashboard", bCustomDashboard);
			parse.SetBool("User Interface", "Custom Guide", bCustomGuide);
			parse.SetBool("User Interface", "Guide Info", bGuideInfo);
			parse.SetBool("User Interface", "Custom Notify", bCustomNotify);

			parse.SetBool("Cheats", "COD WAW", Cheats[0x4156081C]);
			parse.SetBool("Cheats", "COD MW1", Cheats[0x415607E6]);
			parse.SetBool("Cheats", "COD MW2", Cheats[0x41560817]);
			parse.SetBool("Cheats", "COD MW3", Cheats[0x415608CB]);
			parse.SetBool("Cheats", "COD BO1", Cheats[0x41560855]);
			parse.SetBool("Cheats", "COD BO2", Cheats[0x415608C3]);
			parse.SetBool("Cheats", "COD BO3", Cheats[0x4156091D]);
			parse.SetBool("Cheats", "COD AW", Cheats[0x41560914]);
			parse.SetBool("Cheats", "COD GHOST", Cheats[0x415608FC]);

			parse.SetBool("Bypasses","COD BO2", Bypasses[0x415608C3]);
			parse.SetBool("Bypasses","COD BO3", Bypasses[0x4156091D]);
			parse.SetBool("Bypasses","COD AW", Bypasses[0x41560914]);
			parse.SetBool("Bypasses","COD GHOST", Bypasses[0x415608FC]);


			parse.SetString("XDK", "XShell Account Creation Email", szXShellEmail);
			parse.SetString("XDK", "XShell Account Creation Password", szXShellPassword);

			auto sections = parse.GetSections();
			for (int i = 0; i < sections.size(); i++) {
				fprintf(file, "[%s]\n", sections[i].c_str());

				auto data = parse.GetDataFromSection(sections[i].c_str());
				for (int j = 0; j < data.size(); j++) {
					fprintf(file, "%s = %s\n", data[j].first.c_str(), data[j].second.c_str());
				}

				data.clear();

				if (i != sections.size() - 1) {
					fprintf(file, "\n");
				}
			}

			sections.clear();
		}

		parse.ItemMap.clear();

		fclose(file);
	}
}

void Config::InstallDefaultConfig() {
	IniParse parse;
	parse.SetBool("User Interface", "Bypass all", false);
	parse.SetBool("User Interface", "Custom Colors", true);
	parse.SetBool("User Interface", "Custom Dashboard", true);
	parse.SetBool("User Interface", "Custom Guide", true);
	parse.SetBool("User Interface", "Guide Info", true);
	parse.SetBool("User Interface", "Custom Notify", true);

	parse.SetBool("Cheats", "COD WAW", true);
	parse.SetBool("Cheats", "COD MW1", true);
	parse.SetBool("Cheats", "COD MW2", true);
	parse.SetBool("Cheats", "COD MW3", true);
	parse.SetBool("Cheats", "COD BO1", true);
	parse.SetBool("Cheats", "COD BO2", true);
	parse.SetBool("Cheats", "COD BO3", true);
	parse.SetBool("Cheats", "COD AW", true);
	parse.SetBool("Cheats", "COD GHOST", true);

	parse.SetBool("Bypasses", "COD BO2", true);
	parse.SetBool("Bypasses", "COD BO3", true);
	parse.SetBool("Bypasses", "COD AW", true);
	parse.SetBool("Bypasses", "COD GHOST", true);

	parse.SetString("XDK", "XShell Account Creation Email", "chammy.info");
	parse.SetString("XDK", "XShell Account Creation Password", "XeLive");

	FILE* file = fopen("XELIVE:\\XeLive.ini", "w+");
	if (file) {
		auto sections = parse.GetSections();
		for (int i = 0; i < sections.size(); i++) {
			fprintf(file, "[%s]\n", sections[i].c_str());

			auto data = parse.GetDataFromSection(sections[i].c_str());
			for (int j = 0; j < data.size(); j++) {
				fprintf(file, "%s = %s\n", data[j].first.c_str(), data[j].second.c_str());
			}

			data.clear();

			if (i != sections.size() - 1) {
				fprintf(file, "\n");
			}
		}

		sections.clear();

		fclose(file);
	}

	parse.ItemMap.clear();
}

HRESULT Config::Initialize() {

	IniParse parse("XELIVE:\\XeLive.ini");
	if (parse.IsGood()) {
		bBypassAllUI = parse.ReadBool("User Interface", "Bypass all", false);
		if (!bBypassAllUI) {
			bCustomColors = parse.ReadBool("User Interface", "Custom Colors", true);
			bCustomDashboard = parse.ReadBool("User Interface", "Custom Dashboard", true);
			bCustomGuide = parse.ReadBool("User Interface", "Custom Guide", true);
			bGuideInfo = parse.ReadBool("User Interface", "Guide Info", true);
			bCustomNotify = parse.ReadBool("User Interface", "Custom Notify", true);
		}

		Cheats[0x4156081C] = parse.ReadBool("Cheats", "COD WAW", true);
		Cheats[0x415607E6] = parse.ReadBool("Cheats", "COD MW1", true);
		Cheats[0x41560817] = parse.ReadBool("Cheats", "COD MW2", true);
		Cheats[0x415608CB] = parse.ReadBool("Cheats", "COD MW3", true);
		Cheats[0x41560855] = parse.ReadBool("Cheats", "COD BO1", true);
		Cheats[0x415608C3] = parse.ReadBool("Cheats", "COD BO2", true);
		Cheats[0x4156091D] = parse.ReadBool("Cheats", "COD BO3", true);
		Cheats[0x41560914] = parse.ReadBool("Cheats", "COD AW", true);
		Cheats[0x415608FC] = parse.ReadBool("Cheats", "COD GHOST", true);

		Bypasses[0x415608C3] = parse.ReadBool("Bypasses", "COD BO2", true);
		Bypasses[0x4156091D] = parse.ReadBool("Bypasses", "COD BO3", true);
		Bypasses[0x41560914] = parse.ReadBool("Bypasses", "COD AW", true);
		Bypasses[0x415608FC] = parse.ReadBool("Bypasses", "COD GHOST", true);

		if (!Bypasses[0x415608C3]) LOG_PRINT("Opting out of using COD BO2 bypasses!");
		if (!Bypasses[0x4156091D]) LOG_PRINT("Opting out of using COD BO3 bypasses!");
		if (!Bypasses[0x41560914]) LOG_PRINT("Opting out of using COD AW bypasses!");
		if (!Bypasses[0x415608FC]) LOG_PRINT("Opting out of using COD GHOST bypasses!");

		auto email = parse.ReadString("XDK", "XShell Account Creation Email", "chammy.info");
		auto pw = parse.ReadString("XDK", "XShell Account Creation Password", "XeLive");

		parse.ItemMap.clear();

		if (strlen(email) > 12)
			Notify("XeLive - XShell email is too long! Max chars: 12").Message();
		else strcpy(szXShellEmail, email);

		if (strlen(pw) > 11)
			Notify("XeLive - XShell password is too long! Max chars: 11").Message();
		else strcpy(szXShellPassword, pw);

		// re-cache
		UpdateConfig();
	}
	else {
		// create base file
		InstallDefaultConfig();

		// set default vars seeing how it only just now created the file
		bBypassAllUI = false;
		bCustomColors = true;
		bCustomDashboard = true;
		bCustomGuide = true;
		bGuideInfo = true;
		bCustomNotify = true;

		bUsingNoKV = false;

		Cheats[0x4156081C] = true;
		Cheats[0x415607E6] = true;
		Cheats[0x41560817] = true;
		Cheats[0x415608CB] = true;
		Cheats[0x41560855] = true;
		Cheats[0x415608C3] = true;
		Cheats[0x4156091D] = true;
		Cheats[0x41560914] = true;
		Cheats[0x415608FC] = true;
		Cheats[0x545408A7] = true;

		Bypasses[0x415608C3] = true;
		Bypasses[0x4156091D] = true;
		Bypasses[0x41560914] = true;
		Bypasses[0x415608FC] = true;
		Bypasses[0x545408A7] = true;

		strcpy(szXShellEmail, "chammy.info");
		strcpy(szXShellPassword, "XeLive");
	}

	return S_OK;
}