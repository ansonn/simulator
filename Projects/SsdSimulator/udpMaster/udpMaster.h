/*---------------------------------------------------------------------------*/
//  udp master
//
//  CopyRight   : bj.memblaze.com
//  Author      :
//
//  Notes       :
//      2015-12-02  Created by shuke.wang@memblaze.com
/*---------------------------------------------------------------------------*/
#ifndef _UDP_MASTER_H
#define _UDP_MASTER_H
#include <WINSOCK.H>
#include <tchar.h>

class UdpMaster
{
#define _PORT       6900
#define _GROUPIP    "228.4.5.6"    
#define MSGBUFSIZE  256
public:
    UdpMaster() : initSuccess(false)
    {
        WSADATA     wsaData;
        WORD wVersionRequested;

        wVersionRequested = MAKEWORD(1, 1);
        WSAStartup(wVersionRequested, &wsaData);

        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            _tprintf(_T("udp master create socket failed\n"));
            return;
        }

        memset(&rAddr, 0, sizeof(rAddr));
        rAddr.sin_family = AF_INET;
        rAddr.sin_addr.s_addr = inet_addr(_GROUPIP);
        rAddr.sin_port = htons(_PORT);

        initSuccess = true;
    }

    ~UdpMaster()
    {
        closesocket(fd);
    }

    int send(char *buffer, int bufferLen)
    {
        int nbytes = 0;
        
        if (!initSuccess)
        {
            _tprintf(_T("udp master no initsuccess\n"));
            return -1;
        }

        if ((nbytes = sendto(fd, buffer, bufferLen, 0, (struct sockaddr *)&rAddr, sizeof(rAddr))) <= 0)
        {
            _tprintf(_T("udp master create socket failed\n"));
        }

        return nbytes;
    }

    int recv(char *p, int len)
    {
        int nbytes = 0;
        struct sockaddr_in addr;
        int addrLen = sizeof(addr);

        if (!initSuccess)
        {
            _tprintf(_T("udp master no initsuccess\n"));
            return -1;
        }

        if ((nbytes = recvfrom(fd, p, len, 0, (struct sockaddr *)&addr, (int *)&addrLen)) < 0)
        {
            _tprintf(_T("udp master no initsuccess\n"));
            return -1;
        }

        return nbytes;
    }

    int waitAck(void)
    {
        char buf[64] = { 0 };

        if (recv(buf, sizeof(buf)) <= 0)
            return -1;

        if (!strncmp(buf, "ack", 3))
            return 0;

        return -1;
    }

private:
    int fd;
    bool initSuccess;
    struct sockaddr_in rAddr;
};

#endif
