#pragma once
#include "critical.h"

#include "resource.h"
struct MService : public SERVICE_STATUS {
	MService()
	{

		dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
		dwCurrentState = SERVICE_STOPPED;
		dwControlsAccepted = SERVICE_ACCEPT_STOP;
		dwWin32ExitCode = 0;
		dwServiceSpecificExitCode = 0;
		dwCheckPoint = 0;
		dwWaitHint = 0;
		//strcpy(m_szServiceName, "Pluk Service Launcher");


	}
	bool Install();
	bool Uninstall();
	bool Start();

	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	static void WINAPI Handler(DWORD dwOpcode);
	static void SetServiceStatus(DWORD dw);

	static TCHAR m_szServiceName[256];
	static bool fIsServName;

	static SERVICE_STATUS_HANDLE hHandler;
	static DWORD dwThreadID;

};

class AllergoscopeApp : public CWinApp
{
public:
	AllergoscopeApp();
	virtual ~AllergoscopeApp();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static MService g_Service;
	WString pathAllergoscope;
	CriticalSection waitExe;
};
