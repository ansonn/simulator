// ConsoleMouse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
    int idx = 2;
    TCHAR buff[MAX_PATH];


    swprintf_s(buff, MAX_PATH, _T("%s %d"), _T("hello world"), idx);

    _tprintf(_T("%s"), buff);

    system("pause");
#if 0
    HANDLE hStdin;
    DWORD fdwSaveOldMode;
    DWORD fdwMode;


    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("GetStdHandle"));
        return -1;
    }
    // Save the current input mode, to be restored on exit. 

    if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
    {
        _tprintf(_T("GetConsoleMode"));
        return -1;
    }

    // Enable the window and mouse input events. 

    fdwMode = ENABLE_EXTENDED_FLAGS;
    if (!SetConsoleMode(hStdin, fdwMode))
    {
        _tprintf(_T("SetConsoleMode"));
        return -1;
    }



    while (true)
    {
        printf("hello world\n");
        Sleep(1000);
    }
#endif

	return 0;
}

