#include "stdafx.h"
#include "msgthread.h"
#include "uxmessage.h"
#include "mstack.h"
#include "mfile.h"
#include "critical.h"
#include "hashset.h"
#include "minstance.h"
#include <sys/timerfd.h>

#define _TEST_WX_1
#ifdef __PLUK_LINUX__
#ifdef ANDROID_NDK
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif
#define SIG_ANDROID SIGUSR2
#define SIG_ANDROID_TIME SIGUSR1
#endif
#define NUM_SIG 8

void CopyQMsg(QMess *msg, QMess *_msg)
{
    msg->iwnd = _msg->iwnd;
    msg->ghWnd = _msg->ghWnd;
    msg->message = _msg->message;
    msg->wParam = _msg->wParam;
    msg->lParam = _msg->lParam;
    msg->fIsSend = _msg->fIsSend;
    msg->iarg = _msg->iarg;
    msg->privat = _msg->privat;
}

struct ConProcSendMsg {
    int fd;
    BOOL *ret;
    union {
        MPThread::Startup *startup;
        pid_t pid;
    };

    void Set(int _fd, BOOL *_ret, void *_startup)
    {
        fd = _fd;
        ret = _ret;
        startup = (MPThread::Startup *) _startup;
    }
};

static int mTransMsg[NUM_SIG] = {SIG_SENDMESSAGE_WND, SIG_POSTMESSAGE_WND, SIG_SENDMESSAGE,
                                 SIG_POSTMESSAGE, SIG_RUN_MY, SIG_SOCK_SEND, SIGUSR1,
                                 SIGUSR1}; //, SIGIO, SIGALRM, SIGINT, SIGHUP, SIGCHLD}; //, SIG_REAL_TIME};
BOOL privateTranslateMessage(TMSG *msg);

BOOL privatePeekMessage(TMSG *msg, GHWND filter, BOOL fRemove);

BOOL privatePeekMessage(TMSG *msg, BOOL fRemove);

//BOOL privatePeekMessage(TMSG *msg, void *obj, BOOL fRemove);
BOOL privateGetMessageAll(TMSG *msg, BOOL fAll = true);

BOOL privateGetMessage(TMSG *msg, BOOL fAll = true);

BOOL privateDispatchMessage(TMSG *msg);

BOOL privateInitTMSG(TMSG *msg, siginfo_t &sig);

bool _SetEvent(MPThread::Startup *startup, UINT Msg);

UINT MakeTmpId()
{
    return (UINT) (((((UINT) time(0L) << 10) + GetTickCount()) << 14) + GetCurrentThreadId() +
                   rand());
}
//-------------------------------------
//
//-------------------------------------
#pragma pack (push)
#pragma pack (4)

struct tagTHWND {
    pid_t iPid;
    union {
        struct {
            USHORT iCurThread;
            USHORT iProcThread;
        };
        UINT iThread;
    };
};
#pragma pack (pop)

class HashMapWnd : public RBHashSet<MSGThread *> {
public:
    HashMapWnd()
    {
        indThread = 0;
        ind = 1;
    };

    ~HashMapWnd()
    {};

    bool Find(MSGThread *key)
    {
        sec.Enter();
        bool fRet = RBHashSet<MSGThread *>::Find(key);
        sec.Leave();
        return fRet;
    }

    bool Find(THWND key, MSGThread **ret)
    {
        bool fRet = FALSE;
        sec.Enter();
        tagTHWND *t = (tagTHWND *) &key;
        if (t->iPid == getpid())
        {
            MSGThread *th = 0;
            if (map.Lookup(t->iThread, th) && th)
            {
                fRet = RBHashSet<MSGThread *>::Find(th);
                if (!fRet)
                {
                    ASSERT(0);
                    map.Delete(t->iThread);
                } else
                    *ret = th;
            }
        }
        sec.Leave();
        return fRet;
    }

    void Delete(THWND key)
    {
        sec.Enter();
        tagTHWND *t = (tagTHWND *) &key;
        MSGThread *th = 0;
        if (t->iPid == getpid())
        {
            if (map.Lookup(t->iThread, th) && th)
            {
                RBHashSet<MSGThread *>::Delete(th);
                map.Delete(t->iThread);
            }
        }
        sec.Leave();
    }

    THWND Add(MSGThread *key)
    {
        THWND res;
        sec.Enter();
        tagTHWND *t = (tagTHWND *) &res;
        t->iCurThread = ind;

        MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
        if (startup->MessageProc && startup->MessageProc != key)
        {
            tagTHWND *t1 = (tagTHWND *) &startup->MessageProc->m_hWnd;
            t->iProcThread = t1->iCurThread;
        } else
        {
            ++indThread;
            t->iProcThread = indThread;
        }
        t->iPid = getpid();
        ++ind;

        bool fRet = RBHashSet<MSGThread *>::Add(key);
        if (fRet) map[t->iThread] = key;
        sec.Leave();
        return fRet ? res : 0;
    }

    CriticalSection sec;
    MHashMap<MSGThread *, UINT> map;
    USHORT ind;
    USHORT indThread;
};

HashMapWnd *m_privateHash = 0;

static MSGThread *cast2MSGThread(THWND ptr)
{
    MSGThread *th = 0;
    if (m_privateHash->Find(ptr, &th))
        return th;

    return 0;
}

MSGThread *MSGThread::FromHandle(THWND ptr)
{ return cast2MSGThread(ptr); }

MPThread *MPThread::FromHandle(pthread_t t)
{
    MPThread *ret = 0;
    m_privateHash->sec.Enter();
    RBHashSet<MSGThread *>::iterator it(*m_privateHash);
    for (MSGThread **pp = it.begin(); pp; pp = it.next())
    {
        MSGThread *th = *pp;
        if (th && th->startup && th->startup->self && th->startup->self->hThread == t)
        {
            ret = th->startup->self;
            break;
        }
    }
    m_privateHash->sec.Leave();

    return ret;
}

MSGThread *MSGThread::FromHandle(MPThread *t)
{
    return t && t->startup ? t->startup->MessageProc : 0;
}

void CleanUpMSGThreads(MPThread *thread)
{
    m_privateHash->sec.Enter();
    RBHashSet<MSGThread *>::iterator it(*m_privateHash);
    for (MSGThread **pp = it.begin(); pp; pp = it.next())
    {
        MSGThread *th = *pp;
        if (th && th->startup && th->startup->self == thread)
        {
            th->startup = 0;
            if (th->timer && th->timer->Len())
            {

                MHashMap<MSGThread::mTimer, UINT>::iterator it(*th->timer);
                for (RBData<MSGThread::mTimer, UINT> *p = it.begin(); p; p = it.next())
                {
                    WSACancel((SOCKET) p->value.id);
                    close(p->value.id);
                }
                delete th->timer;
            }
            tagTHWND *t = (tagTHWND *) &th->m_hWnd;
            m_privateHash->RBHashSet<MSGThread *>::Delete(th);
            m_privateHash->map.Delete(t->iThread);
            th->m_hWnd = 0;
        }
    }
    m_privateHash->sec.Leave();
}
//-------------------------------------
//
//-------------------------------------
void GUIIdle()
{
       MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
 
#ifndef _CONSOLE_PROG_
    if (startup->fMainThread)
        MGetApp()->GuiIdle();
   else
#endif
    MPThread::Idle();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
MSGThread::MSGThread(bool fInit)
{


    timer = 0;
    if (!m_privateHash)
        m_privateHash = new HashMapWnd;
    m_hWnd = 0;
    if (!fInit)
        startup = 0;
    else
        InitMsg();
}

MPThread *MSGThread::Self()
{

    return startup ? startup->self : 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void MSGThread::InitMsg()
{
    startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (startup && startup->self->IsRun())
    {
        m_hWnd = m_privateHash->Add(this);
        if (!startup->MessageProc)
        {
            TlsSetValue(MPThread::dwWaitForMultipleObjects, 0);
            DWORD_PTR *ptr = new DWORD_PTR[2];
            ptr[0] = ptr[1] = 0;
            TlsSetValue(MPThread::dwMessageWaitBusy, (void *) ptr);
            startup->MessageProc = this;
        }
        InitSignals();
    }
}

#if(1) //ndef ANDROID_NDK
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL MSGThread::SetTimer(UINT iTimeID, size_t mili, TimerProc func)
{

    if (!startup) return false;
    if (timer)
        if (timer->Lookup(iTimeID))
        {
            if ((size_t)(*timer)[iTimeID].tm == mili)
            return 0;
            KillTimer(iTimeID);
            
        }

    int tfd;
    if((tfd= timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK)) < 0)
    {
        StdError(-1);
        return FALSE;
    }
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    itimerspec value;
    bzero(&value,sizeof(itimerspec));  
    struct timespec ts;
    ts.tv_sec = now.tv_sec + mili/1000;
    ts.tv_nsec = (mili%1000) * 1000000;

    value.it_value = ts;
    ts.tv_sec = mili/1000;
    value.it_interval = ts;
    if( timerfd_settime(tfd,TFD_TIMER_ABSTIME,&value,0) <0)
    {
                StdError(-1);

        close(tfd);
        return 0;
    }   
    if (!timer)
    {
        timer = new MHashMap<mTimer, UINT>;
        timer->CreateNew(false);
    }
    (*timer)[iTimeID].Set(tfd, (UINT)mili, func);
    return WSASelect((SOCKET)tfd, this, WM_TIMER, FD_ACCEPT) != 0;	

}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL MSGThread::KillTimer(UINT iTimeID)
{
        if (!startup) return false;

    if (!timer || !timer->Lookup(iTimeID)) return 0;
    WSACancel((SOCKET)(*timer)[iTimeID].id);
    close((*timer)[iTimeID].id);
    timer->Delete(iTimeID);
    if (!timer->Len())
    {
        delete timer;
        timer = 0;
    }
    return 1;
   
}
#endif
#if (0) //def _CONSOLE_PROG_

void UXMessageCalback(int n, siginfo_t *sig, void *context)
{
    MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    TMSG msg;
    if (!startup) return;
     UXSignal &set = startup->self->ProcSignal;
      set.BlockMainThread();
    memset(&msg, 0, sizeof(TMSG));
    if (n ==  SIG_POSTMESSAGE || n == SIG_POSTMESSAGE_WND)
    {
        QMess *p = (QMess *)sig->si_value.sival_ptr;
        if ((((size_t)p->hwnd) >> 16) & 0xFFFF)
        {
            memcpy(&msg, p, sizeof(QMess));
            HFREE(p);
            memcpy(&msg.sig, sig, sizeof(siginfo_t));
            msg.sig.si_value.sival_ptr = msg.hwnd;

        }
        else
            n = -1;
        if (!m_privateHash->Find(cast2MSGThread(msg.iwnd))) return;
    }
    else
    for (int i = 0; i < NUM_SIG; ++i)
        if (n == mTransMsg[i])
        {
            
            msg.message = n;
            memcpy(&msg.sig, sig, sizeof(siginfo_t));
        }
    
        
    do 
    {

       TranslateMessage(&msg);
       DispatchMessage(&msg);

    }while (PeekMessage(&msg, TRUE));
    set.UnBlockMainThread();	

}
#endif
#if (0) //def ANDROID_NDK
void AndroidMainThreadHandler(int n, siginfo_t *sig, void*)
{
   MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    if (startup  &&  startup->MessageProc)
    {

       int nsig = 0;
        static int fBusy = 0;

        ++fBusy;
       if (n < 0) return ;
       startup->critFifo->Enter();
       if (startup->Pool->Len())
       {
           QMess *msg = (QMess *)startup->Pool->Pop();
           if (msg)
           {
               sig->si_value.sival_ptr = msg;
               sig->si_signo = (int)msg->ghWnd;
               nsig = sig->si_signo;
               msg->ghWnd = 0;

           }
       }
       startup->critFifo->Leave();
        TMSG msg;
          startup->PushFifo(msg);
          --fBusy;
          if (!fBusy)
          while(startup->LenFifo())
          {
           msg = startup->PopFifo();
           TranslateMessage(&msg);
           DispatchMessage(&msg);

        }
    }
}
#endif

//-----------------------------------------------------
//
//-----------------------------------------------------
void MSGThread::InitSignals()
{
    if (!startup) return;

    errno = 0;
#if (0)
    UXSignal &set = startup->self->ProcSignal;
#ifndef ANDROID_NDK
       if (sigismember((sigset_t*)&set, SIG_SENDMESSAGE)) return;
        for (int i = 0; i < NUM_SIG; ++i)
        {
            if (!startup->fMainThread && mTransMsg[i] == SIG_UXMESSAGE) continue;
            {

            set.SetHandler((void (*)(int, siginfo_t*, void*))SIG_DFL, mTransMsg[i]);
            set.Add(mTransMsg[i]);
            }
        }
#else
       if (sigismember((sigset_t*)&set, SIG_ANDROID)) return;
        set.SetHandler((void (*)(int, siginfo_t*, void*))SIG_DFL, SIG_ANDROID);
        set.Add(SIG_ANDROID);
        set.SetHandler((void (*)(int, siginfo_t*, void*))SIG_DFL, SIG_ANDROID_TIME);
        set.Add(SIG_ANDROID_TIME);
#endif
    //    setRealTime.SetHandler(RealTimeSig, mTransMsg[NUM_SIG]);
      //  setRealTime.Add(mTransMsg[NUM_SIG]);
    //setRealTime.UnBlockMainThread();
        if (startup->fMainThread)
                set.BlockMainThread();
        else
                set.BlockThreads();
#endif


}

void MSGThread::Detach()
{
    if (startup)
    {
        if (timer && timer->Len())
        {

            MHashMap<mTimer, UINT>::iterator it(*timer);
            for (RBData<mTimer, UINT> *p = it.begin(); p; p = it.next())
            {
                if (ePollGLB)
                    ePollGLB->PostMessage(WM_USER + MSG_DELSOCK, p->value.id, 0);
                close(p->value.id);
            }
            delete timer;
        }
    }
    m_privateHash->Delete(m_hWnd);
    startup = 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
MSGThread::~MSGThread()
{
    if (startup)
    {
        if (timer && timer->Len())
        {

            MHashMap<mTimer, UINT>::iterator it(*timer);
            for (RBData<mTimer, UINT> *p = it.begin(); p; p = it.next())
            {
                if (ePollGLB)
                    ePollGLB->PostMessage(WM_USER + MSG_DELSOCK, p->value.id, 0);
                close(p->value.id);
            }
            delete timer;
        }
    }
    m_privateHash->Delete(m_hWnd);

}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL MSGThread::PumpMessage()
{
    if (!startup) return false;

    TMSG msg;
    if (!PeekMessage(&msg, TRUE))
//    if (!GetMessageA(&msg, true))
        return FALSE;
    TranslateMessage(&msg);
    return DispatchMessage(&msg);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT MSGThread::OnMessage(TMSG &msg)
{
    if (!startup) return false;
    if (msg.ghWnd)
        return WindowProc(msg.ghWnd, msg.message, msg.wParam, msg.lParam);
    return WindowProc(msg.iwnd, msg.message, msg.wParam, msg.lParam);

}

LRESULT MSGThread::RunMy(void *func, void *arg)
{
    if (startup && startup->self->IsRun())
    {
        union sigval sval;

        QMess *val = (QMess *) HMALLOC(sizeof(QMess));
        val->Init((THWND) func, 0, (WPARAM) arg, 0, 0);
        sval.sival_ptr = val;
        if (!StdError(pthread_sigqueue(startup->self->ID(), SIG_RUN_MY, sval)))
        {
            GUIIdle();
            return 1;
        }
        StdError(-1);
        HFREE(val);
    }
    return 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT MSGThread::PostMessage(GHWND hWnd, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (startup && startup->self->IsRun())
    {
        union sigval sval;

        QMess *val = (QMess *) HMALLOC(sizeof(QMess));
        val->Init(hWnd, m_hWnd, Msg, wParam, lParam, arg);

        sval.sival_ptr = val;
        if (!StdError(
                pthread_sigqueue(startup->self->ID(), hWnd ? SIG_POSTMESSAGE_WND : SIG_POSTMESSAGE,
                                 sval)))
        {
            _SetEvent(startup, Msg);

            return 1;
        }
        StdError(-1);
        HFREE(val);
    }
    return 0;

}

int MSGThread::pthread_sigqueue(pthread_t __threadid, int __signo, const union sigval __value)
{
    if (!startup) return false;

#ifdef _TEST_WX_

    if (startup->fMainThread)
    {
        TMSG msg;
        if (__signo ==  SIG_POSTMESSAGE || __signo == SIG_POSTMESSAGE_WND)
        {
            QMess *p = (QMess *)__value.sival_ptr;
            if ((((size_t)p->hwnd) >> 16) & 0xFFFF)
            {
                memcpy(&msg, p, sizeof(QMess));
                msg.sig.si_value.sival_ptr = msg.hwnd;
                msg.sig.si_signo = __signo;

                HFREE(p);

            }
            else
                return -1;
            if (!m_privateHash->Find(msg.hwnd)) return -1;
        }
        else
        if (__signo ==  SIG_RUN_MY)
        {
            QMess *p = (QMess *)__value.sival_ptr;
            if ((((size_t)p->hwnd) >> 16) & 0xFFFF)
            {
              memcpy(&msg, p, sizeof(QMess));
                HFREE(p);
            }
        }
        else
        {
            int i;
            for (i = 0; i < NUM_SIG; ++i)
            if (__signo == mTransMsg[i])
            {
                msg.message = __signo;    
                msg.sig.si_signo = __signo;
                msg.sig.si_value.sival_ptr = __value.sival_ptr;
                msg.hwnd = (__value.sival_ptr);
                break;
            }
            if (i == NUM_SIG) return -1;
        }
        startup->PushFifo(msg);

            GUIIdle();

        return 0;  

    }
#endif
    int ret = -1;
#if (1)
    QMess *p = (QMess *) __value.sival_ptr;
    p->cur_sig = __signo;
    startup->PushPool(__value.sival_ptr);
    ret = startup->sign->Leave();
    return 0;

#else
#ifndef ANDROID_NDK
    ret = ::pthread_sigqueue (__threadid, __signo, __value);
#else
    QMess *p = (QMess *)__value.sival_ptr;
    p->ghWnd = (GHWND)__signo;
    startup->PushPool(__value.sival_ptr);
    ret = pthread_kill(__threadid, SIG_ANDROID);
    StdError(ret);
#endif
#endif

    return ret;

}

bool _SetEvent(MPThread::Startup *startup, UINT Msg)
{
    if (!startup) return false;

    bool res;
    startup->critEvents->Enter();
    if ((res = startup->events->TryEnter()))
    {
        startup->events->SetEvent();
        startup->events->Leave();
    } 
    else
    {
         GUIIdle();
    }
    startup->critEvents->Leave();
    return res;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT MSGThread::SendMessage(GHWND hWnd, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (!startup) return 0;
    LRESULT fRet = 0;
#ifdef __PLUK_DLL__
    Processor *_Pluk = GetOwner();
    if (_Pluk)
    {
        if (!_Pluk->GetEndFlagStatus())
            _Pluk->SetRequestToInterrupt();
    }
#endif
    if (startup && startup->self->IsRun())
    {
        if (startup->self->ID() == GetCurrentThreadId())
        {
            TMSG msg;
            msg.Init(hWnd, m_hWnd, Msg, wParam, lParam, arg, false);
            msg.sig.si_signo = hWnd ? SIG_POSTMESSAGE_WND : SIG_POSTMESSAGE;
            return OnMessage(msg);
        } else
        {
            union sigval sval;
            //  if (!Lock()) return 0;
            DWORD_PTR *p = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
            MDeq<ConProcSendMsg> *dec = (MDeq<ConProcSendMsg> *) p[1];
            if (dec)
                for (int i = 0, l = dec->Len(); i < l; ++i)
                {
                    if ((*dec)[i].startup == startup)
                    {
                        printf("rolback MSGThread::sendmessage\r\n");
                        ASSERT(0);
                        return 0;
                    }
                }

            MPThread::Startup *_startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
            QMess *val = (QMess *) HMALLOC(sizeof(QMess));
            val->Init(hWnd, m_hWnd, Msg, wParam, lParam, arg);
            sval.sival_ptr = val;
            _startup->start->Enter();

            _startup->DataEx = &fRet;

            if (!StdError(pthread_sigqueue(startup->self->ID(),
                                           hWnd ? SIG_SENDMESSAGE_WND : SIG_SENDMESSAGE, sval)))
            {
                _SetEvent(startup, Msg);
                _startup->start->Wait();
                _startup->message.Init(hWnd, m_hWnd, Msg, wParam, lParam, arg, true);
                _startup->start->SetEvent();
                _startup->start->Wait();

            }
            _startup->start->Leave();
            //   UnLock();
        }
    }
    return fRet;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT MSGThread::MessageProc(TMSG &msg, BOOL *fFreeSendedThread)
{
    if (!startup) return false;
    LRESULT ret = 0;

    if (msg.sig.si_signo == SIG_SENDMESSAGE_WND || msg.sig.si_signo == SIG_POSTMESSAGE_WND ||
        msg.sig.si_signo == SIG_SENDMESSAGE || msg.sig.si_signo == SIG_POSTMESSAGE ||
        msg.sig.si_signo == SIG_SOCK_SEND)
    {
        GUIIdle();
        ret = OnMessage(msg);
    } else if (msg.sig.si_signo == SIGIO)
        OnIo(msg);


        GUIIdle();
#ifdef __PLUK_DLL__
   // else
    {
    Processor *_Pluk = GetOwner();
    if (_Pluk)
    {
        if (!_Pluk->GetEndFlagStatus())
            _Pluk->SetRequestToInterrupt();
    }
    }
#endif
    return ret;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool MSGThread::WaitMessage(TMSG *msg)
{
    MPThread::Startup *startup = (MPThread::Startup *) msg->privat;
    if (!startup) return 0;
    DWORD_PTR *p = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
    bool fRet = 0;
    if (startup && startup->self->IsRun())
    {


        //TlsSetValue(MPThread::dwMessageWaitBusy, (void *)1);
        if (startup->DataEx)
        {
#ifdef __PLUK_DLL__
            Processor *pluk = ::GetOwner();
                        if (pluk)
                                pluk->ResetRequestToInterrupt();
#endif
            startup->start->Enter();
            startup->start->SetEvent();
            startup->start->Wait();
            CopyQMsg(msg, &startup->message);

            MSGThread *proc = cast2MSGThread(msg->iwnd);

            MDeq<ConProcSendMsg> *dec = (MDeq<ConProcSendMsg> *) p[1];
            if (!dec)
            {
                dec = new MDeq<ConProcSendMsg>;
                p[1] = (DWORD_PTR) dec;
            }

            MSGThread *th = proc ? proc : dynamic_cast<MSGThread *>(startup->MessageProc);
            BOOL fFreeSendedThread = FALSE;
            dec->PushFront().Set(-1, &fFreeSendedThread, msg->privat);
            LRESULT ret;
            if (th)
                ret = th->MessageProc(*msg, &fFreeSendedThread);
            if (!fFreeSendedThread)
            {
                *((LRESULT *) startup->DataEx) = ret;

                startup->start->SetEvent();
                startup->start->Leave();
                ConProcSendMsg msg = dec->PopFront();
                if (!dec->Len())
                {
                    delete dec;
                    p[1] = 0;
                }
                return fRet;

            }

            fRet = true;
        }

        //TlsSetValue(MPThread::dwMessageWaitBusy, (void *)0);
    }
    return fRet;

}


//-------------------------------------
//
//-------------------------------------
void MSGThread::GUIIdle()
{
    ::GUIIdle();
}

//-------------------------------------
//
//-------------------------------------
MSGThread *MSGThread::GetCurrent()
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
    return startup ? dynamic_cast<MSGThread *>(startup->MessageProc) : 0;
}

//-------------------------------------
//
//-------------------------------------
BOOL MSGThread::ReplyMessage(LRESULT res)
{
    BOOL fRet = 0;
    DWORD_PTR *ptr = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
    MSGThread *p;
    MDeq<ConProcSendMsg> *dec = (MDeq<ConProcSendMsg> *) ptr[1];
    if (!dec) return 0;
    ConProcSendMsg msg = dec->PopFront();
    if (msg.fd == -1)
    {
        MPThread::Startup *startup = (MPThread::Startup *) msg.startup;
        if (startup)
        {
            *((LRESULT *) startup->DataEx) = res;
            *msg.ret = 1;
            startup->start->SetEvent();
            startup->start->Leave();
            fRet = 1;
        }
    } else
    {

        DWORD64 res64 = res;
        struct stat st;
        if (fstat((int) msg.fd, &st) == 0)
        {
            StdError(send(msg.fd, &res64, sizeof(DWORD64), 0));
            *msg.ret = 1;
            close(msg.fd);
            fRet = 1;
        } else
        {
            StdError(-1);
            *msg.ret = 0;
            fRet = 0;
        }


    }
    if (!dec->Len())
    {
        delete dec;
        ptr[1] = 0;
    }
    return fRet;

}

//-------------------------------------
//
//-------------------------------------
void *VThread::exit_func(void *arg)
{
    VThread *th = (VThread *) arg;

    return 0; //th? th->OnExit() : 0;
}

void *VThread::func(void *arg)
{
    VThread *th = (VThread *) arg;
    th->InitMsg();
    th->OnStart();
    TMSG m;
    th->fRun = 1;
    while (th->fRun)
    {
        if (PeekMessage(&m, TRUE))
        {
            TranslateMessage(&m);
            DispatchMessage(&m);
            loop:
            switch (m.message)
            {
                case WM_DESTROY:
                case MSG_DESTROY:
                    return 0;
                case MSG_PAUSE:
                    if (GetMessageA(&m))
                    {
                        TranslateMessage(&m);
                        DispatchMessage(&m);
                        if (m.message == MSG_DESTROY) goto loop;
                        if (m.message != MSG_PLAY)
                        {
                            m.message = MSG_PAUSE;
                            goto loop;
                        }
                    }
                    break;

            }
        }
        if (th->m_crit.TryEnter())
        {
            th->OnRun();
            th->m_crit.Leave();
        }
    }
    return 0;

}

//-------------------------------------
//
//-------------------------------------
BOOL VThread::Stop()
{

    PostMessage(MSG_DESTROY, 0, 0);
    while (th.IsRun())
    {
        Sleep(0);
    }
    m_crit.Enter();
    th.Stop();
    m_crit.Leave();
    return true;
}

//-------------------------------------
//
//-------------------------------------
BOOL VThread::Pause()
{
    m_crit.Enter();
    BOOL f = SendMessage(MSG_PAUSE, 0, 0);
    m_crit.Leave();
    return f;
}

//-------------------------------------
//
//-------------------------------------
BOOL VThread::Play()
{
    if (!th.hThread)
        th.Start(func, this, FALSE, exit_func, this);
    m_crit.Enter();
    BOOL f = SendMessage(MSG_PLAY, 0, 0);
    m_crit.Leave();
    return f;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void EpollThread::UnLock()
{

    if (!startup) return;

//	if (startup->DataEx)
    {
//		startup->start->Leave();
//		startup->DataEx = 0;
        startup->critMsg->Leave();
        GUIIdle();
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool EpollThread::Lock()
{

    if (!startup) return false;

#ifdef __PLUK_DLL__
    Processor *_Pluk = GetOwner();
#endif
    while (!startup->critMsg->TryEnter())
    {
#ifdef __PLUK_DLL__
        if (_Pluk)
    if (_Pluk->GetEndFlagStatus())
        return 0;
#endif

        GUIIdle();
        
    }
//	startup->start->Enter(); 
#ifdef __PLUK_DLL__
    if (_Pluk)_Pluk->SetRequestToInterrupt();
#endif

    return 1;
}

//-------------------------------------
//
//-------------------------------------
EpollThread::~EpollThread()
{
    if (sockMsg[0] > 0) close(sockMsg[0]);
    if (sockMsg[1] > 0) close(sockMsg[1]);
    if (epfd) close(epfd);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void EpollThread::Init()
{
    InitMsg();
    epfd = epoll_create(EPOOL_SIZE);
    if (epfd < 0)
        StdError(-1);
    sockMsg[0] = sockMsg[1] = -1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT EpollThread::SendMessage(MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (sockMsg[1] == -1) return 0;
    if (!startup) return 0;
    LRESULT fRet = 0;

    if (startup && startup->self->IsRun())
    {
        if (startup->self->ID() == GetCurrentThreadId())
        {
            TMSG msg;
            msg.Init(m_hWnd, Msg, wParam, lParam, arg, true);
            msg.sig.si_signo = SIG_SENDMESSAGE;
            MessageProc(msg);
        } else
        {
            union sigval sval;
//        sval.sival_ptr = this;
            MPThread::Startup *_startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);


            if (!Lock()) return 0;
            _startup->start->Enter();

            QMess *val = (QMess *) HMALLOC(sizeof(QMess));
            val->Init(m_hWnd, Msg, wParam, lParam, arg);
            sval.sival_ptr = val;

            _startup->DataEx = &fRet;
            if (!StdError(pthread_sigqueue(startup->self->ID(), SIG_SENDMESSAGE, sval)))
            {
                char a = 0;
                send(sockMsg[1], &a, 1, 0);
                _startup->start->Wait();
                _startup->message.Init(m_hWnd, Msg, wParam, lParam, arg, true);
                _startup->start->SetEvent();
                _startup->start->Wait();
            }
            _startup->start->Leave();
            UnLock();
        }
    }
    return fRet != 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT EpollThread::PostMessage(MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (sockMsg[1] == -1) return 0;
    if (!startup) return 0;
    LRESULT ret = 0;
    MSGThread::PostMessage(Msg, wParam, lParam, arg);
    if (!Lock()) return 0;
    //startup->DataEx = &ret;
    char a = 0;
    send(sockMsg[1], &a, 1, MSG_DONTWAIT);
    UnLock();
    return ret;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void EpollThread::Run()
{
    Init();

    if (StdError(socketpair(AF_UNIX, SOCK_DGRAM, 0, sockMsg))) return;
    epoll_event events[EPOOL_SIZE];
    StdError(fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) | O_NONBLOCK));
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLERR;
    ev.data.fd = sockMsg[0];
    StdError(epoll_ctl(epfd, EPOLL_CTL_ADD, sockMsg[0], &ev));
    while (1)
    {
        bzero(events, EPOOL_SIZE * sizeof(epoll_event));
        TMSG m;

        int count = epoll_wait(epfd, events, EPOOL_SIZE, -1);
        if (count < 0)
        {
            if (errno != EINTR)
                StdError(-1);
            else
                errno = 0;
            continue;

        } else
            for (int i = 0; i < count; ++i)
            {

                if (events[i].data.fd == sockMsg[0])
                {
                    char a;
                    recv(sockMsg[0], &a, 1, 0);
                    
                    while (PeekMessage(&m, TRUE))
                    {
                        TranslateMessage(&m);
                        DispatchMessage(&m);
                    }


                } else
                {
                    if (!data.Lookup(events[i].data.fd))
                        continue;

                    SockMsg &sm = data[events[i].data.fd];
                    GHWND hWnd = sm.hWnd;
                    MSGTYPE Msg = sm.Msg;
                    LPARAM lParam = 0;
                    MSGThread *wnd = sm.hThread;
                    if (!wnd)
                    {
                        continue;
                    }

                    if (events[i].events & EPOLLIN)
                    {
                        if (sm.mask & FD_ACCEPT)
                            lParam = ((events[i].events & EPOLLERR ? FD_ACCEPT : 0) << 16) |
                                     FD_ACCEPT;
                        else if ((sm.mask & FD_CONNECT) && (events[i].events & EPOLLERR))
                            lParam = ((events[i].events & EPOLLERR ? FD_CONNECT : 0) << 16) |
                                     FD_CONNECT;
                        else
                            lParam = ((events[i].events & EPOLLERR ? FD_READ : 0) << 16) | FD_READ;

                    } else if (events[i].events & EPOLLOUT)
                        lParam = ((events[i].events & EPOLLERR ? FD_WRITE : 0) << 16) | FD_WRITE;
                    else if (events[i].events & EPOLLRDHUP)
                    {
                        lParam = ((events[i].events & EPOLLERR ? FD_CLOSE : 0) << 16) | FD_CLOSE;
                        data.Delete(events[i].data.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
                    }
                    //  if (sm.fSink)
                    // wnd->SendMessage(hWnd, Msg, (LPARAM)events[i].data.fd, lParam);
                    //else
                    if (m_privateHash->Find(wnd))
                        SendPostMessage(wnd, hWnd, Msg, (LPARAM) events[i].data.fd, lParam);
                    else
                    {
                        int fd = events[i].data.fd;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
                        data.Delete((int) fd);

                    }


                }
            }
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT EpollThread::OnMessage(TMSG &msg)
{
    struct epoll_event ev;
    switch (msg.message)
    {
        case WM_USER + MSG_ADDSOCK:
        {
            if (!(int) msg.wParam)
                return 0;

            int ctrl = EPOLL_CTL_ADD;
            if (data.Lookup((int) msg.wParam))
                ctrl = EPOLL_CTL_MOD;
            else
                fcntl((int) msg.wParam, F_SETFL, fcntl((int) msg.wParam, F_GETFD, 0) | O_NONBLOCK);


            SockMsg &sm = data[msg.wParam];
            sm = *(SockMsg *) msg.arg;
            sm.fSink = 0;
            delete (SockMsg *) msg.arg;
            if (msg.lParam & FD_ACCEPT)
#ifdef ANDROID_NDK
                ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP |
                            EPOLLHUP | EPOLLERR | EPOLLMSG;
#else
                ev.events = EPOLLIN | EPOLLET |   EPOLLRDHUP |   EPOLLHUP | EPOLLERR | EPOLLMSG;
#endif
            else
                ev.events =
                        EPOLLOUT | EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR | EPOLLMSG;
            ev.data.fd = (int) msg.wParam;
            if (StdError(epoll_ctl(epfd, ctrl, (int) msg.wParam, &ev)) == -1)
            {
                data.Delete((int) msg.wParam);
                return -1;
            }
            return 0;
        }
        case WM_USER + MSG_DELSOCK:
            if (data.Lookup((int) msg.wParam))
            {
                epoll_ctl(epfd, EPOLL_CTL_DEL, (int) msg.wParam, 0);
                int l = data.Len();
                data.Delete((int) msg.wParam);

                return 0;
            }
            break;
        default:
            if (msg.ghWnd)
                return WindowProc(msg.ghWnd, msg.message, msg.wParam, msg.lParam);
            return WindowProc(msg.iwnd, msg.message, msg.wParam, msg.lParam);


    }
    return 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
struct stGetHostByName {
    MSGThread *obj;
    GHWND hWnd;
    MSGTYPE wMsg;
    char *name;
    char *buf;
    int buflen;

};

//-----------------------------------------------------
//
//-----------------------------------------------------
void ThreadGetHostByName(void *p)
{
    stGetHostByName *st = (stGetHostByName *) p;
    MSGThread *obj = st->obj;
    //MSGTYPE wMsg = st->wMsg;
    //const char *name = st->name;
//	char *buf = st->buf;
//	int buflen = st->buflen;
//        GHWND hWnd = st->hWnd;
    hostent *hp = 0, *h;
    int herr;
    int res = 0;

    h = (hostent *) st->buf;

    res = gethostbyname_r(st->name, h, st->buf + sizeof(hostent), st->buflen - sizeof(hostent), &hp,
                          &herr);


    if (!res && !hp)
        res = herr;
    obj->PostMessage(st->hWnd, st->wMsg, 0, (res << 16) | st->buflen);
    HFREE(st->name);
    HFREE(st);
}

//-------------------------------------
//
//-------------------------------------
HANDLE WSAAsyncGetHostByName(MSGThread *obj, GHWND hWnd, MSGTYPE wMsg, const char *name, char *buf,
                             int buflen)
{
    stGetHostByName *st = (stGetHostByName *) HMALLOC(sizeof(stGetHostByName));
    st->obj = obj;
    st->hWnd = hWnd;
    st->wMsg = wMsg;
    st->name = (char *) HMALLOC(strlen(name) + 1);
    strcpy(st->name, name);

    st->buf = buf;
    st->buflen = buflen;

    MPThread *Thread = MBeginThread((MPThread::StartThreadRoutine) ThreadGetHostByName, st, TRUE);
    if (Thread)
    {
        Thread->m_bAutoDelete = TRUE;
        Thread->ResumeThread();
        return (HANDLE) Thread->ID();
    }
    return 0;
}
//-------------------------------------
//
//-------------------------------------
extern "C++" HANDLE
WSAAsyncGetHostByName(GHWND hWnd, MSGTYPE wMsg, const char *name, char *buf, int buflen)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (startup && startup->self->IsRun() && startup->MessageProc)
        return WSAAsyncGetHostByName(dynamic_cast<MSGThread *>(startup->MessageProc), hWnd, wMsg,
                                     name, buf, buflen);
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" HANDLE
WSAAsyncGetHostByName(MSGThread *obj, MSGTYPE wMsg, const char *name, char *buf, int buflen)
{
    return WSAAsyncGetHostByName(obj, 0, wMsg, name, buf, buflen);

}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" void WSACancelAsyncRequest(HANDLE hAsyncTaskHandle)
{
#ifndef ANDROID_NDK
    pthread_cancel((int)hAsyncTaskHandle);
#else
    pthread_kill((int) hAsyncTaskHandle, SIGKILL);
#endif
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT
WSAAsyncSelect(SOCKET s, GHWND hWnd, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{

    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (startup && pollTh && startup->self->IsRun() && pollTh && startup->MessageProc)
        return pollTh->PostMessage(WM_USER + MSG_ADDSOCK, s, lEvent,
                                   new EpollThread::SockMsg((int) s, (UINT) lEvent, false, Msg,
                                                            startup->MessageProc, hWnd));
    return 0;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT WSASelect(SOCKET s, GHWND hWnd, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (startup && pollTh && startup->self->IsRun() && pollTh && startup->MessageProc)
        return pollTh->SendMessage(WM_USER + MSG_ADDSOCK, s, lEvent,
                                   new EpollThread::SockMsg((int) s, (UINT) lEvent, true, Msg,
                                                            startup->MessageProc, hWnd));
    return 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT
WSAAsyncSelect(SOCKET s, MSGThread *hWnd, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{
    if (!s || !m_privateHash->Find(hWnd) || !pollTh)
    {
        return -1;
    }

    return pollTh ? pollTh->PostMessage(WM_USER + MSG_ADDSOCK, s, lEvent,
                                        new EpollThread::SockMsg((int) s, (UINT) lEvent, false, Msg,
                                                                 hWnd, 0)) : 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT WSACancel(SOCKET s, MSGThread *pollTh)
{
    if (!s || !pollTh)
    {
        return -1;
    }
    return pollTh->SendMessage(WM_USER + MSG_DELSOCK, s, 0);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT
WSASelect(SOCKET s, MSGThread *hWnd, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{
    if (!s || !m_privateHash->Find(hWnd) || !pollTh)
    {
        return -1;
    }
    return pollTh->SendMessage(WM_USER + MSG_ADDSOCK, s, lEvent,
                               new EpollThread::SockMsg((int) s, (UINT) lEvent, true, Msg, hWnd,
                                                        0));
}

//-----------------------------------------------------
//
//-----------------------------------------------------

void ReadAllSignals(MPThread::Startup *startup)
{
    TMSG msg;
    while (startup->Pool->Len())
    {
        if (privateGetMessage(&msg, 0))
        {

            if (privateGetMessage(&msg))
                startup->fifo->Push(msg);
        } else
            break;
    }

}

int CheckEvents(MPThread::Startup *startup, MHashMap<int, MSGTYPE> &sSet, TMSG &res)
{
    ReadAllSignals(startup);
    int ret = -1;
    if (startup->fifo->Len())
    {
        for (int i = 0, l = startup->fifo->Len(); i < l; ++i)
        {
            res = (*startup->fifo)[i];
            if (sSet.Lookup(res.message))
            {
                startup->fifo->Delete(i);
                int imsg = res.message;
                ret = sSet[res.message];
                sSet.Delete(imsg);
                break;
            }
        }
    }
    return ret;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" int
WaitForMultipleObjects(DWORD nCount, CONST MSGTYPE *pHandles, BOOL fWaitAll, DWORD iTime)
{
    int ret = WAIT_FAILED;

    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup)
    {
        return WAIT_FAILED;
    }
    MFifo<TMSG> fifo;
    if (((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects))) return WAIT_FAILED;
    TlsSetValue(MPThread::dwWaitForMultipleObjects, (void *) 1);
    TMSG msg;
    MHashMap<int, MSGTYPE> sSet;
    for (DWORD i = 0; i < nCount; ++i)
        sSet[pHandles[i]] = i;
    int error = 0;
    DWORD curTick = GetTickCount();
    while (!error)
    {
        startup->critFifo->Enter();
        ret = CheckEvents(startup, sSet, msg);
        startup->critFifo->Leave();
        if (ret != -1)
        {
            fifo.Push(msg);
            if (!fWaitAll || !sSet.Len())
                goto _exit_func;
            else
                continue;
        }

        /****************************************/
        if ((UINT) iTime == INFINITE)
        {
            if ((error = startup->events->Wait()))
            {
                ret = WAIT_FAILED;
                StdError(0);
                break;
            }
        } else
        {
            if ((error = startup->events->WaitTimeout(iTime)))
                break;
            else if (GetTickCount() - curTick > iTime)
            {
                error = ETIMEDOUT;
                break;
            }
        }


    }
    _exit_func:
    TlsSetValue(MPThread::dwWaitForMultipleObjects, 0);
    MFifo<TMSG>::iterator it(fifo);
    for (TMSG *p = it.begin(); p; p = it.next())
    {
        privateTranslateMessage(p);
        privateDispatchMessage(p);
    }
    switch (error)
    {
        case EINVAL:
            printf("The value specified by cond or mutex is invalid\r\n");
            ret = WAIT_FAILED;
            break;
        case EPERM:
            printf("The mutex was not owned by the current thread at the time of the call\r\n");
            ret = WAIT_FAILED;
            break;
        case ETIMEDOUT:
            ret = WAIT_TIMEOUT;
            break;
    }
    return ret;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" int WaitForSingleObject(MSGTYPE iMsg, DWORD64 iTime)
{
    return WaitForMultipleObjects(1, &iMsg, 1, iTime);
}
//-----------------------------------------------------
extern "C++" BOOL WaitForSingeMessage(TMSG *msg, MSGTYPE iMsg, BOOL fGUIIdle)
{
    return WaitForSingleObject(iMsg, INFINITE);

}
/*
//-----------------------------------------------------
//
//-----------------------------------------------------
void PthreadOnDestroy(int signum, siginfo_t *info, void *context)
{
    MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    	if (startup && startup->self->IsRun())

    {
        if (startup->fMainThread)
            exit(1);
        else
                pthread_exit(0);
    }
}
*/

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL IsWindow(THWND h)
{
    int status = 0;
    if (cast2MSGThread(h) != 0)
        return 1;
    TString test;
    tagTHWND *p = (tagTHWND *) &h;
    if (!p->iPid || p->iPid == getpid()) return 0;
    pid_t pid = waitpid(p->iPid, &status, WNOHANG);
    if (kill(p->iPid, 0) != 0) return 0;

    test.Format("/tmp/PLAPP%u", (UINT) p->iPid);
    struct stat st;
    if (lstat(test.GetPtr(), &st) == -1) return 0;
    return 1;

}
#if(1) //ndef ANDROID_NDK

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL SetTimer(MSGThread *obj, UINT iTimerID, UINT uElapse, TimerProc func)
{

    if (m_privateHash->Find(obj))
        return obj->SetTimer(iTimerID, uElapse, func);
    return 0;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL KillTimer(MSGThread *obj, UINT iTimerID)
{
    if (m_privateHash->Find(obj))
        return obj->KillTimer(iTimerID);
    return 0;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL SetTimer(THWND th, UINT iTimerID, UINT uElapse, TimerProc func)
{

    MSGThread *obj = 0;
    if (m_privateHash->Find(th, &obj))
        return obj->SetTimer(iTimerID, uElapse, func);
    return 0;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL KillTimer(THWND th, UINT iTimerID)
{
    MSGThread *obj = 0;
    if (m_privateHash->Find(th, &obj))
        return obj->KillTimer(iTimerID);
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL SetTimer(GHWND wnd, UINT iTimerID, UINT uElapse, TimerProc func)
{
    MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    if (!iTimerID || !startup) return 0;
#ifndef _CONSOLE_PROG_
    CWnd *pw = 0;
    if (pw = CWnd::FromHandle(wnd))
    {
        pw->SetTimer(iTimerID, uElapse);
        return TRUE;
    }
#endif
    MSGThread *th;
    if (startup && (th = startup->MessageProc))
        return th->SetTimer(iTimerID, uElapse, func);
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL KillTimer(GHWND wnd, UINT iTimerID)
{
   MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
#ifndef _CONSOLE_PROG_
    CWnd *pw = 0;
    if (pw = CWnd::FromHandle(wnd))
    {
        pw->KillTimer(iTimerID);
        return 1;
    }
#endif
    MSGThread *th;
    if (startup && (th = startup->MessageProc))
        return th->KillTimer(iTimerID);
    return 0;
}
#else
extern "C++" BOOL SetTimer(GHWND wnd, UINT iTimerID, UINT uElapse, TimerProc func)
{
    return true;
}
extern "C++" BOOL KillTimer(GHWND wnd, UINT iTimerID)
{
    return true;
}

#endif
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" pthread_t GetWindowThreadProcessId(THWND hWnd, DWORD *lpdwProcessId)
{
    if (lpdwProcessId) *lpdwProcessId = 0;

    if (!hWnd) return 0;
    tagTHWND *p = (tagTHWND *) &hWnd;
    if (!p->iPid) return 0;
    int status;

    MSGThread *proc = cast2MSGThread(hWnd);
    if (proc)
    {
        if (lpdwProcessId)
            *lpdwProcessId = p->iPid;
        return proc->Self() ? proc->Self()->Handle() : 0;
    } else if (lpdwProcessId)
        if (kill(p->iPid, 0) == 0)
        {
            *lpdwProcessId = p->iPid;
            return p->iThread;
        }
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT
PostMessage(GHWND ghWnd, MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (!m_privateHash->Find(obj)) return -1;
    return obj->PostMessage(ghWnd, Msg, wParam, lParam, arg);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT
SendMessage(GHWND ghWnd, MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    if (!m_privateHash->Find(obj)) return -1;
    return obj->SendMessage(ghWnd, Msg, wParam, lParam, arg);

}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT PostMessage(THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    MSGThread *p = cast2MSGThread(obj);
    if (p)
        return PostMessage(p, Msg, wParam, lParam, arg);
    else
    {
        tagTHWND *tag = (tagTHWND *) &obj;
        if (tag->iPid && tag->iPid != getpid())
            return (LRESULT) ProcCon::_SendMessage(tag->iPid, (DWORD64) obj, Msg, wParam, lParam,
                                                   false);
    }
    return -1;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" LRESULT SendMessage(THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg)
{
    MSGThread *p = cast2MSGThread(obj);
    if (p)
        return SendMessage(p, Msg, wParam, lParam, arg);
    else
    {
        tagTHWND *tag = (tagTHWND *) &obj;
        if (tag->iPid && tag->iPid != getpid())
            return (LRESULT) ProcCon::_SendMessage(tag->iPid, (DWORD64) obj, Msg, wParam, lParam,
                                                   true, arg ? (UINT *)arg : 0);

    }
    return -1;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool ProcCon::Register(const char *s)
{
    InitMsg();
    struct sockaddr_un local;
    if (1)
    {
        s = "PLAPP";
        sprintf(local.sun_path, "/tmp/%s%u", s, (UINT) getpid());
    } else
        sprintf(local.sun_path, "/tmp/%s%u", s, (UINT) getpid());
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        StdError(-1);
        return 0;
    }
    local.sun_family = AF_UNIX;

    unlink(local.sun_path);
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    int r = ::bind(sockfd, (struct sockaddr *) &local, len);
    if (StdError(r) == -1)
    {

        close(sockfd);
        sockfd = 0;
        return 0;
    }

    if (StdError(listen(sockfd, 5)) == -1)
    {
        close(sockfd);
        sockfd = 0;
        return 0;
    }
    WSAAsyncSelect(sockfd, this, WM_USER + SCK_EVENT, FD_ACCEPT);
    return 1;
};

void ProcCon::Free()
{
    if (sockfd > 0)
    {
        WSACancel(sockfd);
        close(sockfd);
    }
    sockfd = -1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
ProcCon::~ProcCon()
{
    if (sockfd > 0)
    {
        WSACancel(sockfd);
        close(sockfd);
    }
}


DWORD64 ProcCon::_SendMessage(pid_t proc, DWORD64 thread, DWORD64 Msg, DWORD64 wParam, DWORD64 lParam,
                      bool fSend, UINT *iTimeout/* = 0*/)
{
    DWORD64 res;

    if (!kill(proc, 0))
    {

        DWORD_PTR *p = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
        MDeq<ConProcSendMsg> *dec = (MDeq<ConProcSendMsg> *) p[1];
        if (dec)
            for (int i = 0, l = dec->Len(); i < l; ++i)
            {
                if ((*dec)[i].pid == proc)
                {
                    printf("rolback ProcCon::_sendmessage\r\n");
                    //   ASSERT(0);
                    //  return 0;
                    break;
                }
            }


        SOCKET fd;
        struct sockaddr_un local;
        struct stat st;
        sprintf(local.sun_path, "/tmp/PLAPP%u", (UINT) proc);
        if (lstat(local.sun_path, &st) == -1) return 0;

        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            StdError(-1);
            return -1;
        }
        local.sun_family = AF_UNIX;
        int len = strlen(local.sun_path) + sizeof(local.sun_family);
        if (StdError(connect(fd, (struct sockaddr *) &local, len)) != -1)
        {

            ConMsg mess;
            char *buf = (char *) &mess;
            mess.Set(getpid(), thread, Msg, wParam, lParam, fSend);
            len = sizeof(ConMsg);
#if (0)
            {
            UINT mid = MakeTmpId();
            MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
            MSGThread *self = startup->MessageProc;
            StdError(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK));
            WSASelect((SOCKET)fd, self, mid, FD_READ | FD_CLOSE);
            send(fd, buf, len, 0); // send(fd, buf, len));
            TlsSetValue(MPThread::dwWaitForMultipleObjects, (const void *)1);
            TMSG msg;
            int count = 0;
            if (startup->fMainThread)
                AfxGetApp()->fBlocking = 1;
            self->SetTimer(mid, 100);
            while (1) //count < 600)
            {
                privateGetMessage(&msg, TRUE);
                    struct stat st;
                    if (fstat(fd, &st ) != 0)
                        break;
                int r = 0;

                if (self->m_hWnd == msg.iwnd)
                {

                    if (msg.message == mid) // && msg.lParam == FD_READ || msg.lParam == FD_CLOSE)
                    {
                        label:

                        len = recv(fd, (char *)&res + r, sizeof(DWORD64), 0);
                        if ((len  + r) == sizeof(DWORD64))
                        {
                            len = sizeof(DWORD64);
                            break;
                        }
                        else
                        if (len < 0)
                        {
                            int rr = errno;
                            if (rr == EAGAIN || rr == EINTR)
                                continue;

                            StdError(-1);
                            break;
                        }
                        else
                        {
                            r += len;
                            goto label;
                         }
                    }
                    else
                    if (msg.message == WM_TIMER)
                    {
                        ++count;
                        if (recv(fd, (char *)&res + r, sizeof(DWORD64),MSG_DONTWAIT | MSG_PEEK) > 0)
                            goto label;
                       // if (count == 4)
                       // {
                       // printf ("Timeout SendMessage\r\n");
                       // break;

                            if (startup->fMainThread)
                                AfxGetApp()->GUIIdle();


                    }
                }
                else
                {
                    privateTranslateMessage(&msg);
                    privateDispatchMessage(&msg);
                }
            }

            WSACancel((SOCKET)fd);

            close(fd);

            TlsSetValue(MPThread::dwWaitForMultipleObjects, 0);

            return len == sizeof(DWORD64) ? res : 0;
         }
        else
#endif
            {
            int err = 0; 
            socklen_t errlen;
            TString sserr;
            if (iTimeout)
            {
                    struct timeval timeout;      
                    timeout.tv_sec = (*iTimeout)/1000;
                    if (!timeout.tv_sec)
                        timeout.tv_sec = 1;
                    timeout.tv_usec = 0;

                if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                            sizeof(timeout)) < 0)
                {
                    errlen = sizeof(err);        
                    err = 0;
                    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *) &err,  &errlen) == 0)
                        sserr.Format("socket error = %s ( %d (%d) )\n", strerror(err),  err, errno);
                    errno = 0;
                }

                if (setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, &timeout,   sizeof(timeout)) < 0)
                {
                    errlen = sizeof(err);        
                    err = 0;
                    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *) &err,  &errlen) == 0)
                        sserr.Format("socket error = %s ( %d (%d) )\n", strerror(err),  err, errno);
                    errno = 0;
                }
            }
                    int ret = send(fd, buf, len, 0);
                    if (ret == len)
                    {
                        len = recv(fd, &res, sizeof(DWORD64), 0);
                    }
                    else
                        len = 0;

                    close(fd);
                    ::GUIIdle();
                    return len == sizeof(DWORD64) ? res : 0;
                }
            }

        }
    return -1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LRESULT ProcCon::OnMessage(TMSG &msg)
{
    if (msg.message == WM_USER + SCK_EVENT)
    {
        switch (msg.lParam)
        {
            case FD_ACCEPT:
            {
                //          WSACancel(sockfd);
                sockaddr_in their_addr;
                socklen_t socklen = sizeof(struct sockaddr_in);

                SOCKET client = accept(sockfd, (struct sockaddr *) &their_addr, &socklen);
                if (client > 0)
                {
                    int len = sizeof(ConMsg);
                    ConMsg mess;
                    char *buf = (char *) &mess;
                    while (len > 0)
                    {
                        int res = StdError(recv(client, buf, len, 0));
                        if (res <= 0) break;
                        len -= res;
                        buf += res;
                    }
                    if (!len)
                    {
                        DWORD64 res = 0;
                        MSGThread *proc = cast2MSGThread(mess.thread);
                        tagTHWND *tag = (tagTHWND *) &mess.thread;
                        if (!proc)
                        {
                            ASSERT(0);
                        } else
                        {
                            if (!mess.fSend)
                                res = proc->PostMessage((MSGTYPE) mess.Msg, (WPARAM) mess.wParam,
                                                        (LPARAM) mess.lParam);
                            else
                            {
                                union sigval sval;
                                QMess *val = (QMess *) HMALLOC(sizeof(QMess));
                                val->Init(0, proc->m_hWnd, (MSGTYPE) mess.Msg, (WPARAM) mess.wParam,
                                          (LPARAM) mess.lParam, (void *) (size_t) client);
                                val->privat = (void *) (size_t) mess.pid;
                                sval.sival_ptr = val;
                                if (!StdError(
                                        proc->pthread_sigqueue(proc->Self()->ID(), SIG_SOCK_SEND,
                                                               sval)))
                                {
                                    GUIIdle();

                                    //                WSAAsyncSelect(sockfd, this, WM_USER + SCK_EVENT, FD_ACCEPT);
                                    return 0;


                                } else
                                {
                                    StdError(-1);
                                    HFREE(val);
                                }
                            }

                        }
                        StdError(send(client, &res, sizeof(DWORD64), 0));

                    }
                    close(client);
                } else
                    StdError(-1);

            }
                break;
        }
    }

    return 0;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL InSendMessage()
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (startup)
    {
        DWORD_PTR *l = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
        return l[1] != 0;
    }
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" int WSAGetLastError()
{
    int err = errno;
    if (err)
        StdError(-1);
    switch (err)
    {
        case EINTR:
            return WSAEINTR;
        case EBADF:
            return WSAEBADF;
        case EPERM:
        case EACCES:
            return WSAEACCES;
        case EFAULT:
            return WSAEFAULT;
        case EINVAL:
            return WSAEINVAL;
        case EMFILE:
            return WSAEMFILE;
        case EWOULDBLOCK:
            return WSAEWOULDBLOCK;
        case EINPROGRESS:
            return WSAEINPROGRESS;
        case EALREADY:
            return WSAEALREADY;
        case ENOTSOCK:
            return WSAENOTSOCK;
        case EDESTADDRREQ:
            return WSAEDESTADDRREQ;
        case EMSGSIZE:
            return WSAEMSGSIZE;
        case EPROTOTYPE:
            return WSAEPROTOTYPE;
        case ENOPROTOOPT:
            return WSAENOPROTOOPT;
        case EPROTONOSUPPORT:
            return WSAEPROTONOSUPPORT;
        case ESOCKTNOSUPPORT:
            return WSAESOCKTNOSUPPORT;
        case EOPNOTSUPP:
            return WSAEOPNOTSUPP;
        case EPFNOSUPPORT:
            return WSAEPFNOSUPPORT;
        case EAFNOSUPPORT:
            return WSAEAFNOSUPPORT;
        case EADDRINUSE:
            return WSAEADDRINUSE;
        case EADDRNOTAVAIL:
            return WSAEADDRNOTAVAIL;
        case ENETDOWN:
            return WSAENETDOWN;
        case ENETUNREACH:
            return WSAENETUNREACH;
        case ENETRESET:
            return WSAENETRESET;
        case ECONNABORTED:
            return WSAECONNABORTED;
        case EPIPE:
        case ECONNRESET:
            return WSAECONNRESET;
        case ENOBUFS:
            return WSAENOBUFS;
        case EISCONN:
            return WSAEISCONN;
        case ENOTCONN:
            return WSAENOTCONN;
        case ESHUTDOWN:
            return WSAESHUTDOWN;
        case ETOOMANYREFS:
            return WSAETOOMANYREFS;
        case ETIMEDOUT:
            return WSAETIMEDOUT;
        case ECONNREFUSED:
            return WSAECONNREFUSED;
        case ELOOP:
            return WSAELOOP;
        case ENAMETOOLONG:
            return WSAENAMETOOLONG;
        case EHOSTDOWN:
            return WSAEHOSTDOWN;
        case EHOSTUNREACH:
            return WSAEHOSTUNREACH;
        case ENOTEMPTY:
            return WSAENOTEMPTY;
#ifdef EPROCLIM
            case EPROCLIM:          return WSAEPROCLIM;
#endif
#ifdef EUSERS
        case EUSERS:
            return WSAEUSERS;
#endif
#ifdef EDQUOT
        case EDQUOT:
            return WSAEDQUOT;
#endif
#ifdef ESTALE
        case ESTALE:
            return WSAESTALE;
#endif
#ifdef EREMOTE
        case EREMOTE:
            return WSAEREMOTE;
#endif
        default:
            if (err) return WSAEFAULT;
    }

    return err;
}

#endif
//-----------------------------------------------------
//
//-----------------------------------------------------
struct FileChangeNotification {
    TString dir;
    UINT param;
    int handle;
    MSGTYPE iMsg;
};

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL FindCloseChangeNotification(HANDLE handle)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
    if (startup->iNotifyHandle && handle)
    {
        FileChangeNotification *fn = (FileChangeNotification *) handle;
        int ret = inotify_rm_watch(startup->iNotifyHandle, fn->handle);

        if (ret == 0)
        {

            delete fn;
            return TRUE;
        }
    }
    return FALSE;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" BOOL FindNextChangeNotification(HANDLE handle)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
    if (startup->iNotifyHandle && handle)
    {
        FileChangeNotification *fn = (FileChangeNotification *) handle;
        int len = NAME_MAX + sizeof(inotify_event);
        inotify_event *n = (inotify_event *) alloca(len);
        int N = 0, k = 0;
        MString sn;
        memset(n, 0, len);
        WSACancel((SOCKET) startup->iNotifyHandle);
        int rd = read(startup->iNotifyHandle, n, len);
        while (rd > 0)
        {
            sn = n[k].name;

            if (!(n[k].mask & IN_IGNORED) && sn.StrLen())
            {
                ++N;

            }


            int nn = strlen(n->name) / sizeof(inotify_event) +
                     (strlen(n->name) % sizeof(inotify_event) != 0) + 1;
            rd -= sizeof(inotify_event) * nn;
            k += nn;

        }

        fn->handle = inotify_add_watch(startup->iNotifyHandle, fn->dir.GetPtr(),
                                       fn->param);
        WSAAsyncSelect((SOCKET) startup->iNotifyHandle,
                       dynamic_cast<MSGThread *>(startup->MessageProc), fn->iMsg, FD_ACCEPT);
        return N > 0;
    }
    return FALSE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern "C++" HANDLE FindFirstChangeNotification(LPCSTR lpPathName, BOOL, DWORD dwNotifyFilter)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
    if (!startup->iNotifyHandle)
        startup->iNotifyHandle = inotify_init();
    if (startup->iNotifyHandle)
    {
        WSACancel((SOCKET) startup->iNotifyHandle);
        int handle = inotify_add_watch(startup->iNotifyHandle, lpPathName,
                                       dwNotifyFilter);
        if (handle > 0)
        {
            FileChangeNotification *fn = new FileChangeNotification;
            fn->handle = handle;
            fn->dir = lpPathName;
            fn->param = dwNotifyFilter;
            fn->iMsg = (MSGTYPE) fn;
            WSAAsyncSelect((SOCKET) startup->iNotifyHandle,
                           dynamic_cast<MSGThread *>(startup->MessageProc), fn->iMsg, FD_ACCEPT);
            return (HANDLE) fn;
        }
    }
    return 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
extern void OnEndChildProcess(int status, pid_t pid);

BOOL privateDispatchMessage(TMSG *msg)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    MSGThread *proc = cast2MSGThread(msg->iwnd);
    if (msg->sig.si_signo == SIGCHLD)
    {
        int status;
        pid_t pid = waitpid(msg->sig.si_pid, &status, WNOHANG);
        if (pid != -1)
            OnEndChildProcess(status, pid);

    } else if (startup && startup->self->IsRun() && startup->MessageProc)
    {
        DWORD_PTR *ptr = (DWORD_PTR *) TlsGetValue(MPThread::dwMessageWaitBusy);
        if (proc && msg->sig.si_signo == SIG_SOCK_SEND)
        {
            MDeq<ConProcSendMsg> *dec = (MDeq<ConProcSendMsg> *) ptr[1];
            if (!dec)
            {
                dec = new MDeq<ConProcSendMsg>;
                ptr[1] = (DWORD_PTR) dec;
            }
            BOOL fFreeSendedThread = FALSE;
            dec->PushFront().Set((int) msg->iarg, &fFreeSendedThread, msg->privat);
            LRESULT rs = proc->MessageProc(*msg, 0);
            if (!fFreeSendedThread && (dec = (MDeq<ConProcSendMsg> *) ptr[1]))
            {
                DWORD64 res = rs;
                struct stat st;
                if (fstat((int) msg->iarg, &st) == 0)
                {
                    StdError(send((int) msg->iarg, &res, sizeof(DWORD64), 0));
                    close((int) msg->iarg);

                } else
                {
                    StdError(-1);
                }
                dec->PopFront();
                if (!dec->Len())
                {
                    delete dec;
                    ptr[1] = 0;
                }

            }
            return msg->sig.si_signo;
        }

        if (proc &&
            (msg->sig.si_signo == SIG_SENDMESSAGE || msg->sig.si_signo == SIG_SENDMESSAGE_WND))
        {

            MSGThread::WaitMessage(msg);
            return msg->sig.si_signo;
        } else if (proc && (msg->sig.si_signo == SIG_POSTMESSAGE ||
                            msg->sig.si_signo == SIG_POSTMESSAGE_WND))
        {
            if (!proc) return 0;
            proc->MessageProc(*msg);
            return msg->sig.si_signo;
        } else
        {

            if (!proc) return 0;
            for (int i = 0; i < NUM_SIG; ++i)
            {
                if (mTransMsg[i] == msg->sig.si_signo)
                {

                    proc->MessageProc(*msg);
                    return msg->sig.si_signo;
                }
            }

        }
    }
    return 0;

}

extern "C++" BOOL DispatchMessage(TMSG *msg)
{

    LRESULT ret = ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0
                                                                              : privateDispatchMessage(
                    msg);

    return ret;

}

static int SigWait(const sigset_t *set,
                   siginfo_t *sig)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);

    //  struct timespec timeout;
    // size_t milisecond = 500;
    // timeout.tv_nsec = milisecond * 1e6;
    // timeout.tv_sec = 0;
    // int n = -1;
    //if (!InSendMessage())
#if (1)
    int nsig = 0;
    startup->sign->Enter();
    startup->critFifo->Enter();
    if (startup->Pool->Len())
    {
        QMess *msg = (QMess *) startup->Pool->Pop();
        if (msg)
        {
            sig->si_value.sival_ptr = msg;
            sig->si_signo = msg->cur_sig;
            nsig = sig->si_signo;
            msg->ghWnd = 0;

        }
    }
    startup->critFifo->Leave();
    return nsig ? nsig : -1;


#else
#ifndef ANDROID_NDK
    int nsig, n = sigwaitinfo(set, sig);
    if (n >= 0)
    {
         nsig = sig->si_signo;
         n = 0;
    }
#else
    int nsig = 0;
    int n = sigwait(set, &nsig);
    if (n < 0 || nsig != SIG_ANDROID) return -1;
    startup->critFifo->Enter();
    if (startup->Pool->Len())
    {
        QMess *msg = (QMess *)startup->Pool->Pop();
        if (msg)
        {
            sig->si_value.sival_ptr = msg;
            sig->si_signo = (int)msg->ghWnd;
            nsig = sig->si_signo;
            msg->ghWnd = 0;

        }
    }
    startup->critFifo->Leave();

   // p->ghWnd = (GHWND)__signo;

#endif

    //else
      //   n = sigtimedwait(set, sig, &timeout);
    return !n ? nsig : -1 ;
#endif
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privateInitTMSG(TMSG *msg, siginfo_t &sig)
{
    int n = sig.si_signo;
    if (n == SIG_RUN_MY)
    {
        QMess *p = (QMess *) sig.si_value.sival_ptr;
        if ((((size_t) p->hwnd) >> 16) & 0xFFFF)
        {
            CopyQMsg(msg, p);
            HFREE(p);
            MPThread::StartThreadRoutine ipc = (MPThread::StartThreadRoutine) msg->hwnd;
            ipc((void *) msg->wParam);

            return 0;

        }
    } else if (n == SIG_POSTMESSAGE || n == SIG_POSTMESSAGE_WND ||
               n == SIG_SENDMESSAGE_WND || n == SIG_SENDMESSAGE || n == SIG_SOCK_SEND)
    {
        QMess *p = (QMess *) sig.si_value.sival_ptr;
        MSGThread *proc = cast2MSGThread(p->iwnd);
        if (proc)
        {
#if(1) //ndef ANDROID_NDK
            if (n ==  SIG_POSTMESSAGE && p->message == WM_TIMER)
            {
               if (proc->timer)
               {
                    MHashMap<MSGThread::mTimer, UINT>::iterator it(*proc->timer);
                    for (RBData<MSGThread::mTimer, UINT> *t = it.begin(); t; t = it.next())
                    {
                         if (p->wParam == t->value.id)
                         {
                             DWORD64 tt;
                             read(t->value.id, &tt, 8);
                             p->wParam = t->key;
                             p->lParam = GetTickCount();
                             if (t->value.func)
                             t->value.func(0, WM_TIMER, t->key, GetTickCount());
                             break;
                         }
                    }
               }


            }
#endif
            CopyQMsg(msg, p);
            HFREE(p);
            memcpy(&msg->sig, &sig, sizeof(siginfo_t));
            msg->sig.si_value.sival_ptr = msg->hwnd;

        } else
            n = -1;
        if (n == SIG_POSTMESSAGE || n == SIG_POSTMESSAGE_WND)
            if (!cast2MSGThread(msg->iwnd)) return 0;
    } else
        for (int i = 0; i < NUM_SIG; ++i)
            if (n == mTransMsg[i])
            {

                msg->message = n;
                memcpy(&msg->sig, &sig, sizeof(siginfo_t));
            }


    return n != -1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privateGetMessage(TMSG *msg, BOOL fAll)
{
    //for (int ii = 0; ii < 3; ++ii)
    {
        MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);

        if (!startup) return 0;
        memset(msg, 0, sizeof(TMSG));
        int err, n;

#ifndef ANDROID_NDK
        sigset_t &set = startup->self->ProcSignal;
#else
        sigset_t &set = startup->self->ProcSignal.self;
#endif
        siginfo_t sig;
        bzero(&sig, sizeof(siginfo_t));
        if (fAll)
            n = SigWait(&set, &sig);
        else
        {
            UXSignal set;
#ifndef ANDROID_NDK
            set.Add(SIG_SENDMESSAGE_WND);
            set.Add(SIG_POSTMESSAGE_WND);
            n = SigWait(&set, &sig);
#else
            n = SigWait(&set.self, &sig);
#endif
        }
        if (n == -1 || sig.si_signo != n)
        {
            err = errno;
#ifndef ANDROID_NDK
            pthread_testcancel();
#endif
            if (err != EINTR && err != EAGAIN && err != EINVAL && err != ETIMEDOUT)
                StdError(-1);
#if (0)
            else
             if (err != ETIMEDOUT)
                  continue;
#endif
            n = -1;

        } else
            return privateInitTMSG(msg, sig);
    }
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privateGetMessageAll(TMSG *msg, BOOL fAll)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return 0;
    if (startup->LenFifo())
    {
        *msg = startup->PopFifo();

        return TRUE;
    }
#ifdef _TEST_WX_

    if (startup->fMainThread)
        return FALSE;
#endif
    return privateGetMessage(msg, fAll);
}

extern "C++" BOOL GetMessageA(TMSG *msg, BOOL fAll)
{
    return ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0 : privateGetMessageAll(
            msg, fAll);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privatePeekMessage(TMSG *msg, THWND obj, BOOL fRemove)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup || (obj && !cast2MSGThread(obj))) return 0;
    if (startup->LenFifo())
    {
        startup->critFifo->Enter();
        for (int i = 0, l = startup->fifo->Len(); i < l; ++i)
        {
            TMSG _msg = (*startup->fifo)[i];
            if (obj && _msg.sig.si_signo == SIGCHLD)
            {
                tagTHWND *t = (tagTHWND *) &_msg.iwnd;
                tagTHWND *t2 = (tagTHWND *) &obj;
                if (t->iPid == t2->iPid)
                    _msg.iwnd = obj;

            }

            if (_msg.iwnd == obj)
            {
                if (fRemove)
                {
                    *msg = _msg;
                    startup->fifo->Delete(i);
                }
                startup->critFifo->Leave();
                return 1;
            }
        }
        startup->critFifo->Leave();
    }
    while (startup->Pool->Len())
    {
        if (privateGetMessage(msg))
        {
            if (msg->sig.si_value.sival_ptr == msg->hwnd)
            {
                if (!fRemove)
                    startup->PushFifo(*msg);
                return 1;
            } else
                startup->PushFifo(*msg);
        } else
            break;
    }

    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privatePeekMessage(TMSG *msg, BOOL fRemove)
{

    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup)
        return FALSE;
    if (startup->LenFifo())
    {
        startup->critFifo->Enter();
        *msg = fRemove ? startup->fifo->Pop() : startup->fifo->GetPtr()->value;
        startup->critFifo->Leave();

        return TRUE;
    }
    while (startup->Pool->Len())
    {
        if (privateGetMessage(msg))
        {
            if (!fRemove)
                startup->PushFifo(*msg);
            return 1;
        } else
            break;
    }

    return 0;


}

extern "C++" BOOL PeekMessage(TMSG *msg, BOOL fRemove)
{
    return ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0 : privatePeekMessage(msg,
                                                                                                fRemove);
}
int GetNumMessages()
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    return ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0 : startup->LenFifo() + startup->Pool->Len();
                                                                                                
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privatePeekMessage(TMSG *msg, GHWND filter, BOOL fRemove)
{

    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup)
        return FALSE;
    if (startup->LenFifo())
    {
        int i;
        TMSG *p;
        startup->critFifo->Enter();
        MFifo<TMSG>::iterator it(*startup->fifo);
        for (i = 0, p = it.begin(); p; p = it.next(), ++i)
        {
            if ((!filter && p->ghWnd) || filter == p->ghWnd)
            {
                *msg = *p;
                if (fRemove)
                    startup->fifo->Delete(i);
                startup->critFifo->Leave();
                return true;
            }
        }
        startup->critFifo->Leave();
        // *msg = fRemove ? startup->fifo->Pop() : startup->fifo->GetPtr()->value;
        // return TRUE;
    }
    while (startup->Pool->Len())
    {
        if (privateGetMessage(msg, 0))
        {

            if (!fRemove)
                startup->PushFifo(*msg);
            if ((!filter && msg->ghWnd) || filter == msg->ghWnd)
                return 1;
        } else
            break;
    }
    return 0;


}

extern "C++" BOOL PeekMessage(TMSG *msg, GHWND filter, BOOL fRemove)
{
    return ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0 : privatePeekMessage(msg,
                                                                                                filter,
                                                                                                fRemove);

}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL privateTranslateMessage(TMSG *msg)
{
    if (msg->sig.si_signo == 0)
        return 0;
    return 1;
}

extern "C++" BOOL TranslateMessage(TMSG *msg)
{
    return ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0
                                                                       : privateTranslateMessage(
                    msg);
}
extern "C++" void ShowWindow(THWND)
{
}
extern "C++" BOOL PeekMessage(TMSG *msg, THWND obj, int, int, BOOL fRemove)
{
    BOOL fRes = ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0
                                                                            : privatePeekMessage(
                    msg, obj, fRemove);
    return fRes;
}
extern "C++" BOOL PeekMessage(TMSG *msg, THWND obj, BOOL fRemove)
{
    BOOL fRes = ((LRESULT) TlsGetValue(MPThread::dwWaitForMultipleObjects)) ? 0
                                                                            : privatePeekMessage(
                    msg, obj, fRemove);
    return fRes;
}

extern "C++" HANDLE
WSAAsyncGetHostByName(THWND hThread, MSGTYPE wMsg, const char *name, char *buf, int buflen)
{
    return WSAAsyncGetHostByName(cast2MSGThread(hThread), wMsg, name, buf, buflen);

}
extern "C++" LRESULT
WSAAsyncSelect(SOCKET s, THWND hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{
    return WSAAsyncSelect(s, cast2MSGThread(hThread), Msg, lEvent, pollTh);
}
extern "C++" LRESULT
WSASelect(SOCKET s, THWND hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh)
{
    return WSASelect(s, cast2MSGThread(hThread), Msg, lEvent, pollTh);

}
extern "C++" BOOL GetMessageA(TMSG *msg, THWND obj, int iMin, int iMax)
{
    MPThread::Startup *startup = (MPThread::Startup *) TlsGetValue(MPThread::dwStartupPtr);
    if (!startup) return -1;
    if (obj && !cast2MSGThread(obj))
        return -1;
    MVector<TMSG> v;
    int ret = 0;
    while (ret != 1)
    {
        ret = GetMessageA(msg);
        if (ret < 1) break;
        else if (!obj && !iMin && !iMax)
            break;
        if (obj && msg->iwnd != obj)
        {
            v.Add() = *msg; //startup->PushFifo(*msg);
            ret = 0;
        } else
        {
            if (iMin > 0 && iMax > 0)
            {
                if (msg->message >= iMin && msg->message <= iMax)
                    ret = 1;
                else
                {
                    v.Add() = *msg;
                    ret = 0;
                }
            }

        }
    }
    for (int i = 0, l = v.GetLen(); i < l; ++i)
        startup->PushFifo(v[i]);
    return ret == 1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
/*
extern "C++" BOOL GetMessageA(TMSG *msg, void *obj)
{
    
    MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
 * 
    if (!startup || !m_privateHash->Find(obj)) return -1;
    while (1)
    {
        int ret = GetMessageA(msg);
        if (ret < 1) return ret;
        if (msg->hwnd != obj)
        {
            startup->PushFifo(*msg);
        }
        else
            break;
    }
    return 1;     
    
    
}
*/
timer_t mTimerID;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    static int n = 0;
    timer_t *tidp;
//	TRACE("TIMER %d\r\n", n);
    tidp = (timer_t *) si->si_value.sival_ptr;

    if (*tidp == mTimerID)
    {
        // 	TRACE("TIMER %d", n);
        ++n;
    }

}

int ProbaTimer(timer_t *timerID, int expireMS, int intervalMS)
{
    struct sigevent te;
    struct itimerspec its;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = timerHandler;
    if (StdError(sigaction(SIGUSR1, &act, 0)) != 0)
    {
        return -1;
    }
    sigprocmask(SIG_UNBLOCK, &act.sa_mask, 0);

    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL; //SIGEV_THREAD;
    te.sigev_signo = SIGUSR1;
   // te.sigev_notify_thread_id = 0;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = intervalMS * 1000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = expireMS * 1000000;
    timer_settime(*timerID, 0, &its, NULL);

    return 0;
}
EXPORTNIX void ProbaTimer()
{
//	makeTimer(&mTimerID, 500, 500 );
    struct itimerspec new_value;
    int max_exp, fd;
    struct timespec now;
    uint64_t exp, tot_exp;
    ssize_t s;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        return;

    /* Create a CLOCK_REALTIME absolute timer with initial
       expiration and interval as specified in command line */

    new_value.it_value.tv_sec = now.tv_sec + 1;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_nsec = 0;
    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
        ;
}
