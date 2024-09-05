//
// Created by alex on 04.05.2018.
//
#include "stdafx.h"
#include "CPing.h"
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_bridge.h>
#include <linux/neighbour.h>
static inline __u32 nl_mgrp(__u32 group)
{
    if (group > 31 )
        return 0;

    return group ? (1 << (group - 1)) : 0;
}

CPing::CPing(LPCSTR address)
{
    sd = -1;
    bzero(&addr, sizeof(addr));
    TString s = address;
    MVector<TString> toc;
    s.Tokenize(".", toc);
    int i = 0;
    if (toc.GetLen() == 4)
    {

        for (i = 0; i < 4; ++i)
        {
            int len = toc[i].StrLen();
            if (len && len <= 3)
            {
                int n = 0;
                for (int j = 0; j < len; ++j)
                    n += isdigit(toc[i][j]);

                if (n < len) break;
            }
            else
                break;
        }
    }
    bzero(&addr, sizeof(addr));
    addr.sin_port = 0;
    int k;
    if (i != 4)
    {
        hostent *hname  = gethostbyname(address);
        if (!hname) return;
        addr.sin_family = hname->h_addrtype;
        addr.sin_addr.s_addr = *(int*)hname->h_addr;
    }
    else
    {
        addr.sin_family = AF_INET;
        if ((k = inet_aton(address, (in_addr *)&addr.sin_addr.s_addr)) <= 0)
            return;
    }
    const int val=255;

    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if ( sd < 0 )
        return;
    if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
    {
        sd = -1;
        return;
    }
    k = fcntl(sd, F_SETFL, O_NONBLOCK);

}
//-------------------------------------
//
//-------------------------------------
CPing::~CPing()
{
    if (sd > 0)
        close(sd);
}
//-------------------------------------
//
//-------------------------------------
BOOL CPing::Ping(int N, int timeout)
{
    if (sd == -1) return false;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int cnt = 0;
    int err = 0;
    for (int i= 0; i < N; ++i)
    {
        socklen_t len=sizeof(r_addr);
        if (recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0)
            return TRUE;
        bzero(&pckt, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = getpid();
        int j;
        for (j = 0; j < sizeof(pckt.msg)-1; ++j )
            pckt.msg[j] = j+'0';
        pckt.msg[j] = 0;
        pckt.hdr.un.echo.sequence = cnt++;
        pckt.hdr.checksum = Checksum(&pckt, sizeof(pckt));
        if ( sendto(sd, &pckt, sizeof(pckt), 0, (sockaddr*)&addr, sizeof(addr)) <= 0 )
        {
            ++err;
        }
        Sleep(timeout);
    }
    return 0;
}

//-------------------------------------
//
//-------------------------------------
USHORT CPing::Checksum(void *b, int len)
{
    USHORT *buf = (USHORT *)b;
    UINT sum = 0;
    USHORT result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
