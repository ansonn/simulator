#include "udpendpoint.h"
#include <stdio.h>
#pragma comment(lib, "WSOCK32.lib")
int main(int argc, char *argv[])
{
    char buffer[256] = { 0 };
    int recvLen = 0;

    udpEndpoint udpSlave;
    udpEndpoint udpSlave0;
    udpEndpoint udpSlave1;
    udpEndpoint udpSlave2;

    udpSlave.init(6000);
    while (1)
    {
        _tprintf(_T("."));
        memset(buffer, 0x00, 256);

        recvLen = udpSlave.recv_nb(buffer, 256);
        if (recvLen < 0)
        {
            _tprintf(_T("udpSlave recv failed\n"));
        }
        else if (recvLen == 0)
        {
            Sleep(300);
            continue;
        }
        
        buffer[recvLen] = '\0';
        printf("udpSlave recv %s, len=%d\n", buffer, recvLen);

        udpSlave.send("ack\n", sizeof("ack\n"), 7000);

    }
    return 0;
}