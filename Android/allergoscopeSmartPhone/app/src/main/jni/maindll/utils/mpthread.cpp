/*
 * mpthread.cpp
 *
 *  Created on: Mar 25, 2012
 *      Author: alex
 */

#include "stdafx.h"

#ifndef __PLUK_LINUX__
#include <process.h>
#include <mmsystem.h>
#endif

#include "mpthread.h"
#include "uxmessage.h"

pthread_key_t MPThread::dwStartupPtr = 0;
pthread_key_t MPThread::dwMessageWaitBusy = 0;
pthread_key_t MPThread::dwWaitForMultipleObjects = 0;

QMess::QMess(THWND wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg, bool fIsSend)
{
    fIsSend = false;
    Init(wnd, msg, _wParam, _lParam, arg);
}

void QMess::Init(GHWND hWnd, THWND _wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg,
                 bool _fIsSend)
{
    privat = TlsGetValue(MPThread::dwStartupPtr);
    fIsSend = _fIsSend;
    ghWnd = hWnd;
    iwnd = _wnd;
    message = msg;
    wParam = _wParam;
    lParam = _lParam;
    arg = _arg;
}

void QMess::Init(THWND wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg, bool _fIsSend)
{
    privat = TlsGetValue(MPThread::dwStartupPtr);
    fIsSend = _fIsSend;
    iwnd = wnd;
    message = msg;
    wParam = _wParam;
    lParam = _lParam;
    arg = _arg;
}

//-------------------------------------
//
//-------------------------------------
EXPORTNIX MPThread *MBeginThread(MPThread::StartThreadRoutine func, void *param, bool fStartSuspend)
{
    MPThread *m = new MPThread;
    if (!m->Start(func, param, fStartSuspend))
    {
        delete m;
        m = 0;
    }
    return m;

}

//-------------------------------------
//
//-------------------------------------
MPThread::MPThread()
{
    hThread = 0;
    fKill = TRUE;
    fFirstTime = false;
    startup = 0;

    m_bAutoDelete = false;
    fFreeStartUp = false;

}

//-------------------------------------
//
//-------------------------------------
MPThread::~MPThread()
{
    Stop();
    CleanStartUp();
}

//-------------------------------------
//
//-------------------------------------
bool MPThread::Wait()
{
#ifdef __PLUK_LINUX__
    start_func:
    //    return hThread ? pthread_detach(hThread) == 0 : false;
    if (IsRun() && hThread)
    {
        int i;
        i = pthread_join(hThread, 0);
        if (i < 0)
        {
            i = errno;
            if (i == EINTR)
            {
                Sleep(100);
                goto start_func;
            }
        } else
            return true;
    }

    return 0;
#else
    if (hThread)
    {
        WaitForSingleObject( hThread, INFINITE);
        return true;
    }
    return false;
#endif
}

//-------------------------------------
//
//-------------------------------------
void MPThread::Sleep(UINT uPeriod)
{
#ifdef __PLUK_LINUX__
	#if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 1993409
		timespec ts;
		ts.tv_sec = uPeriod / 1000;
		ts.tv_nsec = (uPeriod % 1000) * 1000000;
		nanosleep(&ts, NULL);
	#else
		usleep(uPeriod * 1000);
	#endif
#else
	::Sleep(uPeriod);
#endif
}

#ifdef ANDROID_NDK
extern "C" void pthread_yield()
{
	usleep(1);

}
#endif

//-------------------------------------
//
//-------------------------------------
void MPThread::TimeBeginPeriod(UINT uPeriod)
{
#ifdef __PLUK_LINUX__
    Sleep(uPeriod + 1);
#else
    timeBeginPeriod(uPeriod);
#endif
}

//-------------------------------------
//
//-------------------------------------
void MPThread::TimeEndPeriod(UINT uPeriod)
{
#ifdef __PLUK_LINUX__
    Sleep(uPeriod + 1);
#else
    timeEndPeriod(uPeriod);
#endif

}

//-------------------------------------
//
//-------------------------------------
void MPThread::Idle()
{
    pthread_yield();

}

bool MPThread::Resume()
{
    if (!hThread) return 0;
    if (fFirstTime)
    {
        fFirstTime = 0;
        startup->cond->SetEvent();
        startup->cond->Leave();
        Sleep(100);


    } else
#ifdef __PLUK_LINUX__
        pthread_kill(hThread, SIGCONT);
#else
    ::ResumeThread(hThread);
#endif
    return true;
}

bool MPThread::Suspend()
{
    if (!hThread) return 0;

#ifdef __PLUK_LINUX__
    pthread_kill(hThread, SIGSTOP);
#else
    ::SuspendThread(hThread);
#endif
    return true;
}

//-------------------------------------
//
//-------------------------------------
void MPThread::CleanFunc(void * arg)
{

try {
		Startup *startup = (Startup *)arg;
		MSGThread *th = startup->MessageProc;
		if (th)
			delete th;
		startup->MessageProc = 0;
		TlsSetValue(dwStartupPtr, 0);
        startup->events->Leave();

		if (startup->CleanUp)
		{
			startup->CleanUp(startup->CleanUpArgs);
		}
		if (startup->self->m_bAutoDelete)
		{
			startup->self->fKill = 0;
			delete startup->self;
		}
		else
		{
			startup->self->hThread = 0;
			pthread_attr_destroy(&startup->self->attr);
		}
	//	TRACE("thread CleanUp\r\n");
		#ifndef __PLUK_LINUX__
			_endthreadex(1);
        #endif
	}
	catch(...)
	{
	#ifndef __PLUK_LINUX__
		_endthreadex(1);
  
	#endif
	}
}




//-------------------------------------
//
//-------------------------------------
#ifndef __PLUK_LINUX__
unsigned __stdcall
#else

void *
#endif
MPThread::ThreadFunc(void *arg)
{

//TRACE("enter Func\r\n");
    Startup *startup = (Startup *) arg;
    StartThreadRoutine func = startup->func;
    startup->cond->Enter();
    arg = startup->arg;
#ifdef __PLUK_LINUX__
#ifndef ANDROID_NDK
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_cleanup_push(CleanFunc, startup);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#else
    pthread_cleanup_push(CleanFunc, startup);
#endif

#endif
        startup->events->Enter();
        startup->start->Enter();
        startup->start->SetEvent();
//	TRACE("SetEvent Func\r\n");
        startup->start->Leave();
//	TRACE("Leave Func\r\n");
        startup->cond->Wait();
//	TRACE("Wait Func\r\n");
//	TRACE("Leave cond Func\r\n");
        TlsSetValue(dwStartupPtr, startup);
        MSGThread *th = new MSGThread;
        startup->cond->Leave();
        func(arg);
#ifdef __PLUK_LINUX__
            pthread_cleanup_pop(1);
#else
    CleanFunc(startup);
#endif

    return 0;
}

#if(1) //def ANDROID_NDK

void MPThread::Startup::PushPool(void *msg)
{
    if (critFifo)
    {
        critFifo->Enter();
        Pool->Push(msg);
        critFifo->Leave();
    }
}

#endif

void MPThread::Startup::PushFifo(TMSG &msg)
{
    if (critFifo)
    {
        critFifo->Enter();
//        if (m_privateHash->Find(msg.hwnd))
        fifo->Push(msg);
        //      else
        //        ASSERT(0);
        critFifo->Leave();
    }
}

TMSG MPThread::Startup::PopFifo()
{
    TMSG msg;
    if (critFifo)
    {
        critFifo->Enter();
        msg = fifo->Pop();
        critFifo->Leave();
    }
    return msg;
}

int MPThread::Startup::LenFifo()
{
    int len = 0;
    if (critFifo)
    {
        critFifo->Enter();
        len = fifo->Len();
        critFifo->Leave();
    }
    return len;

}
//extern void CleanUpMSGThreads(MPThread *p);

//-------------------------------------
//
//-------------------------------------
void MPThread::CleanStartUp()
{
    if (startup && fFreeStartUp)
    {
        MSGThread *th = (MSGThread *) startup->MessageProc;
        if (th)
            delete th;
        startup->MessageProc = 0;
        startup->events->Leave();
        delete startup->critMsg;
        delete startup->critEvents;
        delete startup->critFifo;
#if (1) //def ANDROID_NDK
        delete startup->Pool;
#endif

        delete startup->cond;
        delete startup->start;
        delete startup->events;
        delete startup->sign;


#ifdef __PLUK_LINUX__
        delete startup->fifo;
        if (startup->iNotifyHandle > 0) close(startup->iNotifyHandle);
#endif
        bzero(startup, sizeof(Startup));
        HFREE(startup);
    }
    startup = 0;
}

//-------------------------------------
//
//-------------------------------------
void MPThread::InitStartUp(StartThreadRoutine _ThreadFunc, void *param, bool fStartSuspend,
                           StartThreadRoutine _CleanUp, void *paramCleanUp)
{
    CleanStartUp();
    fFreeStartUp = true;
    startup = (Startup *) HMALLOC(sizeof(Startup));
    memset(startup, 0, sizeof(Startup));
    startup->self = this;
    startup->cond = new CondEvent2;
    startup->start = new CondEvent2;
    startup->events = new CondEvent2;
    startup->func = _ThreadFunc;
    startup->arg = param;
    startup->CleanUp = _CleanUp;
    startup->CleanUpArgs = paramCleanUp;
    startup->critMsg = new CriticalSection;
    startup->critEvents = new CriticalSection;
    startup->critFifo = new CriticalSection;
    startup->sign = new LocalSemaphore();
#ifdef __PLUK_LINUX__
    startup->fifo = new MFifo<TMSG>;
#if(1) //def ANDROID_NDK
    startup->Pool = new MFifo<void *>;
#endif

    startup->iNotifyHandle = 0;
#endif
}

//-------------------------------------
//
//-------------------------------------
bool MPThread::Attach(bool fIsMainThread)
{
    Startup *_startup = (Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (_startup) return false;
    InitStartUp(0, 0, 0, 0, 0);
    startup->fMainThread = fIsMainThread;
    hThread = GetCurrentThread();
#ifndef __PLUK_LINUX__
    dwThreadID = GetCurrentThreadId();
#endif
    TlsSetValue(dwStartupPtr, startup);

    fKill = false;
    fFirstTime = false;
    startup->events->Enter();
    MSGThread *th = new MSGThread;
    return true;
}

extern void CleanUpMSGThreads(MPThread *thread);

bool MPThread::Detach()
{
    Startup *_startup = (Startup *) TlsGetValue(MPThread::dwStartupPtr);
    CleanUpMSGThreads(this);
    if (_startup == 0 || startup == 0 || _startup != startup) return false;
    startup->events->Leave();
    CleanStartUp();
    TlsSetValue(dwStartupPtr, 0);
    hThread = 0;
    return true;
}

//-----------------------------------------------------------
//
//----------------------------------------------------------
bool MPThread::Start(StartThreadRoutine _ThreadFunc, void *param, bool fStartSuspend,
                     StartThreadRoutine _CleanUp, void *paramCleanUp)
{
    Stop();
    InitStartUp(_ThreadFunc, param, fStartSuspend, _CleanUp, paramCleanUp);
    startup->start->Enter();

#ifdef __PLUK_LINUX__
    pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&hThread, &attr, ThreadFunc, (void *) startup) == 0)
    {

#else
        if (hThread = (pthread_t)_beginthreadex(0, 0, (unsigned (__stdcall *) (void *))ThreadFunc,  startup, 0, (UINT *)&dwThreadID))
        {

#endif
        Idle();
        startup->start->Wait();
        startup->start->Leave();
        startup->cond->Enter();
        fFirstTime = 1;
        if (!fStartSuspend)
            Resume();
    } else
    {
        startup->start->Leave();
        CleanStartUp();
    }

    return hThread != 0;
}

//-----------------------------------------------------------
//
//----------------------------------------------------------
bool MPThread::Stop()
{
    if (hThread)
    {
        if (fKill)
        {
            m_bAutoDelete = 0;

#ifndef __PLUK_LINUX__
            DWORD dwRetCode;
            for (int i = 0; i < 10; ++i)
                Sleep(0);
            GetExitCodeThread(hThread, &dwRetCode);
            if(dwRetCode == STILL_ACTIVE)
            {
                TerminateThread(hThread, 1);
            }
#else
            pthread_kill(hThread, 9);
#ifndef ANDROID_NDK
            startup->sign->Leave();
            if (!pthread_cancel(hThread))
            while(IsRun())
                Sleep(500);
#endif
#endif
        }
#ifndef __PLUK_LINUX__
        CloseHandle(hThread);
#else
        pthread_attr_destroy(&attr);
#endif
    }

    hThread = 0;

    return 1;
}

//-------------------------------------
//
//-------------------------------------
bool MPThread::IsRun()
{
#ifndef __PLUK_LINUX__
    DWORD dwRetCode = 0;
    return hThread && GetExitCodeThread(hThread, &dwRetCode) && dwRetCode == STILL_ACTIVE;
#else
    return startup && (startup->fMainThread || (hThread && !pthread_kill(hThread, 0)));
#endif
}
