#ifndef _RWThread_h_
#define _RWThread_h_

#ifdef WIN32
#include "MPThread.h"
#include "LocalSemaphore.h"

#else
//#define GLOBAL_POOL
#include "CSemaphore.h"

#endif

class RWThread {
public:
#pragma pack(push)
#pragma pack(1)
    struct Param {
        int len;
        char param[1];
    };

    struct Query {
        char name[8];

        void Set(LPCSTR str)
        {
            strcpy_s(name, 8, str);
        }
    };

    struct FunCall {
        char name[8];
        int len;
        int num_param;
        Param param[1];

        void Init(LPCSTR call)
        {
            strcpy_s(name, 8, call);
            len = sizeof(FunCall);
            num_param = 0;
            memset(param, 0, sizeof(Param));
        }

        void SetName(LPCSTR call)
        {
            strcpy_s(name, 8, call);
        }

    };

#pragma pack(pop)
public:
    struct Msg {
        UINT m;
        WPARAM w;
        LPARAM l;

        Msg(UINT _m = 0, WPARAM _w = 0, LPARAM _l = 0)
        { Init(_m, _w, _l); }

        void Init(UINT _m, WPARAM _w, LPARAM _l)
        { m = _m, l = _l, w = _w; }
    };

    struct PacketRead {
        Query cmd;
        BYTE *pResult;
        int dwBytesTransferred;
    };
    struct PacketWrite : public PacketRead {
        BYTE *pBuff;
        int dwLenBuff;
    };
    struct PacketWriteFile : public PacketWrite {
        char *FilePath;
        char *date;
    };

    struct WaitPacket {
        PacketRead *pRead;

        WaitPacket()
        { Reset(); }

        bool IsValid()
        { return pRead != 0; }

        void Reset()
        { pRead = 0; }

        void Init(PacketRead *p)
        {
            Reset();
            pRead = p;
        }
    };

    enum {
        iREAD = 1000,
        iWRITE,
        iWRITE_FILE,
        iGETROI,
        iERROR,
        iFILE,
        iOK,
        iTHREAD_EXIT
    };

    RWThread();

    virtual ~RWThread();

    BOOL IsConnected();

#ifdef WIN32
#else

    virtual LRESULT
    SendMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0, int timeout = 240000) = 0;

    virtual LRESULT PostMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) = 0;

    static RWThread *instance;

    virtual void MessageLoop() = 0;

#endif
protected:
    LocalSemaphore critical;
    LocalSemaphore mutex;

    bool OnRead(SOCKET fd, MArray<char> &buffer);

    bool IsName(char *name);

    virtual BOOL Close();

    int write(void *p, int size);

    static void CleanUp(void *);

protected:
#ifdef WIN32
    int read(SOCKET fd, void* buff, int len);

    WSAOVERLAPPED RecvOverlapped;
    WSAOVERLAPPED SendOverlapped;
    MThread thread;

    WSANETWORKEVENTS NetworkEvents;
    WSAEVENT eOnRead;
    WSAEVENT eExit;

#else
    __pthread_cleanup_t __cleanup;
    MPThread self;
#endif
#ifndef WIN32
#ifdef GLOBAL_POOL
    public:
        static SOCKET epfd;
#else
protected:
    SOCKET epfd;
#endif
#endif

    SOCKET sockMsg[2];
    SOCKET fd;
    bool fNonBlock;
    int port;
    bool fRun;
    char address[32];

};

#endif
