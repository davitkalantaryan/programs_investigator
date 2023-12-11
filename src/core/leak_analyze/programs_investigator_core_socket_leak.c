//
// file:			crash_investigator_linux_simple_analyze.cpp
// path:			src/core/crash_investigator_linux_simple_analyze.cpp
// created on:		2023 Mar 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <progs_invest/analyze_leaking02.h>
#include <cinternal/logger.h>
#include <cinternal/replace_function_sys.h>
#include <stdlib.h>
#include <assert.h>


CPPUTILS_BEGIN_C


typedef SOCKET (WSAAPI *TypeSocket)(int af,int type,int protocol);
typedef int (*TypeClosesocket)(SOCKET s);
typedef HMODULE(WINAPI* TypeLoadLibraryA)(LPCSTR lpLibFileName);
typedef HMODULE(WINAPI* TypeLoadLibraryW)(LPWSTR lpLibFileName);
typedef HMODULE(WINAPI* TypeLoadLibraryExA)(LPCSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);
typedef HMODULE(WINAPI* TypeLoadLibraryExW)(LPWSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);

static struct SPrInvLeakAnalyzerData* s_pTable = CPPUTILS_NULL;
static TypeSocket s_socket_original;
static TypeClosesocket s_closesocket_original;
static TypeLoadLibraryA		s_loadlibA_st_lib	/*= &LoadLibraryA		*/;
static TypeLoadLibraryW		s_loadlibW_st_lib	/*= &LoadLibraryW		*/;
static TypeLoadLibraryExA	s_loadlibExA_st_lib /*= &LoadLibraryExA		*/;
static TypeLoadLibraryExW	s_loadlibExW_st_lib	/*= &LoadLibraryExW		*/;


static SOCKET WSAAPI SockLeakHookCrtSocketStatic(int a_af, int a_type, int a_protocol) CPPUTILS_NOEXCEPT
{
    const SOCKET pRet = (*s_socket_original)(a_af,a_type, a_protocol);
    if (pRet) {
        ProgsInvestLeakAnalyzeAddResource(s_pTable, CPPUTILS_REINTERPRET_CAST(void*, pRet), 1);
    }
    return pRet;
}


static int SockLeakHookRemSocketStatic(SOCKET a_sock) CPPUTILS_NOEXCEPT
{
    if (a_sock) {
        ProgsInvestLeakAnalyzeRemoveResource(s_pTable, CPPUTILS_REINTERPRET_CAST(void*, a_sock));
    }
    return (*s_closesocket_original)(a_sock);
}


static inline void ReplaceSocketFunctionsForSingleModuleInline(HMODULE a_retMod) {
    struct SCInternalReplaceFunctionData vReplaceData[2];

    vReplaceData[0].funcname = "socket";
    vReplaceData[0].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, s_socket_original);
    vReplaceData[0].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookCrtSocketStatic);

    vReplaceData[1].funcname = "closesocket";
    vReplaceData[1].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, s_closesocket_original);
    vReplaceData[1].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookRemSocketStatic);

    CInternalReplaceFunctionsForModule(a_retMod,2, vReplaceData);
}


static HMODULE WINAPI CinternalLoadLibraryA(LPCSTR a_lpLibFileName)
{
    const HMODULE retMod = (*s_loadlibA_st_lib)(a_lpLibFileName);
    if (retMod) {
        ReplaceSocketFunctionsForSingleModuleInline(retMod);
    }

    return retMod;
}


static HMODULE WINAPI CinternalLoadLibraryW(LPWSTR a_lpLibFileName)
{
    const HMODULE retMod = (*s_loadlibW_st_lib)(a_lpLibFileName);
    if (retMod) {
        ReplaceSocketFunctionsForSingleModuleInline(retMod);
    }

    return retMod;
}


static HMODULE WINAPI CinternalLoadLibraryExA(LPCSTR a_lpLibFileName, HANDLE a_hFile, DWORD  a_dwFlags)
{
    const HMODULE retMod = (*s_loadlibExA_st_lib)(a_lpLibFileName, a_hFile, a_dwFlags);
    if (retMod) {
        ReplaceSocketFunctionsForSingleModuleInline(retMod);
    }

    return retMod;
}


static HMODULE WINAPI CinternalLoadLibraryExW(LPWSTR a_lpLibFileName, HANDLE a_hFile, DWORD  a_dwFlags)
{
    const HMODULE retMod = (*s_loadlibExW_st_lib)(a_lpLibFileName, a_hFile, a_dwFlags);
    if (retMod) {
        ReplaceSocketFunctionsForSingleModuleInline(retMod);
    }

    return retMod;
}


static void programs_investigator_core_memory_leak_clean(void) CPPUTILS_NOEXCEPT {

    struct SCInternalReplaceFunctionData vReplaceData[2];

    vReplaceData[0].funcname = "socket";
    vReplaceData[0].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, s_socket_original);
    vReplaceData[0].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookCrtSocketStatic);

    vReplaceData[1].funcname = "closesocket";
    vReplaceData[1].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, s_closesocket_original);
    vReplaceData[1].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookRemSocketStatic);

    CInternalReplaceFunctionsAllModules(2, vReplaceData);


    ProgsInvestLeakAnalyzeCleanup(s_pTable);
    s_pTable = CPPUTILS_NULL;
}



CPPUTILS_C_CODE_INITIALIZER(programs_investigator_core_socket_leak_init){

    struct SCInternalReplaceFunctionData vReplaceData[4];
        
    s_pTable = ProgsInvestLeakAnalyzeInitialize(1, CPPUTILS_NULL, CPPUTILS_NULL, CPPUTILS_NULL);
    CInternalLogDebug("s_pTable = %p", s_pTable);
    if (!s_pTable) {
        CInternalLogError("Table is not created");
        exit(1);
    }

    s_socket_original = &socket;
    s_closesocket_original = &closesocket;
    s_loadlibA_st_lib = &LoadLibraryA;
    s_loadlibW_st_lib = CPPUTILS_REINTERPRET_CAST(TypeLoadLibraryW, &LoadLibraryW);
    s_loadlibExA_st_lib = &LoadLibraryExA;
    s_loadlibExW_st_lib = CPPUTILS_REINTERPRET_CAST(TypeLoadLibraryExW ,&LoadLibraryExW);


    // LoadLibrary and friends
    vReplaceData[0].funcname = "LoadLibraryA";
    vReplaceData[0].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &CinternalLoadLibraryA);
    vReplaceData[0].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_loadlibA_st_lib);

    vReplaceData[1].funcname = "LoadLibraryW";
    vReplaceData[1].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &CinternalLoadLibraryW);
    vReplaceData[1].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_loadlibW_st_lib);

    vReplaceData[2].funcname = "LoadLibraryExA";
    vReplaceData[2].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &CinternalLoadLibraryExA);
    vReplaceData[2].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_loadlibExA_st_lib);

    vReplaceData[3].funcname = "LoadLibraryExW";
    vReplaceData[3].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &CinternalLoadLibraryExW);
    vReplaceData[3].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_loadlibExW_st_lib);

    CInternalReplaceFunctionsAllModules(4, vReplaceData);

    vReplaceData[0].funcname = "socket";
    vReplaceData[0].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookCrtSocketStatic);
    vReplaceData[0].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_socket_original);

    vReplaceData[1].funcname = "closesocket";
    vReplaceData[1].newFuncAddress = CPPUTILS_STATIC_CAST(const void*, &SockLeakHookRemSocketStatic);
    vReplaceData[1].replaceIfAddressIs = CPPUTILS_STATIC_CAST(const void*, s_closesocket_original);

    CInternalReplaceFunctionsAllModules(2, vReplaceData);

    atexit(&programs_investigator_core_memory_leak_clean);

}


CPPUTILS_END_C
