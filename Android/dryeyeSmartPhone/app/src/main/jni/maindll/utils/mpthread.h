/*
 * mpthread.h
 *
 *  Created on: Mar 25, 2012
 *      Author: alex
 */

#ifndef MPTHREAD_H_
#define MPTHREAD_H_
#include "critical.h"
#ifdef __PLUK_LINUX__
#include "mtree.h"
#include "mstack.h"
#endif
#ifdef _CONSOLE_PROG_
	#ifdef ANDROID_NDK
            #include "AWnd.h"
            extern "C" void pthread_yield();
#else
            typedef tagPTR * GHWND; 
    #endif

#else
	#ifndef ANDROID_NDK
		#include "wx_def.h"
                
    #endif
#endif

#include "critical.h"
typedef HANDLE MSGTYPE;
#pragma pack(push)
#pragma pack(8)

EXPORTNIX struct QMess {
    union {
    void *hwnd;
    THWND iwnd;
    };
    GHWND ghWnd;
    MSGTYPE    message;
    WPARAM      wParam;
    LPARAM      lParam;
    bool fIsSend;
    union {
    void *arg;
    size_t iarg;
    };
    void *privat;
    int cur_sig;
    QMess() {cur_sig = 0;  fIsSend = false; privat = 0; ghWnd = 0; hwnd = 0; message = 0; wParam = lParam = 0; arg = 0;}

    QMess(const QMess &m) { privat = m.privat; cur_sig = m.cur_sig; fIsSend = false; privat = m.privat; ghWnd = m.ghWnd; hwnd = m.hwnd; message = m.message; wParam = m.wParam; lParam = m.lParam; arg = m.arg;}
    QMess(THWND wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg = 0, bool fIsSend = false);
   void Init(GHWND hWnd, THWND _wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg = 0, bool _fIsSend = false);
    void Init(THWND wnd, MSGTYPE msg, WPARAM _wParam, WPARAM _lParam, void *_arg = 0, bool _fIsSend = false);
};

#ifndef __PLUK_LINUX__
	#ifndef _MT
		#define	_MT
	#endif
	#include <process.h>
	#define ExitPThread() _endthreadex(1)
	#define pthread_yield() Sleep(0)
	template <class T>
	inline bool pthread_equal(T A, T B) { return A == B; }
        struct CondEvent2 : public CriticalSection {
                CondEvent2() { InitializeConditionVariable (&cond); };
                ~CondEvent2() {	 }
                void SetEvent() { WakeConditionVariable(&cond);pthread_yield(); }
                int Wait() { return !SleepConditionVariableCS(&cond, Handle(), INFINITE); }
                int WaitTimeout(int mili) {  return !SleepConditionVariableCS(&cond, Handle(), mili); }
                CONDITION_VARIABLE cond;

        };
        typedef DWORD pthread_key_t;
#else
        #ifndef TlsAlloc
        #include "uxsignal.h"
        #define GetCurrentThreadId pthread_self
        #define GetCurrentThread pthread_self
        #define ExitPThread() pthread_exit(0)

        #define TlsFree pthread_key_delete
	#define TlsGetValue pthread_getspecific
	#define TlsSetValue pthread_setspecific
        EXPORTNIX struct TMSG : public QMess {
            siginfo_t   sig;
            TMSG() { bzero(&sig, sizeof(siginfo_t)); }
             TMSG(const TMSG &m):QMess(m)  { memcpy(&sig, &m.sig, sizeof(siginfo_t)); }
             TMSG &operator = (const TMSG &m)  { memcpy(this, &m, sizeof(TMSG)); return *this; }
        };

        struct CondEvent2 : public CriticalSection {
                CondEvent2() { pthread_cond_init(&cond, 0); };
                ~CondEvent2() {	pthread_cond_destroy(&cond); }
                void SetEvent() { pthread_cond_signal(&cond);pthread_yield(); }
                int Wait() { return pthread_cond_wait(&cond, Handle()); }
                int WaitTimeout(int milisecond) {
                        timespec tsp;
                        timeval now;
                        gettimeofday(&now, 0);
                        tsp.tv_sec = now.tv_sec + milisecond/1000;
                        DWORD64 tmp = now.tv_usec * 1000 + (milisecond%1000) * 1000000;
                        if (tmp > 999999999)
                            tmp = 999999999;
                        tsp.tv_nsec = (INT64)tmp;
                        
                        return pthread_cond_timedwait(&cond, Handle(), &tsp);
                        }

                pthread_cond_t cond;

        };
        #endif
#endif
class MSGThread; 
EXPORTNIX class _PEXPORT MPThread
{
public:
#ifdef __PLUK_LINUX__
	typedef  void *(*StartThreadRoutine) (void *);
#else
	typedef LPTHREAD_START_ROUTINE StartThreadRoutine;
#endif
	struct Startup;
public :

struct Startup {
	MPThread *self;
	StartThreadRoutine func, CleanUp;
	void *arg, *CleanUpArgs;
	CondEvent2 *cond, *start, *events;
	QMess message;
#ifdef __PLUK_LINUX__
	int iNotifyHandle;
    MFifo<TMSG> *fifo;
//#ifdef ANDROID_NDK
    MFifo<void *> *Pool;
    void PushPool(void *msg);
    LocalSemaphore *sign;
//#endif

#endif
	CriticalSection *critEvents, *critMsg, *critFifo;
        BOOL fMainThread;
        MSGThread *MessageProc;
        void *DataEx;
        void PushFifo(TMSG &msg);
        int LenFifo();
        TMSG PopFifo();
        };
        MPThread();
	virtual ~MPThread();
	bool Attach(bool fIsMainThread = true);
	bool Detach();
	bool Start(StartThreadRoutine, void *param, bool fStartSuspend = false, StartThreadRoutine _CleanUp = 0, void *paramCleanUp = 0);
	virtual bool Stop();
	bool IsRun();
	bool Wait();
	static void Idle();
	static void TimeEndPeriod(UINT uPeriod);
	static void TimeBeginPeriod(UINT uPeriod);
	static void Sleep(UINT uPeriod);
	bool ResumeThread() { return Resume(); }
	bool Resume();

	bool SuspendThread() { return Suspend(); }
	bool Suspend();
	pthread_t Handle() { return hThread; }
	static MPThread *FromHandle(pthread_t);
#ifdef __PLUK_LINUX__
	pthread_t ID() { return hThread; }
#else
	DWORD ID() { return dwThreadID; }
#endif
	Startup *startup;
protected:
	virtual void InitStartUp(StartThreadRoutine _ThreadFunc, void *param, bool fStartSuspend, StartThreadRoutine _CleanUp, void *paramCleanUp);
	virtual void CleanStartUp();
public:
	pthread_t hThread;
#ifdef __PLUK_LINUX__
	pthread_attr_t attr;
#else
	DWORD dwThreadID;
#endif
public:
	bool fKill;
	bool m_bAutoDelete;
	static pthread_key_t dwStartupPtr;
	static pthread_key_t dwMessageWaitBusy;
    static pthread_key_t dwWaitForMultipleObjects;
#ifdef __PLUK_LINUX__
        UXSignal ProcSignal;
#endif


protected:
#ifdef __PLUK_LINUX__
	static void *ThreadFunc(void *);
#else
	static unsigned __stdcall ThreadFunc(void *);
#endif
	static void CleanFunc(void *);

	bool fFreeStartUp;
	bool fFirstTime;
public:
};
#pragma pack(pop)

EXPORTNIX MPThread *MBeginThread(MPThread::StartThreadRoutine func, void *param, bool fStartSuspend = 0);
#ifdef __PLUK_LINUX__

inline void Sleep(int mili)
{
    MPThread::Sleep(mili);
}
inline void timeEndPeriod(UINT uPeriod)
{
	MPThread::TimeEndPeriod(uPeriod);
}
inline void timeBeginPeriod(UINT uPeriod)
{
    MPThread::TimeBeginPeriod(uPeriod);
}
struct PtrEvent {};
inline MSGTYPE CreateEvent() { return (MSGTYPE)new PtrEvent; }
 inline void DeleteEvent(MSGTYPE ev) { if (ev)delete (PtrEvent *)ev; }
#endif
#endif /* MPTHREAD_H_ */
