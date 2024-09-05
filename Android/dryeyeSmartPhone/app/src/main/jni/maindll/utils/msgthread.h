#ifndef __MSG__PTHREAD__
#define __MSG__PTHREAD__
#include "mpthread.h"
#include "mfile.h"
#ifdef __PLUK_LINUX__


extern int h_errno;

typedef void (*TimerProc)(GHWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);
#pragma pack(push) //NADA
#pragma pack(4)

EXPORTNIX class _PEXPORT MSGThread
{
public:
	MSGThread(bool fInit = true);
    virtual ~MSGThread();
	void InitMsg();
	void Detach();
  LRESULT RunMy(void *func, void *arg);
  virtual LRESULT WindowProc(GHWND hWnd, MSGTYPE message, WPARAM wParam, LPARAM lParam) { return 0; }
  virtual LRESULT WindowProc(THWND hWnd, MSGTYPE message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual LRESULT SendMessage(GHWND hWnd, MSGTYPE Msg, WPARAM wParam = 0, LPARAM lParam = 0, void *arg = 0);
    virtual LRESULT PostMessage(GHWND hWnd, MSGTYPE Msg, WPARAM wParam = 0, LPARAM lParam = 0, void *arg = 0);
    BOOL PumpMessage();

	virtual LRESULT SendMessage(MSGTYPE Msg, WPARAM wParam = 0, LPARAM lParam = 0, void *arg = 0)
	{
		return SendMessage((GHWND)0, Msg, wParam, lParam,arg);
	}
    virtual LRESULT PostMessage(MSGTYPE Msg, WPARAM wParam = 0, LPARAM lParam = 0, void *arg = 0)
	{
		return PostMessage((GHWND)0, Msg, wParam, lParam,arg);
	}
    virtual LRESULT SetEvent(MSGTYPE Msg)
    {
		return PostMessage((GHWND)0, Msg, 0, 0, 0);
    }
    MPThread *Self();
    static MSGThread *FromHandle(THWND);
    static MSGThread *FromHandle(MPThread *t);

    virtual LRESULT OnMessage(TMSG &msg);
#if(1) //ndef ANDROID_NDK
    BOOL SetTimer(UINT iTimeID, size_t mili, TimerProc func = 0);
    BOOL KillTimer(UINT iTimeID);
#endif
    virtual void OnIo(TMSG &msg) { }
    int pthread_sigqueue (pthread_t __threadid, int __signo,
			     const union sigval __value);
protected:
    LRESULT MessageProc(TMSG &msg, BOOL *fFreeSendedThread = 0);
    static bool WaitMessage(TMSG *msg);

public:
    static BOOL ReplyMessage(LPARAM l);
    static MSGThread *GetCurrent();

    void GUIIdle();

protected:
    void InitSignals();
	MPThread::Startup *startup;
public:
	struct mTimer {
	int id;
    UINT tm;
	TimerProc func;
	mTimer() { id = 0; func = 0; tm = 0; } 
	void Set(int _id, UINT _tm, TimerProc _func) { id = _id; tm = _tm; func = _func; } 
	};
	MHashMap<mTimer, UINT> *timer;
    
    friend BOOL privateGetMessage(TMSG *msg, BOOL fAll);
    friend void CleanUpMSGThreads(MPThread *p);
    friend BOOL privateDispatchMessage(TMSG *msg);



public:
    THWND m_hWnd;
friend class MPThread;

};
EXPORTNIX class _PEXPORT VThread: public MSGThread {
public:
    VThread():MSGThread(false), fRun(0) {  }
    virtual ~VThread() { Stop(); }
	enum {
	 MSG_DESTROY = 100,
	 MSG_PAUSE = 101,
	 MSG_PLAY = 102
	};
	BOOL Play();
	BOOL Stop();
	BOOL Pause();
	
	virtual BOOL OnRun() = 0;
	virtual BOOL OnExit() = 0;
	virtual BOOL OnStart() = 0;
public:
	static void *func(void *arg);
	static void *exit_func(void * arg);
protected:
	bool fRun;
public:
    CriticalSection m_crit;
	MPThread th;
};

#define EPOOL_SIZE 128
#define MSG_ADDSOCK 1
#define MSG_DELSOCK 2
#ifndef SCK_EVENT
#define SCK_EVENT			901
#endif


EXPORTNIX class _PEXPORT EpollThread : public MSGThread {
public:
    struct SockMsg {
    int fd;
    UINT mask;
    MSGThread *hThread;
    GHWND hWnd; 
    MSGTYPE Msg;
    bool fSink;
    SockMsg() { memset(this, 0, sizeof(SockMsg)) ; Msg = WM_USER + SCK_EVENT; }
    SockMsg(int _fd, UINT _mask, bool _fSink, MSGTYPE _Msg, MSGThread *thread, GHWND wnd = 0) 
    {
        fSink = _fSink;
        Msg = _Msg;
		fd = _fd; 
		mask = _mask; 
		hThread = thread;
		hWnd = wnd;
    }
  };

    public:
    EpollThread():MSGThread(false) { epfd = 0; sockMsg[0] = sockMsg[1] = INVALID_SOCKET; }
    virtual ~EpollThread();
	virtual LRESULT SendMessage(MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
    virtual LRESULT PostMessage(MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
    void Run();
    virtual LRESULT OnMessage(TMSG &msg);
    int epfd;
    int sockMsg[2];
protected:
    virtual void SendPostMessage(MSGThread *wnd, GHWND hWnd, MSGTYPE Msg, WPARAM wParam, LPARAM lParam)
    {
    	wnd->PostMessage(hWnd, Msg, wParam, lParam, 0);
    }

     bool Lock();
	void UnLock();

    void Init();
    MHashMap<SockMsg, int> data;

    
};
#ifndef _PLUKEXPORT
#define _PLUKEXPORT
#endif
EXPORTNIX class _PLUKEXPORT ProcCon : public MSGThread {
#pragma pack(push)
#pragma pack(4)
    struct ConMsg {
    pid_t pid;
    _int32 fSend;
    DWORD64 thread;
    DWORD64 Msg;
    DWORD64 wParam;
    DWORD64 lParam;
    
        void Set(pid_t _pid, DWORD64 _thread, DWORD64 _Msg, DWORD64 _wParam, DWORD64 _lParam, BOOL _fSend)
        {
            pid = _pid; thread = _thread; Msg = _Msg, wParam = _wParam; lParam =_lParam, fSend = _fSend;
        }
    };
#pragma pack(pop)
public:
	ProcCon():MSGThread(0) { sockfd = 0; };
    virtual ~ProcCon();
	bool Register(const char *s = 0);
    void Free();
    static DWORD64 _SendMessage(pid_t proc, DWORD64 thread, DWORD64 Msg, DWORD64 wParam, DWORD64 lParam, bool fSend, UINT *iTimeout = 0);
protected:    
    virtual LRESULT OnMessage(TMSG &msg);
    SOCKET sockfd;
};
#pragma pack(pop) //NADA

#define WAIT_OBJECT_0       0
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define WAIT_ABANDONED_0          ((DWORD   )0x00000080L)    
#define WAIT_FAILED          ((DWORD   )-1)    
#define WAIT_TIMEOUT          ((DWORD   )-1)    

extern "C++" int WaitForMultipleObjects(DWORD nCount, CONST MSGTYPE *pHandles, BOOL fWaitAll, DWORD iTime);
extern "C++" int WaitForSingleObject(MSGTYPE iMsg, DWORD64 iTime);

extern "C++" BOOL WaitForSingeMessage(TMSG *msg, MSGTYPE iMsg, BOOL fGUIIdle = FALSE);
//extern "C++" BOOL WaitForSingeMessage(TMSG *msg, MSGTYPE iMsg);
extern "C++" BOOL GetMessageA(TMSG *msg, BOOL fAll = 1);
extern "C++" BOOL PeekMessage(TMSG *msg, BOOL fRemove);
extern "C++" BOOL PeekMessage(TMSG *msg, THWND obj, BOOL fRemove);
extern "C++" BOOL IsWindow(THWND h);
extern "C++" BOOL PeekMessage(TMSG *msg, GHWND hWnd, BOOL fRemove);

extern "C++" BOOL PeekMessage(TMSG *msg, THWND obj, int, int, BOOL fRemove);
inline BOOL PreTranslateMessage(TMSG *) { return 0; }
#ifndef PM_REMOVE
#define  PM_REMOVE 1
#define  PM_NOREMOVE 0
#endif

extern "C++" BOOL TranslateMessage(TMSG *msg);
extern "C++" BOOL DispatchMessage(TMSG *msg);
extern "C++" BOOL GetMessageA(TMSG *msg, THWND obj, int, int);
extern EpollThread *ePollGLB;

extern "C++" void ShowWindow(THWND, int);
extern "C++" LRESULT PostMessage(GHWND hWnd, MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
extern "C++" LRESULT SendMessage(GHWND hWnd, MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
extern "C++" LRESULT PostMessage(GHWND hWnd, THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
extern "C++" LRESULT SendMessage(GHWND hWnd, THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);

inline LRESULT PostMessage(MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0)
{
	return PostMessage(0, obj, Msg, wParam, lParam, arg);
}
inline LRESULT SendMessage(MSGThread *obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0)
{
	return SendMessage(0, obj, Msg, wParam, lParam, arg);

}
inline BOOL ReplyMessage(LPARAM l) { return MSGThread::ReplyMessage(l); }

extern "C++" LRESULT PostMessage(THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
extern "C++" LRESULT SendMessage(THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0);
inline LRESULT PostThreadMessage(THWND obj, MSGTYPE Msg, WPARAM wParam, LPARAM lParam, void *arg = 0)

{
    return PostMessage(obj, Msg, wParam, lParam, arg);
}

inline LRESULT DefWindowProc(THWND hThread, MSGTYPE message, WPARAM wParam, LPARAM lParam) { return 0; }

#define SMTO_BLOCK 1
inline LRESULT SendMessageTimeout(THWND hThread, MSGTYPE Msg,WPARAM wParam,LPARAM lParam,UINT fuFlags,UINT uTimeout, LRESULT *lpdwResult)
{
    LRESULT res = SendMessage(hThread, Msg, wParam, lParam, &uTimeout);
    if ((int)res != -1)
        if (lpdwResult) *lpdwResult = res;
    return res != (LRESULT)-1;
}
extern "C++" BOOL SetTimer(THWND th, UINT iTimerID, UINT uElapse, TimerProc func);
extern "C++" BOOL KillTimer(THWND th, UINT iTimerID);

extern "C++" BOOL SetTimer(MSGThread *obj, UINT iTimerID, UINT uElapse, TimerProc func);
extern "C++" BOOL KillTimer(MSGThread *obj, UINT iTimerID);
extern "C++" BOOL SetTimer(GHWND,  UINT iTimerID, UINT uElapse, TimerProc func);
extern "C++" BOOL KillTimer(GHWND, UINT iTimerID);



extern "C++" void WSACancelAsyncRequest(HANDLE hAsyncTaskHandle);
extern "C++" HANDLE WSAAsyncGetHostByName(MSGThread *hThread, MSGTYPE wMsg, const char *name, char *buf, int buflen);
extern "C++" HANDLE WSAAsyncGetHostByName(GHWND hWnd, MSGTYPE wMsg, const char *name, char *buf, int buflen);
extern "C++" LRESULT WSAAsyncSelect(SOCKET s, MSGThread *hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);
extern "C++" LRESULT WSASelect(SOCKET s, MSGThread *hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);
extern "C++" LRESULT WSACancel(SOCKET s, MSGThread *pollTh = ePollGLB);
extern "C++" LRESULT WSAAsyncSelect(SOCKET s, GHWND, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);
extern "C++" LRESULT WSASelect(SOCKET s, GHWND, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);
extern "C++" HANDLE WSAAsyncGetHostByName(THWND hThread, MSGTYPE wMsg, const char *name, char *buf, int buflen);
extern "C++" LRESULT WSAAsyncSelect(SOCKET s, THWND hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);
extern "C++" LRESULT WSASelect(SOCKET s, THWND hThread, MSGTYPE Msg, LPARAM lEvent, MSGThread *pollTh = ePollGLB);


extern "C++" BOOL FindCloseChangeNotification(HANDLE handle);
extern "C++" BOOL FindNextChangeNotification(HANDLE handle);
extern "C++" HANDLE FindFirstChangeNotification(LPCSTR lpPathName, BOOL, DWORD dwNotifyFilter);

#define PeekMessageA PeekMessage
extern "C++" BOOL InSendMessage();

extern "C++" pthread_t GetWindowThreadProcessId(THWND hThread, DWORD *pid);
extern "C++" int WSAGetLastError();
inline void WSASetLastError(int i) { errno = i; }
#define closesocket close
typedef sockaddr_in SOCKADDR_IN;
typedef sockaddr SOCKADDR;
typedef sockaddr* LPSOCKADDR;
#define WSABASEERR                 10000
#define WSAGETSELECTEVENT(lParam)           LOWORD(lParam)
#define WSAGETSELECTERROR(lParam)           HIWORD(lParam)

/*
 * Windows Sockets definitions of regular Microsoft C error constants
 */
#define WSAEINTR                   (WSABASEERR+4)
#define WSAEBADF                   (WSABASEERR+9)
#define WSAEACCES                  (WSABASEERR+13)
#define WSAEFAULT                  (WSABASEERR+14)
#define WSAEINVAL                  (WSABASEERR+22)
#define WSAEMFILE                  (WSABASEERR+24)

/*
 * Windows Sockets definitions of regular Berkeley error constants
 */
#define WSAEWOULDBLOCK             (WSABASEERR+35)
#define WSAEINPROGRESS             (WSABASEERR+36)
#define WSAEALREADY                (WSABASEERR+37)
#define WSAENOTSOCK                (WSABASEERR+38)
#define WSAEDESTADDRREQ            (WSABASEERR+39)
#define WSAEMSGSIZE                (WSABASEERR+40)
#define WSAEPROTOTYPE              (WSABASEERR+41)
#define WSAENOPROTOOPT             (WSABASEERR+42)
#define WSAEPROTONOSUPPORT         (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT         (WSABASEERR+44)
#define WSAEOPNOTSUPP              (WSABASEERR+45)
#define WSAEPFNOSUPPORT            (WSABASEERR+46)
#define WSAEAFNOSUPPORT            (WSABASEERR+47)
#define WSAEADDRINUSE              (WSABASEERR+48)
#define WSAEADDRNOTAVAIL           (WSABASEERR+49)
#define WSAENETDOWN                (WSABASEERR+50)
#define WSAENETUNREACH             (WSABASEERR+51)
#define WSAENETRESET               (WSABASEERR+52)
#define WSAECONNABORTED            (WSABASEERR+53)
#define WSAECONNRESET              (WSABASEERR+54)
#define WSAENOBUFS                 (WSABASEERR+55)
#define WSAEISCONN                 (WSABASEERR+56)
#define WSAENOTCONN                (WSABASEERR+57)
#define WSAESHUTDOWN               (WSABASEERR+58)
#define WSAETOOMANYREFS            (WSABASEERR+59)
#define WSAETIMEDOUT               (WSABASEERR+60)
#define WSAECONNREFUSED            (WSABASEERR+61)
#define WSAELOOP                   (WSABASEERR+62)
#define WSAENAMETOOLONG            (WSABASEERR+63)
#define WSAEHOSTDOWN               (WSABASEERR+64)
#define WSAEHOSTUNREACH            (WSABASEERR+65)
#define WSAENOTEMPTY               (WSABASEERR+66)
#define WSAEPROCLIM                (WSABASEERR+67)
#define WSAEUSERS                  (WSABASEERR+68)
#define WSAEDQUOT                  (WSABASEERR+69)
#define WSAESTALE                  (WSABASEERR+70)
#define WSAEREMOTE                 (WSABASEERR+71)

/*
 * Extended Windows Sockets error constant definitions
 */
#define WSASYSNOTREADY             (WSABASEERR+91)
#define WSAVERNOTSUPPORTED         (WSABASEERR+92)
#define WSANOTINITIALISED          (WSABASEERR+93)
#define WSAEDISCON                 (WSABASEERR+101)
#define WSAENOMORE                 (WSABASEERR+102)
#define WSAECANCELLED              (WSABASEERR+103)
#define WSAEINVALIDPROCTABLE       (WSABASEERR+104)
#define WSAEINVALIDPROVIDER        (WSABASEERR+105)
#define WSAEPROVIDERFAILEDINIT     (WSABASEERR+106)
#define WSASYSCALLFAILURE          (WSABASEERR+107)
#define WSASERVICE_NOT_FOUND       (WSABASEERR+108)
#define WSATYPE_NOT_FOUND          (WSABASEERR+109)
#define WSA_E_NO_MORE              (WSABASEERR+110)
#define WSA_E_CANCELLED            (WSABASEERR+111)
#define WSAEREFUSED                (WSABASEERR+112)
#endif

#endif
