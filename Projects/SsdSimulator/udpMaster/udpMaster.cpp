#include "udpendpoint.h"
#include <stdio.h>

#pragma comment(lib, "WSOCK32.lib")

int main(int argc, char *argv[])
{
    int nbytes = 0;
    char buff[256] = { 0 };
    udpEndpoint udpMaster;
    udpEndpoint udpMaster0;
    udpEndpoint udpMaster1;

    udpMaster.init(7000);
    while (1)
    {
        udpMaster.send("hello\n", sizeof("hello\n"), 6000);

        nbytes = udpMaster.recv(buff, 256);
        if (nbytes < 0)
            printf("master recv faild\n");
        buff[nbytes] = '\0';


        printf("-->recv ack %s\n", buff);

        Sleep(1000);
    }
    return 0;
}