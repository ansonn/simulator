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
#define SIM_NOR_SECTOR_SIZE 4096
#define SIM_NOR_SECTOR_ADDR_MASK 0x0FFF
#define SIM_NOR_SECTOR_ERASED 1
#define SIM_NOR_SECTOR_MAX 1024

void simNorflashChipErase(void)
{
    u8 buf[SIM_NOR_SECTOR_SIZE];
    memset(buf, 0xFF, SIM_NOR_SECTOR_SIZE);
    
    FILE *fp;
    fopen_s(&fp, "norFlashData.bin", "wb");
    for (u32 i = 0; i < SIM_NOR_SECTOR_MAX; i++)
    {
        fwrite(buf, 1, SIM_NOR_SECTOR_SIZE, fp);
    }
    fclose(fp);
}

void simNorflashSectErase(u32 address)
{
    u8 buf[SIM_NOR_SECTOR_SIZE];
    memset(buf, 0xFF, SIM_NOR_SECTOR_SIZE);

    FILE *fp;
    fopen_s(&fp, "norFlashData.bin", "wb");
    fseek(fp, address & (~SIM_NOR_SECTOR_ADDR_MASK), SEEK_SET);
    fwrite(buf, 1, SIM_NOR_SECTOR_SIZE, fp);
    fclose(fp);
}

void simNorflashWrite(u32 address, void *data, u32 len)
{
    u8 buf[SIM_NOR_SECTOR_SIZE];
    ASSERT(len <= SIM_NOR_SECTOR_SIZE);

    FILE *fp;
    ASSERT(fopen_s(&fp, "norFlashData.bin", "rb+") == 0);
    fseek(fp, address, SEEK_SET);
    fread(buf, 1, len, fp);

    for (u32 i = 0; i < len; i++)
    {
        if (buf[i] != 0xFF)
            ASSERT(0);
    }

    fseek(fp, address, SEEK_SET);
    fwrite(data, 1, len, fp);
    fclose(fp);
}

void simNorflashRead(u32 address, void *data, u32 len)
{
    FILE *fp;
    fopen_s(&fp, "norFlashData.bin", "rb");
    fseek(fp, address, SEEK_SET);
    fread(data, 1, len, fp);
    fclose(fp);
}

void testMain(void)
{
    simNorflashChipErase();

    u8 buf[SIM_NOR_SECTOR_SIZE] = { 0 };
    memset(buf, 0xaa, SIM_NOR_SECTOR_SIZE);
    simNorflashWrite(4, buf, 4);
}



