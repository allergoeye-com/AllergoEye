//
// Created by alex on 07.05.2018.
//

#ifndef FUTURA_CNETLINK_H
#define FUTURA_CNETLINK_H
#pragma pack(push)
#pragma pack(0)

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#pragma pack(pop)

#include "msgthread.h"

#pragma pack(push)
#pragma pack(4)

class CNetLink : public AObject {
public:
    enum {
        THREAD_EXIT = 100
    };

    struct Msg {
        UINT m;
        WPARAM w;
        LPARAM l;

        Msg(UINT _m = 0, WPARAM _w = 0, LPARAM _l = 0)
        { Init(_m, _w, _l); }

        void Init(UINT _m, WPARAM _w, LPARAM _l)
        { m = _m, l = _l, w = _w; }
    };

    enum {
        TYPE_ARP, TYPE_ROUTE
    };

    CNetLink();

    virtual ~CNetLink()
    {}

    BOOL ReadArpTable(RBMap<TString, TString> &table);

    void MessageLoop();

    BOOL Connect(int _type);

    BOOL Start();

    BOOL IsConnected();

    LRESULT PostMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

    LocalSemaphore mutex;

    void Exit();

    void Close();

    static void CleanUp(void *);

public:
    MPThread self;
    __pthread_cleanup_t __cleanup;
protected:
    void ParseRtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);

    int Receive(char *buff, sockaddr_nl &sock_addr, int sock);

    BOOL ReadArp(RBMap<TString, TString> &table, char *buff, int bytes);

    BOOL ReadArp(nlmsghdr *h, TString &address, TString &mac);

    void ParseRTAttr(rtattr *tb[], int max, rtattr *rta, int len);

    void Trace(const char *pFormat, ...);

    int type;
    int fd;
    int epfd;
    int sockMsg[2];
    bool fRun;
    sockaddr_nl local;

};

#pragma pack(pop)


#endif //FUTURA_CNETLINK_H
