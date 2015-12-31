/*---------------------------------------------------------------------------*/
//  udp endpoint
//
//  CopyRight   : bj.memblaze.com
//  Author      :
//
//  Notes       :
//      2015-12-02  Created by shuke.wang@memblaze.com
/*---------------------------------------------------------------------------*/
#ifndef _UDP_ENDPOINT_H
#define _UDP_ENDPOINT_H
#include <WINSOCK.H>
#include <tchar.h>

class udpEndpoint
{
public:
    udpEndpoint() : initSuccess(false)
    {
        WSADATA     wsaData;
        WORD wVersionRequested;

        wVersionRequested = MAKEWORD(1, 1);
        WSAStartup(wVersionRequested, &wsaData);
    }

    ~udpEndpoint()
    {
        if (initSuccess)
            closesocket(fd);
    }

    int init(u_short bindPort)
    {
        u_int yes = 1;
        struct sockaddr_in addr;

        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            _tprintf(_T("udp slave create socket failed\n"));
            return -1;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
        {
            _tprintf(_T("udp slave setsockopt failed\n"));
            return -1;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* N.B.: differs from sender */
        addr.sin_port = htons(bindPort);
        /* bind to receive address */
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            _tprintf(_T("udp slave bind failed\n"));
            return -1;
        }
        initSuccess = true;

        return 0;
    }

    int recv_nb(char *mesgBuffer, int bufferLen)
    { 
        int nbytes = 0;
        u_long rbytes = 0;
        struct sockaddr_in tAddr;
        int addrlen = sizeof(tAddr);

        if (!initSuccess)
        {
            _tprintf(_T("udp slave no initsuccess\n"));
            return -1;
        }

        if (ioctlsocket(fd, FIONREAD, &rbytes) != 0)
        {
            _tprintf(_T("udp ioctlsocket failed\n"));
            return -1;
        }

        if (rbytes == 0)
            return 0;

        nbytes = recvfrom(fd, mesgBuffer, bufferLen, 0
                                        , (struct sockaddr *)&tAddr, (int *)&addrlen);
        if (nbytes <= 0)
            _tprintf(_T("udpslave recv failed\n"));

        return nbytes;
    }

    int recv(char *mesgBuffer, int bufferLen)
    {
        int nbytes = 0;
        u_long rbytes = 0;
        struct sockaddr_in tAddr;
        int addrlen = sizeof(tAddr);

        if (!initSuccess)
        {
            _tprintf(_T("udp slave no initsuccess\n"));
            return -1;
        }

        nbytes = recvfrom(fd, mesgBuffer, bufferLen, 0
                                        , (struct sockaddr *)&tAddr, (int *)&addrlen);
        if (nbytes <= 0)
            _tprintf(_T("udpslave recv failed\n"));

        return nbytes;
    }

    int send(char *mesgBuffer, int bufferLen, u_short bindPort)
    {
        int nbytes = 0;
        struct sockaddr_in sAddr;

        if (!initSuccess)
        {
            _tprintf(_T("udpslave no init success or invalid remote addr\n"));
            return -1;
        }

        memset(&sAddr, 0, sizeof(sAddr));
        sAddr.sin_family = AF_INET;
        sAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* N.B.: differs from sender */
        sAddr.sin_port = htons(bindPort);

        nbytes = sendto(fd, mesgBuffer, bufferLen, 0, (struct sockaddr *)&sAddr, sizeof(sAddr));
        if (nbytes <= 0)
        {
            _tprintf(_T("udpslave echo sendto failed\n"));
            return -1;
        }

        return nbytes;
    }

private:
    int fd;
    bool initSuccess;  
};

#endif
