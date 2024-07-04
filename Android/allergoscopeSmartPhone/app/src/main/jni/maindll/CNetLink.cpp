//
// Created by alex on 07.05.2018.
//

#include "stdafx.h"
#include "CNetLink.h"
#pragma pack(push)
#pragma pack(0)
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_bridge.h>
#include <linux/neighbour.h>
#include <android/log.h>

#ifndef NDA_RTA
#  define NDA_RTA(r) ((rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(ndmsg))))
#endif
#pragma pack(pop)
void CNetLink::ParseRtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));

    while(RTA_OK(rta, len))
    {
        if(rta->rta_type <= max)
            tb[rta->rta_type] = rta;

        rta = RTA_NEXT(rta, len);
    }
}

//--------------------------------------------------------
//
//--------------------------------------------------------
CNetLink::CNetLink()
{

}
void CNetLink::Trace(const char *pFormat, ...)
{
/*
    if (!pFormat || !pFormat[0]) return;
    TString tr((DINT)strlen(pFormat) + 1024);
    va_list args;
    va_start(args, pFormat);
    vsprintf(tr.GetPtr(), pFormat, args);
    va_end(args);
    JVariable var;
    TString s;
    var.Set((const char *)tr.GetPtr());
    Run("OnTrace", &var, 1);
    */
    va_list lVarArgs;
    va_start(lVarArgs, pFormat);
    __android_log_vprint(ANDROID_LOG_ERROR, "TRACE", pFormat,
                         lVarArgs);
    __android_log_print(ANDROID_LOG_ERROR, "TRACE", "\n");
    va_end(lVarArgs);

}

BOOL CNetLink::ReadArp(nlmsghdr *h, TString &address, TString &mac)
{
    ndmsg *rt_msg = (ndmsg *)NLMSG_DATA(h);
    rtattr *rt_attr = (rtattr *)RTM_RTA(rt_msg);
    int len = RTM_PAYLOAD(h);
    int i = 0;
    for (; RTA_OK(rt_attr, len); rt_attr = RTA_NEXT(rt_attr, len))
    {
        switch (rt_attr->rta_type)
        {
            case NDA_DST:
                address.SetLen(64);
                address.Set(0);
                inet_ntop(AF_INET, RTA_DATA(rt_attr), address.GetPtr(), address.GetLen());
                ++i;
                break;
            case NDA_LLADDR:
            {
                BYTE *b = (BYTE *)RTA_DATA(rt_attr);
                mac.Format("%02x:%02x:%02x:%02x:%02x:%02x",  b[0], b[1], b[2], b[3], b[4], b[5]);
                ++i;
                break;
            }
            default:
                break;
        }
    }
    return i >= 2;

}
void CNetLink::Close()
{
    struct stat st;
    if (epfd != INVALID_SOCKET || fstat((int)epfd, &st ) == 0)
    {
        if (sockMsg[0] != INVALID_SOCKET)
        {
            epoll_ctl(epfd, EPOLL_CTL_DEL, sockMsg[0], 0);
        }
        if (fd != INVALID_SOCKET)
            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
        close(epfd);
    }
    if (sockMsg[1] != INVALID_SOCKET)
    {
        close(sockMsg[1]);
        close(sockMsg[0]);
    }

    if (fd != INVALID_SOCKET)
        close(fd);
    sockMsg[1] = INVALID_SOCKET;
    sockMsg[0] = INVALID_SOCKET;
    fd = epfd = INVALID_SOCKET;

}

void CNetLink::MessageLoop()
{
    bool  fDelete = false;

    if (StdError(socketpair( AF_UNIX, SOCK_DGRAM, 0, sockMsg ))) return;
    char buf[8192];
    iovec iov;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msghdr msg;
    msg.msg_name = &local;
    msg.msg_namelen = sizeof(local);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    epoll_event events[EPOOL_SIZE];
    StdError(fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0)|O_NONBLOCK));
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLERR;
    ev.data.fd = sockMsg[0];
    StdError(epoll_ctl(epfd, EPOLL_CTL_ADD, sockMsg[0], &ev));
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
    ev.data.fd = fd;
    int err = 0;
    StdError(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK));
    StdError(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev));
    mutex.SetLeave(1);
    while (1)
    {
        bzero(events, EPOOL_SIZE * sizeof(epoll_event));


        int count = epoll_wait(epfd, events, EPOOL_SIZE, -1);
        if (count < 0)
        {
            if (errno != EINTR)
                Trace("error = %s\n", strerror(errno));
            errno = 0;
            continue;

        }
        else
        {
            for (int i = 0; i < count; ++i)
            {

                if (events[i].data.fd == fd)
                {
                    if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLRDHUP))
                    {
                        if ((events[i].events & EPOLLERR))
                        {
                            int  error = 0;
                            socklen_t errlen = sizeof(error);
                            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
                            {
                                Trace("error = %s\n", strerror(error));
                                errno = 0;
                            }
                        }
                        else
                        if (events[i].events & EPOLLRDHUP)
                        {
                            Trace("EPOLLRDHUP\n");
                            goto goto__exit;
                        }
                        continue;
                    }
                    if (events[i].events & EPOLLIN)
                    {
                        BOOL fContinue = true;
                        while(fContinue)
                        {
                            ssize_t bytes = recvmsg(fd, &msg, MSG_DONTWAIT);
                            if (bytes < 0)
                            {
                                if (errno == EINTR || errno == EAGAIN)
                                {
                                   usleep(150000);
                                    continue;
                                }
                                break;
                            }
                            if (msg.msg_namelen != sizeof(local))
                                break;
                            for (nlmsghdr* h = (nlmsghdr*)buf; bytes >= (ssize_t)sizeof(*h); )
                            {
                                int len = h->nlmsg_len;
                                int l = len - sizeof(nlmsghdr);
                                if ((l < 0) || (len > bytes))
                                {
                                    //fContinue = false;
                                    continue;
                                }
                                if (h->nlmsg_type == RTM_NEWNEIGH || h->nlmsg_type == RTM_DELNEIGH)
                                {
                                    TString address, mac;
                                    if (ReadArp(h, address, mac))
                                    {
                                        JVariable var[3];
                                        TString s;
                                        var[0] = (int)(h->nlmsg_type == RTM_NEWNEIGH);
                                        var[1].Set((const char *)address.GetPtr());
                                        var[2].Set((const char *)mac.GetPtr());
                                        Run("OnArpChange", var, 3);

                                    }
                                }
                                {
                                    TString ifName;
                                    {
                                        ifinfomsg *ifi;
                                        rtattr *tb[IFLA_MAX + 1];
                                        ifi = (ifinfomsg*) NLMSG_DATA(h);
                                        ParseRtattr(tb, IFLA_MAX, IFLA_RTA(ifi), RTM_PAYLOAD(h));

                                        rtmsg *routemsg = (struct rtmsg *)NLMSG_DATA(h);

                                        if (tb[IFLA_IFNAME])
                                            ifName = (const char*)RTA_DATA(tb[IFLA_IFNAME]);
                                        BOOL fUp = (ifi->ifi_flags & IFF_UP) == IFF_UP;
                                        BOOL fRun = (ifi->ifi_flags & IFF_RUNNING) == IFF_RUNNING;

                                        char ifAddress[256];
                                        if (tb[IFLA_ADDRESS])
                                        { // проверяем валидность атрибута, хранящего имя соединения
                                            BYTE *ptr = (BYTE*)RTA_DATA(tb[IFLA_ADDRESS]); //получаем имя соединения
                                            if (ptr)
                                            {
                                                char routeAddr[128];
                                                inet_ntop(AF_INET, ptr, routeAddr, sizeof(routeAddr));
                                                TRACE("----------> %s\n", routeAddr);
                                            }
                                        }

                                        ifaddrmsg *ifa;
                                        rtattr *tba[IFA_MAX+1];
                                        ifa = (struct ifaddrmsg*)NLMSG_DATA(h);
                                        ParseRtattr(tba, IFA_MAX, IFA_RTA(ifa), RTM_PAYLOAD(h));
                                        if (tba[IFA_LOCAL])
                                            inet_ntop(AF_INET, RTA_DATA(tba[IFA_LOCAL]), ifAddress, sizeof(ifAddress));

                                        if (ifName.StrLen())
                                        switch (h->nlmsg_type)
                                        {
                                            case RTM_DELADDR:
                                            {
                                                JVariable var[2];
                                                TString s;
                                                var[0] = ifName;
                                                var[1] = ifAddress;
                                                Run("OnDeleteAddress", var, 2);
                                                break;
                                            }
                                            case RTM_DELLINK:
                                            {
                                                JVariable var[1];
                                                TString s;
                                                var[0] = ifName;
                                                Run("OnDeleteInterface", var, 1);

                                             }
                                             break;
                                            case RTM_NEWLINK:
                                            {
                                                JVariable var[3];
                                                TString s;
                                                var[0] = ifName;
                                                var[1] = fUp;
                                                var[2] = fRun;
                                                Run("OnNewInterface", var, 3);
                                            }
                                                break;
                                            case RTM_NEWADDR:
                                            {
                                                JVariable var[2];
                                                TString s;
                                                var[0] = ifName;
                                                var[1] = ifAddress;
                                                Run("OnNewAddress", var, 2);
                                                break;
                                            }
                                        }
                                    }
                                }

                                bytes -= NLMSG_ALIGN(len);
                                h = (nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
                            }
                           fContinue = false;
                        }

                    }

                }
            }

            for (int i = 0; i < count; ++i)
            {

                if (events[i].data.fd == sockMsg[0])
                {
                    Msg m;
                    fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) & ~O_NONBLOCK);
                    recv(sockMsg[0], &m, sizeof(Msg), 0);

                    fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0)|O_NONBLOCK);

                    if (m.m == THREAD_EXIT)
                        goto goto__exit;
                }
            }
           // usleep(250000);
        }
    }
    goto__exit:
    __pthread_cleanup_pop( &__cleanup, 1);
    Close();
    self.Detach();
    mutex.SetLeave(1);
}
void CNetLink::Exit()
{
    if (!IsConnected() || !mutex.TryEnter()) return;
    Msg a(THREAD_EXIT, 0, 0);
    send(sockMsg[1], &a, sizeof (Msg), MSG_DONTWAIT);
    if (!mutex.TryEnter(10000))
    {

            if (IsConnected())
            {
                Msg b(THREAD_EXIT,0, 0);
                send(sockMsg[1], &b, sizeof (Msg), MSG_DONTWAIT);
                if (!mutex.TryEnter(15000))
                    mutex.SetLeave(1);
                else
                    self.Stop();

            }
    }
    mutex.SetLeave(1);
}

BOOL CNetLink::Connect(int _type)
{

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0)
        return false;
    type = _type;
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups =  RTMGRP_NOTIFY | RTMGRP_NEIGH | RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV4_MROUTE;
    local.nl_pid = getpid();

    if (::bind(fd, (sockaddr*)&local, sizeof(local)) < 0)
    {
        TRACE("Ошибка связывания с netlink сокетом: %s", (char*)strerror(errno));
        close(fd);
        return false;
    }
    sockMsg[0] = sockMsg[1] = INVALID_SOCKET;
    epfd = epoll_create(EPOOL_SIZE);
    if (epfd < 0)
    {
        StdError(-1);
        close(fd);
        fd = INVALID_SOCKET;
    }

    return fd != INVALID_SOCKET;

}
//-----------------------------------------
//
//-----------------------------------------
void CNetLink::CleanUp(void *p)
{
    if (p)
    {
        CNetLink *th = (CNetLink *)p;
        th->Close();
        th->self.Detach();
    }
}


//--------------------------------------------------------
//
//--------------------------------------------------------
int CNetLink::Receive(char *buf, sockaddr_nl &sock_addr, int sock)
{
    struct nlmsghdr *nh;
    int len, nll = 0;
    char *buf_ptr;

    buf_ptr = buf;
    while (1) {
        len = recv(sock, buf_ptr, 8192 - nll, 0);
        if (len < 0)
            return len;

        nh = (struct nlmsghdr *)buf_ptr;

        if (nh->nlmsg_type == NLMSG_DONE)
            break;
        buf_ptr += len;
        nll += len;
        if ((sock_addr.nl_groups & RTMGRP_NEIGH) == RTMGRP_NEIGH)
            break;
        if ((sock_addr.nl_groups & RTMGRP_LINK) == RTMGRP_LINK)
            break;
        if ((sock_addr.nl_groups & RTMGRP_IPV4_IFADDR) == RTMGRP_IPV4_IFADDR)
            break;

        if ((sock_addr.nl_groups & RTMGRP_IPV4_ROUTE) == RTMGRP_IPV4_ROUTE)
            break;
    }
    return nll;

}
//--------------------------------------------------------
//
//--------------------------------------------------------
BOOL CNetLink::ReadArpTable(RBMap<TString, TString> &table)
{
    sockaddr_nl sa;
    char buf[8192];
    msghdr msg;
    iovec iov;
    BOOL ret = true;
    struct {
        nlmsghdr nl;
        ndmsg rt;
        char buf[8192];
    } req;

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0)
        return false;
    memset(&buf, 0, sizeof(buf));
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    if (::bind(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        close(sock);
        return false;
    }
    memset(&req, 0, sizeof(req));
    req.nl.nlmsg_len = NLMSG_LENGTH(sizeof(rtmsg));
    req.nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nl.nlmsg_type = RTM_GETADDR; //RTM_GETNEIGH;
    req.rt.ndm_state = NUD_REACHABLE;
    req.rt.ndm_family = AF_INET;
    req.nl.nlmsg_pid = getpid();
    req.nl.nlmsg_seq = 1;
    memset(&msg, 0, sizeof(msg));
    iov.iov_base = (void *)&req.nl;
    iov.iov_len = req.nl.nlmsg_len;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    ret = sendmsg(sock, &msg, 0);
    if (ret < 0)
    {
        TRACE("send error: %s\n", strerror(errno));
        close(sock);
        return false;
    }
    int bytes = Receive(buf, sa, sock);
    if (bytes < 0)
    {
        TRACE("recv from netlink: %s\n", strerror(bytes));
        close(sock);
        return false;
    }
    ReadArp(table, buf, bytes);
    close(sock);
    return true;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
BOOL CNetLink::ReadArp(RBMap<TString, TString> &table, char *buff, int bytes)
{
    nlmsghdr *h = (nlmsghdr *)buff;

    for (; NLMSG_OK(h, bytes); h = NLMSG_NEXT(h, bytes))
    {
        TString address, mac;
        if (ReadArp(h, address, mac) >= 2)
        {
            TRACE("%s %s\n", address.GetPtr(), mac.GetPtr());
            table[address] = mac;
        }

    }
    return table.m_iNum > 0;
}
BOOL CNetLink::Start()
{
    self.Attach(false);
    return TRUE;
}
//-----------------------------------------
//
//-----------------------------------------
LRESULT CNetLink::PostMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!this->IsConnected()) return 0;
    Msg a(msg, wParam, lParam);
    send(sockMsg[1], &a, sizeof (Msg), MSG_DONTWAIT);
    return 1;
}
//-----------------------------------------
//
//-----------------------------------------
BOOL CNetLink::IsConnected()
{
    struct stat st;
    return fd != INVALID_SOCKET && !fstat((int)fd, &st );
}

//--------------------------------------------------------
//
//--------------------------------------------------------
void CNetLink::ParseRTAttr(rtattr *tb[], int max, rtattr *rta, int len)
{
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));

    while(RTA_OK(rta, len))
    {
        if(rta->rta_type <= max)
            tb[rta->rta_type] = rta;
        rta = RTA_NEXT(rta, len);
    }
}
