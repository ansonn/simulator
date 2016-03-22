/*---------------------------------------------------------------------------*/
//  ipc endpoint
//
//  CopyRight   : bj.memblaze.com
//  Author      :
//
//  Notes       :
//      2015-12-02  Created by shuke.wang@memblaze.com
/*---------------------------------------------------------------------------*/
#pragma once
#include <Windows.h>
#include <tchar.h>

enum {
    CONSOLE_MB = 0,
    SERVER_MB,
    FIMANAGER_MB,
    MB_BUTT
};

class simControlIPC 
{
public:
    simControlIPC()
    {
        for (unsigned int i = 0; i < MB_BUTT; i++)
        {
            mailBoxName[i][0] = _T('\0');
            hdSender[i] = INVALID_HANDLE_VALUE;
        }
        hdReceiver = INVALID_HANDLE_VALUE;
    }

    ~simControlIPC()
    {
        close();
    }

    inline bool open(unsigned int mbType, ULONG maxReceiveSlotSize, ULONG timeOut = 0)
    {

        for (unsigned int i = 0; i < MB_BUTT; i++)
        {
            swprintf_s(mailBoxName[i], MAX_PATH, _T("\\\\.\\mailslot\\_MBOX%d"), i);
        }
        hdReceiver = CreateMailslot(mailBoxName[mbType], maxReceiveSlotSize, timeOut, (LPSECURITY_ATTRIBUTES)NULL);
        if (hdReceiver == INVALID_HANDLE_VALUE)
        {
            close();
            return false;
        }

        return true;
    }

    inline void close(void)
    {
        for (unsigned int i = 0; i < MB_BUTT; i++)
        {
            if (hdSender[i] != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hdSender[i]);
                hdSender[i] = INVALID_HANDLE_VALUE;
            }
            mailBoxName[i][0] = _T('\0');
        }
        if (hdReceiver != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hdReceiver);
            hdReceiver = INVALID_HANDLE_VALUE;
        }
    }

    inline bool writeSlot(unsigned int mbType, void *message, ULONG size)
    {
        if (hdSender[mbType] == INVALID_HANDLE_VALUE && _tcslen(mailBoxName[mbType]) != 0)
        {
            hdSender[mbType] = CreateFile(mailBoxName[mbType], GENERIC_WRITE, FILE_SHARE_READ
                                                             , (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
                                                             , (HANDLE)NULL);
        }

        if (hdSender[mbType] == INVALID_HANDLE_VALUE)
            return false;

        ULONG cbWritten;
        bool fResult = WriteFile( hdSender[mbType], message, size, &cbWritten, NULL) != FALSE;
        if (cbWritten != size)
            return false;

        return fResult;
    }

    inline bool readSlot(void *message)
    {
        if (hdReceiver == INVALID_HANDLE_VALUE)
            return false;

        bool fResult;
        ULONG cbSize;
        fResult = GetMailslotInfo(hdReceiver, NULL, &cbSize, NULL, NULL) != FALSE;
        if (!fResult || cbSize == MAILSLOT_NO_MESSAGE)
            return false;

        ULONG cbRead;
        fResult = ReadFile(hdReceiver, message, cbSize, &cbRead, NULL) != FALSE;
        if (cbSize != cbRead)
            return false;

        return fResult;
    }

private:
    TCHAR mailBoxName[MB_BUTT][MAX_PATH];
    HANDLE hdSender[MB_BUTT];
    HANDLE hdReceiver;
};

