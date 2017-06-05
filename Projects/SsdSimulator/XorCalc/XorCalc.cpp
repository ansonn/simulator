// XorCalc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winnt.h>

int _tmain(int argc, _TCHAR* argv[])
{
    char dst = 0;
    char src = 0;
    InterlockedXor8(&dst, &src);
    InterlockedXor8(dst, );
    InterlockedXor8(dst, );
    InterlockedXor8(dst, );

	return 0;
}

