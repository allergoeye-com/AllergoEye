//
// Created by alex on 04.05.2018.
//

#ifndef FUTURA_CPING_H
#define FUTURA_CPING_H
#include <sys/socket.h>
#include <linux/icmp.h>
#include <poll.h>
#include <linux/in6.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include "stdafx.h"

class CPing {
    enum { PACKETSIZE =	64 };
    struct packet
    {
        struct icmphdr hdr;
        char msg[PACKETSIZE - sizeof(struct icmphdr)];
    };

public :
    CPing(LPCSTR addr);
    ~CPing();
    BOOL Ping(int N= 10, int timeout = 100);
private:
    USHORT Checksum(void *b, int len);

    sockaddr_in addr;
    int sd;
};


#endif //FUTURA_CPING_H
