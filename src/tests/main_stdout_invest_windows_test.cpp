//
// file:            cinternal_core_replace_function_windows.c
// path:			src/core/windows/cinternal_core_replace_function_windows.c
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/replace_function.h>
#include <cinternal/disable_compiler_warnings.h>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <cinternal/undisable_compiler_warnings.h>


static void ReplaceStdoutFunctions(void);

int main()
{
    ReplaceStdoutFunctions();
    printf("Hellow world 01!\n");
    fflush(stdout);
    fprintf(stdout,"Hellow world 02!\n");
    fprintf(stderr,"Hellow world 03!\n");
    //::std::cout<< "Hellow world 04!\n";
    //::std::cerr<< "Hellow world 05!\n";

    return 0;
}



typedef BOOL (WINAPI *TypeWriteConsole)(
    _In_             HANDLE  hConsoleOutput,
    _In_       const VOID* lpBuffer,
    _In_             DWORD   nNumberOfCharsToWrite,
    _Out_opt_        LPDWORD lpNumberOfCharsWritten,
    _Reserved_       LPVOID  lpReserved
);


typedef BOOL (WINAPI *TypeWriteConsoleOutputW)(
    _In_          HANDLE      hConsoleOutput,
    _In_    const CHAR_INFO* lpBuffer,
    _In_          COORD       dwBufferSize,
    _In_          COORD       dwBufferCoord,
    _Inout_       PSMALL_RECT lpWriteRegion
);


typedef BOOL (*TypeWriteFile)(
    HANDLE       hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
);


typedef BOOL (*TypeWriteFileEx)(
    HANDLE                          hFile,
    LPCVOID                         lpBuffer,
    DWORD                           nNumberOfBytesToWrite,
    LPOVERLAPPED                    lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

typedef int (*Typefprintf)(
    FILE*                          stream,
    const char*                    format,
    ...
    );


static BOOL WINAPI WriteConsoleAStatic(
    _In_             HANDLE  hConsoleOutput,
    _In_       const VOID* lpBuffer,
    _In_             DWORD   nNumberOfCharsToWrite,
    _Out_opt_        LPDWORD lpNumberOfCharsWritten,
    _Reserved_       LPVOID  lpReserved
);


static BOOL WINAPI WriteConsoleWStatic(
    _In_             HANDLE  hConsoleOutput,
    _In_       const VOID* lpBuffer,
    _In_             DWORD   nNumberOfCharsToWrite,
    _Out_opt_        LPDWORD lpNumberOfCharsWritten,
    _Reserved_       LPVOID  lpReserved
);

static BOOL WINAPI WriteConsoleOutputWStatic(
    _In_          HANDLE      hConsoleOutput,
    _In_    const CHAR_INFO* lpBuffer,
    _In_          COORD       dwBufferSize,
    _In_          COORD       dwBufferCoord,
    _Inout_       PSMALL_RECT lpWriteRegion
);

static BOOL WriteFileStatic(
    HANDLE       hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    );

static BOOL WriteFileExStatic(
    HANDLE                          hFile,
    LPCVOID                         lpBuffer,
    DWORD                           nNumberOfBytesToWrite,
    LPOVERLAPPED                    lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

static int fprintfStatic(
    FILE* stream,
    const char* format,
    ...
    );


static TypeWriteConsole s_originalWriteConsoleA = &WriteConsoleA;
static TypeWriteConsole s_originalWriteConsoleW = &WriteConsoleW;
static TypeWriteConsoleOutputW s_originalWriteConsoleOutputW = &WriteConsoleOutputW;
static TypeWriteFile s_originalWriteFile = &WriteFile;
static TypeWriteFileEx s_originalWriteFileEx = &WriteFileEx;
static Typefprintf s_originalfprintf = &fprintf;

#define NUMBER_OF_FUNCS_TO_REPLACE  1

static void ReplaceStdoutFunctions(void)
{
    struct SCInternalReplaceFunctionData aReplaceData[NUMBER_OF_FUNCS_TO_REPLACE];

    //aReplaceData[0].funcname = "WriteConsoleA";
    //aReplaceData[0].newFuncAddress = (void*)&WriteConsoleAStatic;
    //aReplaceData[0].replaceIfAddressIs = (void*)&WriteConsoleA;
    //
    //aReplaceData[1].funcname = "WriteConsoleW";
    //aReplaceData[1].newFuncAddress = (void*)&WriteConsoleWStatic;
    //aReplaceData[1].replaceIfAddressIs = (void*)&WriteConsoleW;
    //
    //aReplaceData[2].funcname = "WriteConsoleOutputW";
    //aReplaceData[2].newFuncAddress = (void*)&WriteConsoleOutputWStatic;
    //aReplaceData[2].replaceIfAddressIs = (void*)&WriteConsoleOutputW;
    //
    //aReplaceData[3].funcname = "WriteFile";
    //aReplaceData[3].newFuncAddress = (void*)&WriteFileStatic;
    //aReplaceData[3].replaceIfAddressIs = (void*)&WriteFile;
    //
    //aReplaceData[4].funcname = "WriteFileEx";
    //aReplaceData[4].newFuncAddress = (void*)&WriteFileExStatic;
    //aReplaceData[4].replaceIfAddressIs = (void*)&WriteFileEx;

    aReplaceData[0].funcname = "fprintf";
    aReplaceData[0].newFuncAddress = (void*)&fprintfStatic;
    aReplaceData[0].replaceIfAddressIs = (void*)&fprintf;

    CInternalReplaceFunctionsAllModules(NUMBER_OF_FUNCS_TO_REPLACE, aReplaceData);
}


static BOOL WINAPI WriteConsoleAStatic(
    _In_             HANDLE  a_hConsoleOutput,
    _In_       const VOID* a_lpBuffer,
    _In_             DWORD   a_nNumberOfCharsToWrite,
    _Out_opt_        LPDWORD a_lpNumberOfCharsWritten,
    _Reserved_       LPVOID  a_lpReserved
)
{
    return (*s_originalWriteConsoleA)(a_hConsoleOutput, a_lpBuffer,a_nNumberOfCharsToWrite,a_lpNumberOfCharsWritten,a_lpReserved);
}


static BOOL WINAPI WriteConsoleWStatic(
    _In_             HANDLE  a_hConsoleOutput,
    _In_       const VOID* a_lpBuffer,
    _In_             DWORD   a_nNumberOfCharsToWrite,
    _Out_opt_        LPDWORD a_lpNumberOfCharsWritten,
    _Reserved_       LPVOID  a_lpReserved
)
{
    return (*s_originalWriteConsoleW)(a_hConsoleOutput, a_lpBuffer, a_nNumberOfCharsToWrite, a_lpNumberOfCharsWritten, a_lpReserved);
}


static BOOL WINAPI WriteConsoleOutputWStatic(
    _In_          HANDLE      a_hConsoleOutput,
    _In_    const CHAR_INFO*  a_lpBuffer,
    _In_          COORD       a_dwBufferSize,
    _In_          COORD       a_dwBufferCoord,
    _Inout_       PSMALL_RECT a_lpWriteRegion
)
{
    return (*s_originalWriteConsoleOutputW)(a_hConsoleOutput,a_lpBuffer,a_dwBufferSize,a_dwBufferCoord,a_lpWriteRegion);
}


static BOOL WriteFileStatic(
    HANDLE       a_hFile,
    LPCVOID      a_lpBuffer,
    DWORD        a_nNumberOfBytesToWrite,
    LPDWORD      a_lpNumberOfBytesWritten,
    LPOVERLAPPED a_lpOverlapped
)
{
    return (*s_originalWriteFile)(a_hFile,a_lpBuffer,a_nNumberOfBytesToWrite,a_lpNumberOfBytesWritten,a_lpOverlapped);
}


static BOOL WriteFileExStatic(
    HANDLE                          a_hFile,
    LPCVOID                         a_lpBuffer,
    DWORD                           a_nNumberOfBytesToWrite,
    LPOVERLAPPED                    a_lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE a_lpCompletionRoutine
) 
{
    return (*s_originalWriteFileEx)(a_hFile, a_lpBuffer, a_nNumberOfBytesToWrite, a_lpOverlapped, a_lpCompletionRoutine);
}


static int fprintfStatic(
    FILE* a_stream,
    const char* a_format,
    ...
)
{
    int nRet;
    va_list argptr;
    va_start(argptr, a_format);
    nRet = vfprintf(a_stream, a_format, argptr);
    va_end(argptr);
    return nRet;
}


CPPUTILS_BEGIN_C

CPPUTILS_C_CODE_INITIALIZER(main_stdout_invest_windows_test_init) {
    s_originalWriteConsoleA = &WriteConsoleA;
    s_originalWriteConsoleW = &WriteConsoleW;
    s_originalWriteConsoleOutputW = &WriteConsoleOutputW;
    s_originalWriteFile = &WriteFile;
    s_originalWriteFileEx = &WriteFileEx;
    s_originalfprintf = &fprintf;
}

CPPUTILS_END_C
