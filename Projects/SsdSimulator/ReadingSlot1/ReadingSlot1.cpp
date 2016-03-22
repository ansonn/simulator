#include <windows.h>
#include <stdio.h>
#include "simcontrolIpc.h"


int main()
{
    unsigned int status = 0;
    char buffer[32] = { 0 };
    unsigned int cnt = 0;

    simControlIPC controlIpc;

    controlIpc.open(SERVER_MB, 32);

    while (1)
    {
        if (controlIpc.readSlot(buffer))
        {
            printf("%s\n", buffer);
            controlIpc.writeSlot(CONSOLE_MB, "SERVER_ACK", 10);
            memset(buffer, 0x00, 32);
        }

    }


    return 0;
}

