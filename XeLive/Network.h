#pragma once

class Network {
public:
	static BOOL Connect();
	static VOID Disconnect();
	static BOOL Send(PVOID Buffer, DWORD Size);
	static BOOL Receive(PVOID Buffer, DWORD Size);
	static BOOL Process(PVOID Request, DWORD RequestSize, PVOID Response, DWORD ResponseSize, BOOL Close);
private:
	static sockaddr_in ServerHandle;
	static SOCKET SocketHandle;
};