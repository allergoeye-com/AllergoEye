/*
 * MultiThread.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "MultiThread.h"

int MultiThreadS::OnError()
{

#ifndef WIN32
    int err = errno;
    StdError(-1);
    //  printf ("%s\r\n", strerror(err));
    // errno = 0;
    return err;
#else
    DWORD err = GetLastError();
    if (err)
    {
        LPTSTR *buff;
       int n  = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,  err,
           MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
           (LPTSTR) &buff, 0, NULL);
       if (n)
       {
           TRACE(L"%s\r\n", buff);
           LocalFree(buff);
       }
       SetLastError(0);
   }
   return err;
#endif
}

//--------------------------------------------------------
//
//--------------------------------------------------------
MultiThreadS::~MultiThreadS()
{
    Destroy();
}

//--------------------------------------------------------
//
//--------------------------------------------------------
bool MultiThreadS::WaitThread(THREAD_HANDLE hThread, THREAD_ID *dwThreadID)
{
#ifdef WIN32
    //DWORD dwRetCode;
    //	BOOL f = GetExitCodeThread(hThread, &dwRetCode);
    //	if (f &&  dwRetCode == STILL_ACTIVE)
            WaitForSingleObject (hThread, INFINITE);
        //		TerminateThread(hThread, 1);

        CloseHandle(hThread);
        return true;
#else
    start_func:
    int n = 0;
    if (hThread)
    {
        int i;
        i = pthread_join(hThread, 0);
        if (!n && i < 0)
        {
            i = errno;
            if (i == EINTR)
            {
                usleep(100000);
                n = 100000;
                goto start_func;
            }
            OnError();
            pthread_kill(hThread, 9);

        }
        return true;
        pthread_attr_destroy(dwThreadID);
    }
    return 0;
#endif
}
//--------------------------------------------------------
//
//--------------------------------------------------------

void MultiThreadS::Destroy()
{
    if (num)
    {
        int i;
        for (i = 0; i < num; ++i)
            WaitSemaphore(startup[i]->hNotifyIn);
        for (i = 0; i < num; ++i)
            startup[i]->fRun = 0;
        ReleaseSemaphore(semphIn, num, 0);
        for (i = 0; i < num; ++i)
            WaitSemaphore(startup[i]->hNotifyOut);

        for (i = 0; i < num; ++i)
        {
            WaitThread(startup[i]->hThread, &startup[i]->dwThreadID);
            delete startup[i];
        }

        free(startup);
    }

    if (semphIn) sem_destroy(semphIn);
    if (semphOut) sem_destroy(semphOut);
    if (hNotifyIn) sem_destroy(hNotifyIn);
    if (hNotifyOut) sem_destroy(hNotifyOut);
#ifndef WIN32
    delete semphOut;
    delete semphIn;
    delete hNotifyIn;
    delete hNotifyOut;
#endif
    semphOut = semphIn = hNotifyIn = hNotifyOut = 0;
    num = 0;

}

bool MultiThreadS::Lock()
{
    lock.Enter();
    flock = true;
    return flock;
}

bool MultiThreadS::TryLock()
{
    if (flock) return 0;
    flock = lock.TryEnter();
    return flock;

}

void MultiThreadS::Unlock()
{
    if (flock)
    {
        flock = 0;
        lock.Leave();
    }
}

//--------------------------------------------------------
//
//--------------------------------------------------------

void MultiThreadS::Wait()
{

    int i, count = 0;
    for (i = 0; i < num; ++i)
        WaitSemaphore(startup[i]->hNotifyIn);
    ReleaseSemaphore(semphIn, num, &count);
    for (i = 0; i < num; ++i)
        WaitSemaphore(startup[i]->hNotifyOut);
    for (i = 0; i < num; ++i)
        startup[i]->func = 0;

    ReleaseSemaphore(semphOut, num, &count);
}
//--------------------------------------------------------
//
//--------------------------------------------------------

bool MultiThreadS::ReleaseSemaphore(SEM_HANDLE s, int n, int *count)
{
    if (s)
    {
#ifdef WIN32
        LONG lcount;
        if (!::ReleaseSemaphore(s, n, &lcount))
            OnError();
        if (count)
            *count = (int)lcount;
#else
        if (count)
            if (sem_getvalue(s, count) < 0)
                OnError();
        for (int i = 0; i < n; ++i)
            if (sem_post(s) < 0) OnError();
#endif
        return true;
    }

    return 0;
}
//--------------------------------------------------------
//
//--------------------------------------------------------

bool MultiThreadS::WaitSemaphore(SEM_HANDLE s)
{
    if (s)
#ifdef WIN32
        WaitForSingleObject (s, INFINITE);
#else
        for (;;)
        {
            if (sem_wait(s) != 0)
            {
                int err = errno;
                if (err == EINTR || err == EAGAIN)
                {
                    errno = 0;
                    continue;
                }
                OnError();

            }
            break;
        }
#endif
    return true;

}
//--------------------------------------------------------
//
//--------------------------------------------------------

bool MultiThreadS::CreateSemaphore(SEM_HANDLE *s)
{
#ifdef WIN32
    *s = ::CreateSemaphore(0, 0, num, 0);
    if (!*s)
    {
        OnError();
        return 0;
    }
#else
    *s = new sem_t;
    if (!*s || sem_init(*s, 0, 0) != 0)
    {
        OnError();
        if (*s) delete *s;
        *s = 0;
        return false;
    }
#endif
    return true;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
int MultiThreadS::NumberOfProcessors()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    return sysinfo.dwNumberOfProcessors;
#else
#ifndef ANDROID_NDK
    int n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n <= 0)
        n = 1;
    return n;
#else
    static int n = 0;
    if (!n)
    {
        //Runtime.getRuntime().availableProcessors()
        JVariable runtime, numproc;
        AObject obj, run;
        if (run.New("java.lang.Runtime", 0, 0, AObject::TJSTATIC))
        {
            if (run.Run("getRuntime", 0, 0, runtime))
            {
                obj.Attach(runtime.obj);
                if (obj.Run("availableProcessors", 0, 0, numproc))
                    n = numproc.value.i;
                obj.Detach();
            }
        }
        if (!n)
        {
            n = sysconf(_SC_NPROCESSORS_ONLN);
            if (n <= 0)
                n = 1;
        }
    }
    return n;
#endif
#endif
}

//--------------------------------------------------------
//
//--------------------------------------------------------
bool MultiThreadS::Create(int n)
{
    if (!n)
        n = NumberOfProcessors();
    num = n;
    startup = (Startup **) malloc(num * sizeof(Startup *));
    if (!startup)
    {
        num = 0;
        return false;
    }
    if (!CreateSemaphore(&semphIn) || !CreateSemaphore(&semphOut) ||
        !CreateSemaphore(&hNotifyOut) ||
        !CreateSemaphore(&hNotifyIn))
        return false;
    for (int j = 0; j < num; ++j)
    {
        startup[j] = new Startup();
        startup[j]->semphIn = semphIn;
        startup[j]->semphOut = semphOut;
        startup[j]->func = 0;
        startup[j]->hNotifyIn = hNotifyIn;
        startup[j]->hNotifyOut = hNotifyOut;
        startup[j]->fRun = 1;
#ifdef WIN32
        startup[j]->hThread = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall *) (void *))ThreadFunc, startup[j], 0, &startup[j]->dwThreadID);
        int r = startup[j]->hThread == 0;
#else
        pthread_attr_init(&startup[j]->dwThreadID);
        int r = pthread_create(&startup[j]->hThread, &startup[j]->dwThreadID, ThreadFunc,
                               (void *) startup[j]);
#endif
        if (r)
        {
            OnError();
            delete startup[j];
            --j;
            --num;
        }

    }
    for (int i = 0; i < num; ++i)
        WaitSemaphore(startup[i]->hNotifyOut);
    ReleaseSemaphore(semphOut, num, 0);
    return true;
}

//--------------------------------------------------------
//
//--------------------------------------------------------
bool MultiThreadS::Run(int i, AFX_THREADPROC func, void *Param)
{

    if (!num)
        Create();
    if (num && i < num)
    {
        startup[i]->func = func;
        startup[i]->param_struct = Param;
        return 1;
    }
    return 0;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
#ifndef WIN32

void *MultiThreadS::ThreadFunc(void *Param)
#else
UINT AFX_CDECL MultiThreadS::ThreadFunc(void *Param)
#endif
{
    Startup *startup = (Startup *) Param;

    while (startup->fRun)
    {
        ReleaseSemaphore(startup->hNotifyOut, 1, 0);
        WaitSemaphore(startup->semphOut);

        ReleaseSemaphore(startup->hNotifyIn, 1, 0);
        WaitSemaphore(startup->semphIn);
        if (startup->fRun && startup->func)
            startup->func(startup->param_struct);
    }
    ReleaseSemaphore(startup->hNotifyOut, 1, 0);


    return 0;
}

