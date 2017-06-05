// apitest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>

int _tmain(int argc, _TCHAR* argv[])
{
    time_t startTime = time(NULL);
    while (1)
    {
        if (time(NULL) - startTime > 10)
        {
            printf_s("--->timeout\n");
            break;
        }
    }

    system("pause");
	return 0;
}

