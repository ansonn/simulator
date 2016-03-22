#include <windows.h>
#include <stdio.h>
#include "simcontrolIpc.h"


int main()
{
    unsigned int status = 0;
    char buffer[32];
    unsigned int cnt = 0;

    simControlIPC controlIpc;

    controlIpc.open(CONSOLE_MB, 32);

    system("pause");

    while (1)
    {

        switch (status)
        {
        case 0:
            controlIpc.writeSlot(SERVER_MB, "server", 6);
            controlIpc.writeSlot(FIMANAGER_MB, "FI", 2);
            controlIpc.writeSlot(CONSOLE_MB, "console", 6);
            status = 1;
            break;

        case 1:
            memset(buffer, 0x00, 32);
            if (controlIpc.readSlot(buffer))
            {
                cnt++;
                printf("%s\n", buffer);
            }
            if (cnt >= 3)
                status = 2;
            break;
            
        case 2:
            break;
        }

    }


    return 0;
}

