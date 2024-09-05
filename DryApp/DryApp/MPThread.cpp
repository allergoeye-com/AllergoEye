#include "stdafx.h"
#include "MPThread.h"

void Idle()
{
	MSG msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)
		&& !AfxGetApp()->PreTranslateMessage(&msg))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

_int64 FileTimeTo64(FILETIME* pF)
{
	return (Int64ShllMod32(pF->dwHighDateTime, 32) | pF->dwLowDateTime);
}

//-----------------------------------------------------------
//
//----------------------------------------------------------
#ifdef _SOC_TREAD_
MThread::MThread(bool fSockTread)
{
	if (fSockTread)
#else
MThread::MThread()
{
	m_fSockTread = false;
#endif
	m_eExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

#ifdef _SOC_TREAD_
	else
	m_eExitEvent = (HANDLE)WSACreateEvent();
	m_fSockTread = fSockTread;
#endif

	m_dwThreadID = 0;
	m_hThread = 0;

}

//-----------------------------------------------------------
//
//----------------------------------------------------------
MThread::~MThread()
{
	Stop();
#ifdef _SOC_TREAD_
	if (!m_fSockTread)
#endif
		CloseHandle(m_eExitEvent);
#ifdef _SOC_TREAD_
	WSACloseEvent((WSAEVENT)m_eExitEvent);
#endif
}

//-----------------------------------------------------------
//
//----------------------------------------------------------
bool MThread::Start(LPTHREAD_START_ROUTINE func, void* param)
{
	m_hThread = (HANDLE)_beginthreadex(0, 0, (unsigned(__stdcall*) (void*))func, param, 0, &m_dwThreadID);
	DWORD dwRetCode;
	DWORD dv = GetTickCount();
	do
	{
		if (GetTickCount() - dv > 5000)
			return 0;
		Sleep(10);
		GetExitCodeThread(m_hThread, &dwRetCode);

	} while (dwRetCode != STILL_ACTIVE);
	return m_hThread != 0;
}
//-----------------------------------------------------------
//
//----------------------------------------------------------
bool MThread::IsRun()
{
	if (!m_hThread) return 0;
	DWORD dwRetCode;
	GetExitCodeThread(m_hThread, &dwRetCode);
	return dwRetCode == STILL_ACTIVE;
}

//-----------------------------------------------------------
//
//----------------------------------------------------------
bool MThread::Stop()
{
	if (!m_hThread) return 0;
	DWORD dwRetCode;
	GetExitCodeThread(m_hThread, &dwRetCode);
	if (GetThreadId(m_hThread) != GetCurrentThreadId())
	if (dwRetCode == STILL_ACTIVE)
	{

#ifdef _SOC_TREAD_
		if (!m_fSockTread)
#endif
			SetEvent(m_eExitEvent);
#ifdef _SOC_TREAD_
		WSASetEvent((WSAEVENT)m_eExitEvent);

#endif
		Sleep(1000);
		DWORD dv = GetTickCount();
		BOOL flg = FALSE;
		while (dwRetCode == STILL_ACTIVE)
		{
			Sleep(10);
			if (GetTickCount() - dv > 5000)
			{

				if (flg) break;
				flg = TRUE;
#ifdef _SOC_TREAD_
				if (!m_fSockTread)
#endif
					ResetEvent(m_eExitEvent);
#ifdef _SOC_TREAD_
				WSAResetEvent((WSAEVENT)m_eExitEvent);
#endif

				TerminateThread(m_hThread, 1);
				dv = GetTickCount();
			}
			Sleep(1);
			GetExitCodeThread(m_hThread, &dwRetCode);
		}
		ResetEvent(m_eExitEvent);
	}
	CloseHandle(m_hThread);
	m_hThread = 0;
	return 1;
}
//-----------------------------------------------------------
//
//----------------------------------------------------------
__int64 MThread::GetTime()
{
	FILETIME tmK, tmU, tmDu;
	GetThreadTimes(m_hThread, &tmDu, &tmDu, &tmK, &tmU);
	return FileTimeTo64(&tmU) + FileTimeTo64(&tmK);
}
//-----------------------------------------------------------
//
//----------------------------------------------------------
int MThread::GetPriorety()
{
	return GetThreadPriority(m_hThread);
}
//-----------------------------------------------------------
//
//----------------------------------------------------------
BOOL MThread::SetPriorety(int nPriority)
{
	return SetThreadPriority(m_hThread, nPriority);
}
//-----------------------------------------------------------
//
//----------------------------------------------------------
BOOL MThread::DisableChangePriorety(BOOL fDisable)
{
	return SetThreadPriorityBoost(m_hThread, fDisable);
}
