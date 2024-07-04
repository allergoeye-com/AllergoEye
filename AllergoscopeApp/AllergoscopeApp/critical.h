///////////////////////////////////////////////////
// critical.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __CriticalSection__
#define __CriticalSection__
struct CriticalSection  {
	long m_iRef;
	CRITICAL_SECTION m_crit;
	CriticalSection () 
	{
		m_iRef = 0; 
		InitializeCriticalSection(&m_crit); 
	}
	~CriticalSection () 
	{ 
		DeleteCriticalSection(&m_crit); 
	}
    void Leave() 
	{  
		if (!m_iRef) 
			return;
		InterlockedDecrement(&m_iRef); 
		LeaveCriticalSection(&m_crit); 
	}
    void Enter() 
	{ 
		EnterCriticalSection(&m_crit); 
		InterlockedIncrement(&m_iRef); 
	}
	bool IsEntred() { return m_iRef > 0; }
	bool Check() 
	{ 
		return m_iRef && (DWORD)(DWORD_PTR)m_crit.OwningThread  == GetCurrentThreadId(); 
	}
	#ifdef _VER_PLK_VS_8
		BOOL TryEnter() 
		{ 
			if (TryEnterCriticalSection(&m_crit)) 
			{
				InterlockedIncrement(&m_iRef); 
				return true; 
			} 
			return false;  
		} 
	#else
		BOOL TryEnter() 
		{ 
			EnterCriticalSection(&m_crit); 
			InterlockedIncrement(&m_iRef); 
			return true; 
		} 
	#endif
};
#endif
