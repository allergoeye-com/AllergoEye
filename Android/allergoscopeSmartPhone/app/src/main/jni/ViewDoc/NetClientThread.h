#ifndef _NetClientThread_h_
#define _NetClientThread_h_

#include "RWThread.h"

class NetClientThread : public RWThread {
public:
    NetClientThread()
    {}

    virtual ~NetClientThread()
    {}

#ifdef WIN32
    static DWORD WINAPI MessageLoop(void* h);
#else

    virtual void MessageLoop();

#endif

    BOOL Connect(const char *host, int port);

    virtual LRESULT
    SendMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0, int timeout = 240000);

    virtual LRESULT PostMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

public:
#ifndef WIN32

    static void InitThread();

    static void ReleaseThread();

#endif
protected:
    UINT result;

    LRESULT OnMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

    void OnRead(SOCKET fd);

    int Read(PacketRead *);

    int Write(PacketWrite *);

    int WriteFile(PacketWriteFile *);

    BOOL Start();

    int Connect(SOCKET socketClient, const char *host, const char *port);

    int GetRoi(PacketWriteFile *p);


#ifdef WIN32
    int send(SOCKET, Msg* buff, int, int);
    Msg global;
#endif
    WaitPacket waitPack;

};

#endif



