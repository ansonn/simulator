/*---------------------------------------------------------------------------*/
//  For Test
//  shuke.wang@memblaze.com
//
/*---------------------------------------------------------------------------*/
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#define MEMORY_REQUESTED        ((2*1024 + 512)*1024*1024)
#define MEMORY_VIRTUAL          1024*1024*512 


BOOL SetPrivilege(HANDLE hProcess, BOOL bEnable)
{
    struct {
        DWORD Count;
        LUID_AND_ATTRIBUTES Privilege[1];
    } Info;

    HANDLE Token;
    BOOL Result;

    // 打开进程的安全信息  
    Result = OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token);
    if (Result != TRUE)
    {
        _tprintf(_T("Cannot open process token.\n"));
        return FALSE;
    }

    //enable 1 attributes
    Info.Count = 1;
    if (bEnable)
        Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        Info.Privilege[0].Attributes = 0;

    // 获得LUID
    Result = LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &(Info.Privilege[0].Luid));
    if (Result != TRUE)
    {
        _tprintf(_T("Cannot get privilege for %s.\n"), SE_LOCK_MEMORY_NAME);
        return FALSE;
    }

    // 修改权限  
    Result = AdjustTokenPrivileges(Token, FALSE, (PTOKEN_PRIVILEGES)&Info, 0, NULL, NULL);
    if (Result != TRUE)
    {
        _tprintf(_T("Cannot adjust token privileges (%u)\n"), GetLastError());
        return FALSE;
    }
    else
    {
        if (GetLastError() != ERROR_SUCCESS)
        {
            _tprintf(_T("Cannot enable the SE_LOCK_MEMORY_NAME privilege  %d; \n"), GetLastError());
            _tprintf(_T("please check the local policy.\n"));
            return FALSE;
        }
    }
    CloseHandle(Token);

    return TRUE;
}


void testAllocMoreThan4GMem(void)
{
    BOOL bResult;                   // 通用bool变量  
    ULONG_PTR NumberOfPages;        // 申请的内存页数  
    __int64 NumberOfPagesInitial;   // 初始的要申请的内存页数  
    ULONG_PTR *aPFNs;               // 页信息,存储获取的内存页成员  
    PVOID lpMemReserved;            // AWE窗口  
    SYSTEM_INFO sSysInfo;           // 系统信息  
    INT PFNArraySize;               // PFN队列所占的内存长度  

    GetSystemInfo(&sSysInfo);  // 获取系统信息  
    _tprintf(_T("This computer has page size %d.\n"), sSysInfo.dwPageSize);

    //计算要申请的内存页数.  
    NumberOfPages = MEMORY_REQUESTED / sSysInfo.dwPageSize;
    _tprintf(_T("Requesting %d pages of memory.\n"), NumberOfPages);

    // 计算PFN队列所占的内存长度  
    PFNArraySize = NumberOfPages * sizeof(ULONG_PTR);
    _tprintf(_T("Requesting a PFN array of %d bytes.\n"), PFNArraySize);

    aPFNs = (ULONG_PTR *)HeapAlloc(GetProcessHeap(), 0, PFNArraySize);
    if (aPFNs == NULL)
    {
        _tprintf(_T("Failed to allocate on heap.\n"));
        return;
    }

    // 开启"锁定内存页"权限  

    if (!SetPrivilege(GetCurrentProcess(), TRUE))
    {
        HeapFree(GetProcessHeap(), 0, aPFNs);
        return;
    }

    // 分配物理内存,长度2.5GB  
    NumberOfPagesInitial = NumberOfPages;
    bResult = AllocateUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, aPFNs);
    if (bResult != TRUE)
    {
        _tprintf(_T("Cannot allocate physical pages (%u)\n"), GetLastError());
        return;
    }

    if (NumberOfPagesInitial != NumberOfPages)
    {
        _tprintf(_T("Allocated only %p pages.\n"), NumberOfPages);
        return;
    }

    // 保留长度0.5GB的虚拟内存块(这个内存块即AWE窗口)的地址  

    lpMemReserved = VirtualAlloc(NULL, MEMORY_VIRTUAL, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);
    if (lpMemReserved == NULL)
    {
        _tprintf(_T("Cannot reserve memory.\n"));
        return;
    }

    char *strTemp;
    for (int i = 0; i< 5; i++)
    {
        // 把物理内存映射到窗口中来  
        // 分5次映射,每次映射0.5G物理内存到窗口中来.  
        // 注意,在整个过程中,lpMenReserved的值都是不变的  
        // 但是映射的实际物理内存却是不同的  
        // 这段代码将申请的2.5G物理内存分5段依次映射到窗口中来  
        // 并在每段的开头写入一串字符串.  
        bResult = MapUserPhysicalPages(lpMemReserved, NumberOfPages/5, aPFNs + NumberOfPages / 5 * i);
        if (bResult != TRUE)
        {
            _tprintf(_T("MapUserPhysicalPages failed (%u)\n"), GetLastError());
            return;
        }

        // 写入字符串,虽然是写入同一个虚存地址,  
        // 但是窗口映射的实际内存不同,所以是写入了不同的内存块中  
        strTemp = (char*)lpMemReserved;
        sprintf_s(strTemp, sizeof("This is the %dth section!"), "This is the %dth section!", i + 1);

        // 解除映射  

        bResult = MapUserPhysicalPages(lpMemReserved,
            NumberOfPages / 5,
            NULL);

        if (bResult != TRUE)
        {
            _tprintf(_T("MapUserPhysicalPages failed (%u)\n"), GetLastError());
            return;
        }
    }

    // 现在再从5段内存中读出刚才写入的字符串  
    for (int i = 0; i<5; i++)
    {
        // 把物理内存映射到窗口中来  

        bResult = MapUserPhysicalPages(lpMemReserved,
            NumberOfPages / 5,
            aPFNs + NumberOfPages / 5 * i);

        if (bResult != TRUE)
        {
            _tprintf(_T("MapUserPhysicalPages failed (%u)\n"), GetLastError());
            return;
        }

        // 将映射到窗口中的不同内存块的字符串在屏幕中打印出来  
        strTemp = (char*)lpMemReserved;
        _tprintf(_T("%s\n"), strTemp);

        // 解除映射  

        bResult = MapUserPhysicalPages(lpMemReserved,
            NumberOfPages / 5,
            NULL);

        if (bResult != TRUE)
        {
            _tprintf(_T("MapUserPhysicalPages failed (%u)/n"), GetLastError());
            return;
        }
    }

    system("pause");
    // 释放物理内存空间  

    bResult = FreeUserPhysicalPages(GetCurrentProcess(),
        &NumberOfPages,
        aPFNs);

    if (bResult != TRUE)
    {
        _tprintf(_T("Cannot free physical pages, error %u./n"), GetLastError());
        return;
    }

    // 释放虚拟内存地址  

    bResult = VirtualFree(lpMemReserved,
        0,
        MEM_RELEASE);

    // 释放PFN队列空间  

    bResult = HeapFree(GetProcessHeap(), 0, aPFNs);

    if (bResult != TRUE)
    {
        _tprintf(_T("Call to HeapFree has failed (%u)/n"), GetLastError());
    }

}



#include <windows.h>
#include <tchar.h>
#include <stdio.h>

VOID
__stdcall
ReadFiberFunc(LPVOID lpParameter);

VOID
__stdcall
WriteFiberFunc(LPVOID lpParameter);

void DisplayFiberInfo(void);

typedef struct
{
    DWORD dwParameter;          // DWORD parameter to fiber (unused)
    DWORD dwFiberResultCode;    // GetLastError() result code
    HANDLE hFile;               // handle to operate on
    DWORD dwBytesProcessed;     // number of bytes processed
} FIBERDATASTRUCT, *PFIBERDATASTRUCT, *LPFIBERDATASTRUCT;

#define RTN_OK 0
#define RTN_USAGE 1
#define RTN_ERROR 13

#define BUFFER_SIZE 32768   // read/write buffer size
#define FIBER_COUNT 3       // max fibers (including primary)

#define PRIMARY_FIBER 0 // array index to primary fiber
#define READ_FIBER 1    // array index to read fiber
#define WRITE_FIBER 2   // array index to write fiber

LPVOID g_lpFiber[FIBER_COUNT];
LPBYTE g_lpBuffer;
DWORD g_dwBytesRead;

int __cdecl forFiberTest(int argc, TCHAR *argv[])
{
    LPFIBERDATASTRUCT fs;

    if (argc != 3)
    {
        printf("Usage: %s <SourceFile> <DestinationFile>\n", argv[0]);
        return RTN_USAGE;
    }

    //
    // Allocate storage for our fiber data structures
    //
    fs = (LPFIBERDATASTRUCT)HeapAlloc(
        GetProcessHeap(), 0,
        sizeof(FIBERDATASTRUCT) * FIBER_COUNT);

    if (fs == NULL)
    {
        printf("HeapAlloc error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    //
    // Allocate storage for the read/write buffer
    //
    g_lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE);
    if (g_lpBuffer == NULL)
    {
        printf("HeapAlloc error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    //
    // Open the source file
    //
    fs[READ_FIBER].hFile = CreateFile(
        argv[1],
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
        );

    if (fs[READ_FIBER].hFile == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    //
    // Open the destination file
    //
    fs[WRITE_FIBER].hFile = CreateFile(
        argv[2],
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
        );

    if (fs[WRITE_FIBER].hFile == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    //
    // Convert thread to a fiber, to allow scheduling other fibers
    //
    g_lpFiber[PRIMARY_FIBER] = ConvertThreadToFiber(&fs[PRIMARY_FIBER]);

    if (g_lpFiber[PRIMARY_FIBER] == NULL)
    {
        printf("ConvertThreadToFiber error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    //
    // Initialize the primary fiber data structure.  We don't use
    // the primary fiber data structure for anything in this sample.
    //
    fs[PRIMARY_FIBER].dwParameter = 0;
    fs[PRIMARY_FIBER].dwFiberResultCode = 0;
    fs[PRIMARY_FIBER].hFile = INVALID_HANDLE_VALUE;

    //
    // Create the Read fiber
    //
    g_lpFiber[READ_FIBER] = CreateFiber(0, ReadFiberFunc, &fs[READ_FIBER]);

    if (g_lpFiber[READ_FIBER] == NULL)
    {
        printf("CreateFiber error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    fs[READ_FIBER].dwParameter = 0x12345678;

    //
    // Create the Write fiber
    //
    g_lpFiber[WRITE_FIBER] = CreateFiber(0, WriteFiberFunc, &fs[WRITE_FIBER]);

    if (g_lpFiber[WRITE_FIBER] == NULL)
    {
        printf("CreateFiber error (%d)\n", GetLastError());
        return RTN_ERROR;
    }

    fs[WRITE_FIBER].dwParameter = 0x54545454;

    //
    // Switch to the read fiber
    //
    SwitchToFiber(g_lpFiber[READ_FIBER]);

    //
    // We have been scheduled again. Display results from the 
    // read/write fibers
    //
    printf("ReadFiber: result code is %lu, %lu bytes processed\n",
        fs[READ_FIBER].dwFiberResultCode, fs[READ_FIBER].dwBytesProcessed);

    printf("WriteFiber: result code is %lu, %lu bytes processed\n",
        fs[WRITE_FIBER].dwFiberResultCode, fs[WRITE_FIBER].dwBytesProcessed);

    //
    // Delete the fibers
    //
    DeleteFiber(g_lpFiber[READ_FIBER]);
    DeleteFiber(g_lpFiber[WRITE_FIBER]);

    //
    // Close handles
    //
    CloseHandle(fs[READ_FIBER].hFile);
    CloseHandle(fs[WRITE_FIBER].hFile);

    //
    // Free allocated memory
    //
    HeapFree(GetProcessHeap(), 0, g_lpBuffer);
    HeapFree(GetProcessHeap(), 0, fs);

    return RTN_OK;
}

VOID
__stdcall
ReadFiberFunc(
LPVOID lpParameter
)
{
    LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;

    //
    // If this fiber was passed NULL for fiber data, just return,
    // causing the current thread to exit
    //
    if (fds == NULL)
    {
        printf("Passed NULL fiber data; exiting current thread.\n");
        return;
    }

    //
    // Display some information pertaining to the current fiber
    //
    DisplayFiberInfo();

    fds->dwBytesProcessed = 0;

    while (1)
    {
        //
        // Read data from file specified in the READ_FIBER structure
        //
        if (!ReadFile(fds->hFile, g_lpBuffer, BUFFER_SIZE,
            &g_dwBytesRead, NULL))
        {
            break;
        }

        //
        // if we reached EOF, break
        //
        if (g_dwBytesRead == 0) break;

        //
        // Update number of bytes processed in the fiber data structure
        //
        fds->dwBytesProcessed += g_dwBytesRead;

        //
        // Switch to the write fiber
        //
        SwitchToFiber(g_lpFiber[WRITE_FIBER]);
    } // while

    //
    // Update the fiber result code
    //
    fds->dwFiberResultCode = GetLastError();

    //
    // Switch back to the primary fiber
    //
    SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}

VOID __stdcall WriteFiberFunc(LPVOID lpParameter)
{
    LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;
    DWORD dwBytesWritten;

    // If this fiber was passed NULL for fiber data, just return,
    // causing the current thread to exit
    if (fds == NULL)
    {
        printf("Passed NULL fiber data; exiting current thread.\n");
        return;
    }

    // Display some information pertaining to the current fiber
    DisplayFiberInfo();

    // Assume all writes succeeded.  If a write fails, the fiber
    // result code will be updated to reflect the reason for failure
    fds->dwBytesProcessed = 0;
    fds->dwFiberResultCode = ERROR_SUCCESS;

    while (1)
    {
        // Write data to the file specified in the WRITE_FIBER structure
        if (!WriteFile(fds->hFile, g_lpBuffer, g_dwBytesRead, &dwBytesWritten, NULL))
        {
            //
            // If an error occurred writing, break
            //
            break;
        }

        //
        // Update number of bytes processed in the fiber data structure
        //
        fds->dwBytesProcessed += dwBytesWritten;

        //
        // Switch back to the read fiber
        //
        SwitchToFiber(g_lpFiber[READ_FIBER]);
    }  // while

    // If an error occurred, update the fiber result code...
    fds->dwFiberResultCode = GetLastError();

    // ...and switch to the primary fiber
    SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}

void
DisplayFiberInfo(
void
)
{
    LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)GetFiberData();
    LPVOID lpCurrentFiber = GetCurrentFiber();

    //
    // Determine which fiber is executing, based on the fiber address
    //
    if (lpCurrentFiber == g_lpFiber[READ_FIBER])
        printf("Read fiber entered");
    else
    {
        if (lpCurrentFiber == g_lpFiber[WRITE_FIBER])
            printf("Write fiber entered");
        else
        {
            if (lpCurrentFiber == g_lpFiber[PRIMARY_FIBER])
                printf("Primary fiber entered");
            else
                printf("Unknown fiber entered");
        }
    }

    //
    // Display dwParameter from the current fiber data structure
    //
    printf(" (dwParameter is 0x%lx)\n", fds->dwParameter);
}


int _tmain(int argc, _TCHAR* argv[])
{
    _tprintf(_T("ULONG_PTR size = %d\n"), sizeof(ULONG_PTR));
    //MessageBox(NULL, _T("hello world!\n"), _T("ASSERTION TEST"), MB_OK);
    //testAllocMoreThan4GMem();


    forFiberTest(argc, argv);


#if 0
    _tprintf(_T("long size = %d\n"), sizeof(long));

    char *ptr1 = new char[0xB0000000];
    char *ptr2 = new char[0xB0000000];
    //MessageBox(NULL, _T("hello world!\n"), _T("ASSERTION TEST"), MB_OK);

    delete ptr1;
    delete ptr2;
#endif
    system("pause");

	return 0;
}

