#pragma once

struct NotifyThreadArgs {
	int iType;

	wchar_t* pNotifyText;
	_XNotifyUITypes Type;
	_XNotifyUIPriorities Priority;

	wchar_t* pKeyboardTitle;
	wchar_t* pKeyboardBody;
	int iKeyboardMaxLen;
	function<void(const char*)> KeyboardCallback;

	LPCWSTR* ppMessageBoxButtons;
	wchar_t* pMessageBoxTitle;
	int iMessageBoxButtonCount;
	function<void(int)> MessageBoxCallback;

	NotifyThreadArgs() {
		iType = 0;
		pNotifyText = nullptr;
		pKeyboardTitle = nullptr;
		pKeyboardBody = nullptr;
		ppMessageBoxButtons = nullptr;
		pMessageBoxTitle = nullptr;
	}
};

void XNotifyThread(NotifyThreadArgs* args);

class Notify {
public:
	Notify(const char* pMessage);
	Notify(wchar_t* pMessage);
	void Message(bool bOverride = false, _XNotifyUITypes notifyType = XNOTIFYUI_TYPE_PREFERRED_REVIEW, _XNotifyUIPriorities priority = XNOTIFYUI_PRIORITY_HIGH);
	void MessageBox(LPCWSTR* pButtons, int iButtonCnt, wchar_t* pTitle, function<void(int)> callback);
	void Keyboard(wchar_t* pBody, int iMaxLen, function<void(const char*)> callback, bool bypassQueue = false);

	static vector<NotifyThreadArgs*> Queue;
private:
	wchar_t* pMessage;
};
