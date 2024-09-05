#ifndef __MThread_h__
#define __MThread_h__
#include "process.h"

struct HostData {
	HWND hWnd;
	WSAEVENT hExitEvent;
	SOCKET hSocket;
	CEdit* pTraceError;
	CEdit* pTraceData;
	TString adr_port;
	HostData() { hSocket = INVALID_SOCKET; }
	HostData(const HostData& d) {
		*this = d;
	}
	HostData& operator = (const HostData& d) {
		hWnd = d.hWnd;
		hExitEvent = d.hExitEvent;
		hSocket = d.hSocket;
		pTraceError = d.pTraceError;
		pTraceData = d.pTraceData;
		adr_port = d.adr_port;
		return *this;
	}
	

};

#define ExitMThread() _endthreadex(1)
#define _SOC_TREAD_
struct MThread
{
public:
#ifdef _SOC_TREAD_
	MThread(bool fSockTread = true);
#else
	MThread();
#endif
	~MThread();
	bool Start(LPTHREAD_START_ROUTINE func, void* param);
	bool Stop();
	bool IsRun();
	bool IsStarted() { return m_hThread != 0; }
	DWORD Resume() { return !m_hThread ? -1 : ::ResumeThread(m_hThread); }
	DWORD Suspend() { return !m_hThread ? -1 : ::SuspendThread(m_hThread); }
	__int64 GetTime();
	BOOL SetPriorety(int nPriority);
	int GetPriorety();
	BOOL DisableChangePriorety(BOOL fDisable);
	HANDLE ExitEvent() { return m_eExitEvent; }
	operator HANDLE () { return m_hThread; }
protected:
	HANDLE m_hThread;
	HANDLE m_eExitEvent;
	UINT m_dwThreadID;
	bool m_fSockTread;

};

#endif
