#pragma once
#include "critical.h"


class MDNSReader
{
public:
	struct SendData {
		MArray<char> rsend;
		sockaddr_in addr;
		SendData() { ZeroMemory((char*)&addr, sizeof(SOCKADDR_IN)); }

	};
	struct SockData {
		HWND hWnd;
		int message;
		HANDLE hExitEvent;
		MArray<SOCKET> hSock;
		MArray<sockaddr_in> addr;
		TString addr_srv;
		CriticalSection sec;
		MDeq<SendData> _in;
		SockData() { hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL); }
		~SockData() { CloseHandle(hExitEvent); }
	};

	MDNSReader(HWND hWnd, int message, LPCSTR address, int port);
	~MDNSReader();
	BOOL Receive(SendData &res);
	void Stop();

protected:
	static SOCKET RestartSocket(SOCKET hSock, sockaddr_in* in, LPCSTR addr_srv);
	static UINT AFX_CDECL SockFunc(void* h);
	static int Read(SOCKET fd, void* buff, int len, sockaddr_in* addr);
	bool GetNetworkIP(MArray<sockaddr_in>& address, int port);
	bool IsValid();

protected:
	CWinThread* mThread;
	SOCKADDR_IN Address;
	SockData* data;

};


