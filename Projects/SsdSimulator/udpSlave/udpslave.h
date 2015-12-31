/*---------------------------------------------------------------------------*/
//  udp receiver
//
//  CopyRight   : bj.memblaze.com
//  Author      :
//
//  Notes       :
//      2015-12-02  Created by shuke.wang@memblaze.com
/*---------------------------------------------------------------------------*/
#ifndef _UDP_SLAVE_H
#define _UDP_SLAVE_H
#include <WINSOCK.H>
#include <tchar.h>

class UdpSlave
{
#define _PORT       6900
#define _GROUPIP    "228.4.5.6"    
#define MSGBUFSIZE  256
public:
    UdpSlave() : rAddrValid(false), initSuccess(false)
    {
        u_int yes = 1;
        struct ip_mreq mreq;
        struct sockaddr_in addr;

        WSADATA     wsaData;
        WORD wVersionRequested;
        wVersionRequested = MAKEWORD(1, 1);
        WSAStartup(wVersionRequested, &wsaData);

        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            _tprintf(_T("udp slave create socket failed\n"));
            return;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
        {
            _tprintf(_T("udp slave setsockopt failed\n"));
            return;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
        addr.sin_port = htons(_PORT);
        /* bind to receive address */
        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        {
            _tprintf(_T("udp slave bind failed\n"));
            return;
        }

        mreq.imr_multiaddr.s_addr = inet_addr(_GROUPIP);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0)
        {
            _tprintf(_T("udp slave setsockopt add multiaddr failed\n"));
            return;
        }

        initSuccess = true;
    }

    ~UdpSlave()
    {
        closesocket(fd);
    }

    int recv(char *mesgBuffer, int bufferLen)
    { 
        int nbytes = 0;
        int addrlen = sizeof(rAddr);

        if (!initSuccess)
        {
            _tprintf(_T("udp slave no initsuccess\n"));
            return -1;
        }

        nbytes = recvfrom(fd, mesgBuffer, bufferLen, 0
                            , (struct sockaddr *)&rAddr, (int *)&addrlen);
        if (nbytes <= 0)
            _tprintf(_T("udpslave recv failed\n"));
        else
            rAddrValid = true;

        return nbytes;
    }

    int echo(char *mesgBuffer, int bufferLen)
    {
        int nbytes = 0;

        if (initSuccess == false && rAddrValid == false)
        {
            _tprintf(_T("udpslave no init success or invalid remote addr\n"));
            return -1;
        }

        nbytes = sendto(fd, mesgBuffer, bufferLen, 0, (struct sockaddr *)&rAddr, sizeof(rAddr));
        if (nbytes <= 0)
        {
            _tprintf(_T("udpslave echo sendto failed\n"));
            return -1;
        }

        return nbytes;
    }

    int echoAck(void)
    {
        return echo("ack", 3);
    }

    int echoNack(void)
    {
        return echo("nack", 4);
    }

private:
    int fd;
    bool rAddrValid;
    bool initSuccess;
    struct sockaddr_in rAddr;
    
};

#endif
