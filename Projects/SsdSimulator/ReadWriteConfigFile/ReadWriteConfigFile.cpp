// ReadWriteConfigFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ReadWriteConfigFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef unsigned int u32;
typedef unsigned char u8;

void testMain(void);
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
            testMain();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
static inline u32 tryConvertToDigits(LPCTSTR inputStr, TCHAR **stopPtr, bool &successful)
{
    TCHAR *scanStopPtr;
    u32 tempValue = (u32)_tcstoui64(inputStr, &scanStopPtr, 0);

    successful = (inputStr != scanStopPtr && tempValue < 0x100000000ULL);

    if (stopPtr != NULL)
    {
        *stopPtr = scanStopPtr;
    }

    return (u32)tempValue;
}

#define NOR_FLASH_PAGE_BUF_SIZE     1024
void simNorflashWrite(u32 sectorIdx, u32 pageIdx, u8 *data, u32 len)
{
    TCHAR addrBuf[64] = _T("");
    TCHAR pageData[NOR_FLASH_PAGE_BUF_SIZE] = _T("");

    ASSERT(len > 0);
    _snwprintf_s(addrBuf, sizeof(addrBuf), _T("sector%d_page%d"), sectorIdx, pageIdx);
    for (u32 i = 0; i < len; i++)
    {
        TCHAR tBuf[32] = { 0 };
        _snwprintf_s(tBuf, sizeof(tBuf), _T("0x%02x,"), data[i]);
        _tcsncat_s(pageData, NOR_FLASH_PAGE_BUF_SIZE, tBuf, 32);
    }
    WritePrivateProfileString(_T("norFlash"), addrBuf, pageData, _T(".\\config_1.ini"));
}

void simNorflashRead(u32 sectorIdx, u32 pageIdx, u8 *data, u32 len)
{
    TCHAR addrBuf[64] = { 0 };
    TCHAR pageData[NOR_FLASH_PAGE_BUF_SIZE] = { 0 };

    _snwprintf_s(addrBuf, sizeof(addrBuf), _T("sector%d_page%d"), sectorIdx, pageIdx);
    GetPrivateProfileString(_T("norFlash"), addrBuf, _T(""), pageData, NOR_FLASH_PAGE_BUF_SIZE, _T(".\\config_1.ini"));

    if (_tcscmp(pageData, _T("")) == 0)
    {
        FillMemory(data, len, 0xFF);
        return;
    }

    bool successful = false;
    TCHAR *startPtr = pageData;
    TCHAR *stopPtr = nullptr;

    for (u32 i = 0; i < len; i++)
    {
        data[i] = (u8)_tcstoui64(startPtr, &stopPtr, 0);
        ASSERT(startPtr != stopPtr);
        startPtr = stopPtr;
        while (*startPtr == ',')
            startPtr++;
    }

}

void testMain(void)
{
/*
    TCHAR path[MAX_PATH] = { 0 };
    TCHAR sectionPair[2048];
    TCHAR filterText[1024];
    UINT32 len;
    TCHAR *pConfigFile = _T(".\\config.ini");
    TCHAR *pcur;

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    hFind = FindFirstFile(pConfigFile, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE || PathIsFileSpec(pConfigFile))
    {
        _tprintf(_T("File %s is not exist or path is invalid\n"), pConfigFile);
        FindClose(hFind);
        exit(-1);
    }
    FindClose(hFind);
    _tcsncpy_s(path, pConfigFile, MAX_PATH);
*/
    u8 dataIn[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    u8 dataOut[8] = { 0 };
    simNorflashWrite(1, 0, dataIn, 8);
    simNorflashWrite(1, 1, dataIn, 8);
    simNorflashWrite(1, 2, dataIn, 8);
    simNorflashRead(1, 0, dataOut, 8);
}



