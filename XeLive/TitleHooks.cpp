#include "stdafx.h"

void TitleHooks::RunOnTitleLoad(PLDR_DATA_TABLE_ENTRY moduleHandle) {
	if (moduleHandle && moduleHandle->BaseDllName.Buffer) {
		char buffer[30];
		ZeroMemory(buffer, 30);
		wcstombs(buffer, moduleHandle->BaseDllName.Buffer, sizeof(buffer));

		if (!strstr(buffer, "dash") && !strstr(buffer, "xshell")) {
			if (!Hooking::HookModuleImport(moduleHandle, MODULE_KERNEL, 407, SystemHooks::XexGetProcedureAddressHook)) {
				LOG_PRINT("Failed to hook #fa1b79f7");
			}
		}
	}

	auto mountPath = Utils::GetMountPath();
	Utils::MountPath("XELIVE:", mountPath, true);
	delete[] mountPath;

	if (moduleHandle) {
		XEX_EXECUTION_ID* executionId = (XEX_EXECUTION_ID*)RtlImageXexHeaderField(moduleHandle->XexHeaderBase, XEX_HEADER_EXECUTION_ID);
		if (!executionId) {
			LOG_PRINT("Failed to get execution id!");
			return;
		}

		if (!XeLive.bAccountBanned) {
			AntiCheat::HandleTitle(moduleHandle, executionId->TitleID);
		}

		printf("%X:%X", executionId->TitleID, moduleHandle->TimeDateStamp);

		if (!XeLive.bAccountBanned) {
			ServerXexInfo plugin;
			if (XeLive.GetPluginData(executionId->TitleID, moduleHandle->TimeDateStamp, &plugin)) {
				if (Config::Cheats[executionId->TitleID]) {
					DWORD dwAddress, dwSize;
					if (Requests::PacketDownloadPlugin(plugin.iID, &dwAddress, &dwSize)) {
						BYTE* bytes = (BYTE*)dwAddress;

						HANDLE handle;
						auto moduleLoadStatus = Native::XexLoadImageFromMemory(bytes, dwSize, "xelive_engine.xex", XEX_MODULE_FLAG_DLL, 0, &handle);

						free(bytes);

						if (moduleLoadStatus != ERROR_SUCCESS) {
							if (moduleLoadStatus == 0xC0000018) {
								Notify("XeLive - Failed to load engine, conflicting module detected!").Message();
							}

							Notify(Utils::va("XeLive - Error 0x%X occured when trying to load engine", moduleLoadStatus)).Message();
							return;
						}

						Notify("XeLive - Cheat engine loaded!").Message();
					}
					else {
						Notify("XeLive - Cheats were not loaded. Please restart game to try again.").Message();
					}
				}
			}
		}
	}
}