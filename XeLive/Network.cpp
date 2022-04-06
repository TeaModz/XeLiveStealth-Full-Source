#include "stdafx.h"

sockaddr_in Network::ServerHandle;
SOCKET Network::SocketHandle;

BOOL Network::Connect() {
	XNetStartupParams XNSP = { 0 };
	XNSP.cfgSizeOfStruct = sizeof(XNetStartupParams);
	XNSP.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	if (Native::NetDll_XNetStartup(XNCALLER_SYSAPP, &XNSP) != 0) return FALSE;

	WSADATA WsaData = { 0 };
	if (Native::NetDll_WSAStartupEx(XNCALLER_SYSAPP, WINSOCK_VERSION, &WsaData, WINSOCK_VERSION) != 0) return FALSE;
	if ((SocketHandle = Native::NetDll_socket(XNCALLER_SYSAPP, AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) return FALSE;

	BOOL SockOpt = TRUE;
	if (Native::NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_MARKINSECURE, (CONST PCHAR) & SockOpt, sizeof(BOOL)) != 0) return FALSE;

	DWORD MaxPacketSize = 0x100000;
	if (Native::NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_SNDBUF, (CONST PCHAR) & MaxPacketSize, sizeof(DWORD)) != 0) return FALSE;
	if (Native::NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_RCVBUF, (CONST PCHAR) & MaxPacketSize, sizeof(DWORD)) != 0) return FALSE;


	ServerHandle.sin_family = AF_INET;

	ServerHandle.sin_addr.S_un.S_un_b.s_b1 = 0x69;
	ServerHandle.sin_addr.S_un.S_un_b.s_b2 = 0x69;
	ServerHandle.sin_addr.S_un.S_un_b.s_b3 = 0x69;
	ServerHandle.sin_addr.S_un.S_un_b.s_b4 = 0x69;
	ServerHandle.sin_port = htons(7554);

	bool success = true;
	for (int i = 0; i < 10; i++) {
		if (Native::NetDll_connect(XNCALLER_SYSAPP, SocketHandle, (struct sockaddr*)&ServerHandle, sizeof(sockaddr_in)) == SOCKET_ERROR) {
			success = false;
			LOG_PRINT("Failed to connect to socket, attempt %i to reconnect. Status: %X", i + 1, GetLastError());
		}
		else {
			success = true;
			break;
		}

		Native::Sleep(1000);
	}

	if (!success) {
		Native::NetDll_closesocket(XNCALLER_SYSAPP, SocketHandle);
		Notify("Failed to connect to server after 10 tries.").Message();
		return FALSE;
	}

	return TRUE;
}

VOID Network::Disconnect() {
	if ((SocketHandle != INVALID_SOCKET)) {
		Native::NetDll_closesocket(XNCALLER_SYSAPP, SocketHandle);
		SocketHandle = INVALID_SOCKET;
	}
}

BOOL Network::Send(PVOID Buffer, DWORD Size) {
	DWORD Remaining = (Size);
	PCHAR Current = (PCHAR)Buffer;
	while (Remaining > 0) {
		DWORD Result = Native::NetDll_send(XNCALLER_SYSAPP, SocketHandle, Current, min(0x2500, Remaining), 0);
		if (Result <= 0) return FALSE;
		Remaining -= Result;
		Current += Result;
	}
	return TRUE;
}

BOOL Network::Receive(PVOID Buffer, DWORD Size) {
	DWORD Received = 0;
	DWORD Remaining = Size;
	while (Remaining > 0) {
		DWORD Result = Native::NetDll_recv(XNCALLER_SYSAPP, SocketHandle, (PCHAR)Buffer + Received, min(0x2500, Remaining), 0);
		if (Result <= 0) return FALSE;
		Remaining -= Result;
		Received += Result;
	}
	return (Received == Size);
}

BOOL Network::Process(PVOID Request, DWORD RequestSize, PVOID Response, DWORD ResponseSize, BOOL Close) {
	if (!Send(Request, RequestSize)) return FALSE;
	if (!Receive(Response, ResponseSize)) return FALSE;
	if (Close) Disconnect();
	return TRUE;
}