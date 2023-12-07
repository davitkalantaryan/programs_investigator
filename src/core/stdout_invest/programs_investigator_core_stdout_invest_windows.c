//
// repo:            programs_investigator
// file:            programs_investigator_core_stdout_invest_windows.c
// path:            src/core/stdout_invest/programs_investigator_core_stdout_invest_windows.c
// created on:      2023 Jul 12
// created by:      Davit Kalantaryan (davit.kalantaryan@desy.de)
// purpose:         Implements function to make possible to replace stdout related functions
//


#include <cinternal/internal_header.h>


#ifdef _WIN32

#include <progs_invest/stdout_invest.h>
#include <cinternal/replace_function.h>
#include <cinternal/thread_local_sys.h>
#include <stdio.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl, const void* a_pBuffer, size_t a_size, size_t a_count);
static TypePrInvStdoutInvClbk   s_userClbk = &PrInvStdoutInvClbkOriginal;


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkCurrent(void)
{
    return s_userClbk;
}


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkOriginal(void)
{
    return &PrInvStdoutInvClbkOriginal;
}


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestSetStdoutInvestClbkAndGetOld(TypePrInvStdoutInvClbk a_clbk)
{
    const TypePrInvStdoutInvClbk oldClbk = s_userClbk;
    s_userClbk = a_clbk;
    return oldClbk;
}


typedef int (*TypeStdio01)(
    unsigned __int64 _Options,
    FILE* _Stream,
    const char* _Format,
    _locale_t        _Locale,
    va_list          _ArgList
    );

typedef size_t(*TypeStdio02)(
    const void* _Buffer,
    _In_                                
    size_t      _ElementSize,
    size_t      _ElementCount,
    FILE* _Stream
    );


static CinternalTlsData    s_threadLocalKey;
static TypeStdio01 s_originalStdio01 = &__stdio_common_vfprintf;
static TypeStdio02 s_original_fwrite = &fwrite;

static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl, const void* a_pBuffer, size_t a_size, size_t a_count)
{
    switch (a_hndl) {
    case PrInvStdoutHndlOut:
        return CPPUTILS_STATIC_CAST(int,(*s_original_fwrite)(a_pBuffer, a_size, a_count, stdout));
    case PrInvStdoutHndlErr:
        return CPPUTILS_STATIC_CAST(int,(*s_original_fwrite)(a_pBuffer, a_size, a_count, stderr));
    default:
        break;
    }  //  switch(a_hndl){
    return 0;
}


static int Stdio01Static(
    unsigned __int64 a_options,
    FILE* a_stream,
    const char* a_format,
    _locale_t        a_locale,
    va_list          a_argList )
{
    char vcBuffer[4096];
    int nRet;
    enum PrInvStdoutHndl outHndl;

    if (a_locale) {
        return (*s_originalStdio01)(a_options, a_stream, a_format, a_locale, a_argList);
    }

    if (CinternalTlsGetSpecific(s_threadLocalKey)) {
        return (*s_originalStdio01)(a_options, a_stream, a_format, a_locale, a_argList);
    }

    CPPUTILS_STATIC_CAST(void, a_options);

    nRet = vsnprintf_s(vcBuffer, 4095, 4095, a_format, a_argList);
    vcBuffer[nRet++] = 0;

    CinternalTlsSetSpecific(s_threadLocalKey, (void*)1);
    outHndl = (a_stream == stdout) ? PrInvStdoutHndlOut : PrInvStdoutHndlErr;
    nRet = (*s_userClbk)(outHndl, vcBuffer,1, nRet);
    CinternalTlsSetSpecific(s_threadLocalKey, CPPUTILS_NULL);

    return nRet;
}


static size_t FWriteStatic(
    const void* a_ptr,
    size_t      a_size,
    size_t      a_nmemb,
    FILE* a_stream)
{
    int nRet;
    enum PrInvStdoutHndl outHndl;

    if (CinternalTlsGetSpecific(s_threadLocalKey)) {
        return (*s_original_fwrite)(a_ptr, a_size, a_nmemb, a_stream);
    }
    CinternalTlsSetSpecific(s_threadLocalKey, (void*)1);
    outHndl = (a_stream == stdout) ? PrInvStdoutHndlOut : PrInvStdoutHndlErr;
    nRet = (*s_userClbk)(outHndl, a_ptr, a_size, a_nmemb);
    CinternalTlsSetSpecific(s_threadLocalKey, CPPUTILS_NULL);
    return nRet;
}


static int programs_investigator_core_stdout_invest_windows_clean(void) {
    struct SCInternalReplaceFunctionData aReplaceData[2];

    aReplaceData[0].funcname = "__stdio_common_vfprintf";
    aReplaceData[0].newFuncAddress = (void*)s_originalStdio01;
    aReplaceData[0].replaceIfAddressIs = (void*)&Stdio01Static;

    aReplaceData[1].funcname = "fwrite";
    aReplaceData[1].newFuncAddress = (void*)s_original_fwrite;
    aReplaceData[1].replaceIfAddressIs = (void*)&FWriteStatic;

    CInternalReplaceFunctionsAllModules(2, aReplaceData);

    return 0;
}


static void ThreadLocalKeyDestructor(void* a_ptr) {
    CPPUTILS_STATIC_CAST(void, a_ptr);
}


CPPUTILS_C_CODE_INITIALIZER(programs_investigator_core_stdout_invest_windows_init) {

    struct SCInternalReplaceFunctionData aReplaceData[2];
    s_userClbk = &PrInvStdoutInvClbkOriginal;
    s_originalStdio01 = &__stdio_common_vfprintf;
    s_original_fwrite = &fwrite;

    if (CinternalTlsAlloc(&s_threadLocalKey, &ThreadLocalKeyDestructor)) {
        exit(1);
    }

    aReplaceData[0].funcname = "__stdio_common_vfprintf";
    aReplaceData[0].newFuncAddress = (void*)&Stdio01Static;
    aReplaceData[0].replaceIfAddressIs = (void*)&__stdio_common_vfprintf;

    aReplaceData[1].funcname = "fwrite";
    aReplaceData[1].newFuncAddress = (void*)&FWriteStatic;
    aReplaceData[1].replaceIfAddressIs = (void*)&fwrite;

    CInternalReplaceFunctionsAllModules(2, aReplaceData);

    _onexit(&programs_investigator_core_stdout_invest_windows_clean);

}



CPPUTILS_END_C


#endif  //  #ifdef _WIN32
