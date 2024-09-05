/*
 * MultiThread.h
 *
 *  Created on: Oct 15, 2014
 *      Author: alex
 */

#ifndef MULTITHREAD_H_
#define MULTITHREAD_H_
#ifdef WIN32
#include <process.h>
#include "assert.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#define sem_destroy CloseHandle
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
 #include <signal.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
typedef   unsigned ( *AFX_THREADPROC)( void * );
#define AFX_CDECL
#endif
#ifdef WIN32
#include "critical.h"
#endif
#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class _PEXPORT MultiThreadS {
#ifndef WIN32
	typedef  void *(*StartThreadRoutine) (void *);
	typedef sem_t * SEM_HANDLE;
	typedef pthread_attr_t THREAD_ID;
	typedef pthread_t THREAD_HANDLE;
#else
	typedef HANDLE SEM_HANDLE;
	typedef UINT THREAD_ID;
	typedef HANDLE THREAD_HANDLE;
#endif
	struct Startup {
		SEM_HANDLE semphOut;
		SEM_HANDLE semphIn;
		SEM_HANDLE hNotifyOut;
		SEM_HANDLE hNotifyIn;
		void *param_struct;
		AFX_THREADPROC func;
		THREAD_ID dwThreadID;
		THREAD_HANDLE hThread;
#ifdef WIN32
		LONG fRun;
#else
		int fRun;
#endif

		Startup() { hThread = 0; param_struct = 0; func = 0; fRun = 0; semphOut = semphIn = hNotifyIn = hNotifyOut = 0; }
	};
	CriticalSection lock;
public:
    MultiThreadS() { flock = 0; startup = 0; num = 0; semphOut = semphIn = hNotifyIn = hNotifyOut = 0; }
    ~MultiThreadS();
    int NumberOfProcessors();
   int Num() { return num; };

    bool Create (int n = 0);
    void Wait();
    bool Run(int i, AFX_THREADPROC func, void *Param);
    bool Lock();
    bool TryLock();
    void Unlock();
    void Destroy();

private:
	bool CreateSemaphore(SEM_HANDLE *s);
    bool WaitThread(THREAD_HANDLE hThread, THREAD_ID *);

private:
    static int OnError();
#ifdef WIN32
	static UINT AFX_CDECL ThreadFunc(void *Param);
#else
    static void *ThreadFunc(void *Param);
#endif
    static bool ReleaseSemaphore(SEM_HANDLE s, int n, int *count);
    static bool WaitSemaphore(SEM_HANDLE s);
private:

    Startup **startup;
    bool flock;
    int num;
    SEM_HANDLE semphOut;
    SEM_HANDLE semphIn;
    SEM_HANDLE hNotifyIn;
    SEM_HANDLE hNotifyOut;

};
#pragma pack(pop) //NADA

#endif /* MULTITHREAD_H_ */
