#include "stdafx.h"
#include "uxsignal.h"

#ifdef __PLUK_LINUX__

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::Add(int sig)
{
    if (!sigismember((sigset_t *) this, sig))
        sigaddset((sigset_t *) this, sig);


}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool UXSignal::SetHandler(Func handler, int sig)
{
    //if (sigismember(this, sig))
    {
        struct sigaction act;
        memset(&act, 0, sizeof(act));

        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, sig);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler;
        int r = sigaction(sig, &act, NULL);
        return StdError(r) != -1;
    }
    return 0;
}

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::BlockThreads()
{
    StdError(pthread_sigmask(SIG_BLOCK, (sigset_t *) this, &oldSet));

}

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::UnBlockThreads()
{
    StdError(pthread_sigmask(SIG_UNBLOCK, (sigset_t *) this, NULL));

}

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::BlockMainThread()
{
    StdError(sigprocmask(SIG_BLOCK, (sigset_t *) this, &oldSet));
}

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::UnBlockMainThread()
{
    StdError(sigprocmask(SIG_UNBLOCK, (sigset_t *) this, NULL));

}

//-------------------------------------------
//
//-------------------------------------------
void UXSignal::Remove(int sig)
{
    if (sigismember((sigset_t *) this, sig))
        StdError(sigdelset((sigset_t *) this, sig));
}

//-------------------------------------------
//
//-------------------------------------------
UXSignal::UXSignal(bool fEmpty)
{
    sigemptyset(&oldSet);
    sigset_t t;
    int ret1 = sigemptyset(&t);
    StdError(ret1);
    int ret2 = sigemptyset((sigset_t *) this);
    StdError(ret2);

}

//-------------------------------------------
//
//-------------------------------------------
bool UXSignal::Wait(siginfo_t &sig)
{
    sigset_t set;
    sigfillset(&set);
//   struct timespec timeout;
    // size_t milisecond = 100;
    // timeout.tv_nsec = milisecond * 1e6;
    //timeout.tv_sec = 5;
#ifndef ANDROID_NDK
    int n = sigwaitinfo(&set, &sig);
#else
    int isig = 0;
    int n = sigwait(&set, &isig);
    sig.si_signo = isig;
#endif
    if (n == -1)
    {
        if (errno != EAGAIN && errno != EINVAL)
            StdError(-1);
        else
            errno = 0;
    } else
        n = sigismember((sigset_t *) this, n);

    return n != -1;
}

#endif
