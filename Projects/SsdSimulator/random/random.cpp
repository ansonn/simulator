// random.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int _tmain(int argc, _TCHAR* argv[])
{
    srand(time(NULL));
    
    for (int i = 0; i < 50; i++)
    {
        printf("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10
            , rand() % 10);
    }
    printf("sizeof(LONG)=%d\n", sizeof(long));

    system("pause");

    return 0;
}

