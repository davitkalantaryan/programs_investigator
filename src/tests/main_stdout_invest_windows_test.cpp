//
// file:            cinternal_core_replace_function_windows.c
// path:			src/core/windows/cinternal_core_replace_function_windows.c
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/disable_compiler_warnings.h>
#include <iostream>
#include <stdio.h>


int main()
{
    printf("Hellow world 01!\n");
    fflush(stdout);
    fprintf(stdout,"Hellow world 02!\n");
    fprintf(stderr,"Hellow world 03!\n");
    fwrite("Hello world 03!\n",1,17,stderr);
    ::std::cout<<int(2)<< " Hellow world 04!\n";
    ::std::cout.write("hi\n", 4);
    ::std::cerr << "cerr\n";
    ::std::clog << "clog\n";
    //::std::cerr<< "Hellow world 05!\n";

    return 0;
}

#ifdef _WIN32


CPPUTILS_BEGIN_C

#include <cinternal/replace_function.h>
#include <stdarg.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>


static void ReplaceStdoutFunctions(void);


typedef int (*TypeStdio01)(
    _In_                                    unsigned __int64 _Options,
    _Inout_                                 FILE* _Stream,
    _In_z_ _Printf_format_string_params_(2) char const* _Format,
    _In_opt_                                _locale_t        _Locale,
    va_list          _ArgList
    );

typedef size_t (*TypeStdio02)(
    void const* _Buffer,
    _In_                                           size_t      _ElementSize,
    _In_                                           size_t      _ElementCount,
    _Inout_                                        FILE* _Stream
    );



static int Stdio01Static(
    _In_                                    unsigned __int64 _Options,
    _Inout_                                 FILE* _Stream,
    _In_z_ _Printf_format_string_params_(2) char const* _Format,
    _In_opt_                                _locale_t        _Locale,
    va_list          _ArgList
    );

static size_t Stdio02Static(
    void const* _Buffer,
    _In_                                           size_t      _ElementSize,
    _In_                                           size_t      _ElementCount,
    _Inout_                                        FILE* _Stream
);


static TypeStdio01 s_originalStdio01 = &__stdio_common_vfprintf;
static TypeStdio02 s_originalStdio02 = &fwrite;

#define NUMBER_OF_FUNCS_TO_REPLACE  2

static void ReplaceStdoutFunctions(void)
{
    struct SCInternalReplaceFunctionData aReplaceData[NUMBER_OF_FUNCS_TO_REPLACE];

    aReplaceData[0].funcname = "__stdio_common_vfprintf";
    aReplaceData[0].newFuncAddress = (void*)&Stdio01Static;
    aReplaceData[0].replaceIfAddressIs = &__stdio_common_vfprintf;

    aReplaceData[1].funcname = "fwrite";
    aReplaceData[1].newFuncAddress = (void*)&Stdio02Static;
    aReplaceData[1].replaceIfAddressIs = &fwrite;

    CInternalReplaceFunctionsAllModules(NUMBER_OF_FUNCS_TO_REPLACE, aReplaceData);
}


static int Stdio01Static(
    _In_                                    unsigned __int64 _Options,
    _Inout_                                 FILE* _Stream,
    _In_z_ _Printf_format_string_params_(2) char const* _Format,
    _In_opt_                                _locale_t        _Locale,
    va_list          _ArgList
)
{
    return (*s_originalStdio01)(_Options,_Stream, _Format, _Locale,_ArgList);
}


static size_t Stdio02Static(
    void const* _Buffer,
    _In_                                           size_t      _ElementSize,
    _In_                                           size_t      _ElementCount,
    _Inout_                                        FILE* _Stream
)
{
    return (*s_originalStdio02)(_Buffer, _ElementSize, _ElementCount, _Stream);
}



CPPUTILS_C_CODE_INITIALIZER(main_stdout_invest_windows_test_init) {
    s_originalStdio01 = &__stdio_common_vfprintf;
    s_originalStdio02 = &fwrite;
    ReplaceStdoutFunctions();
}

CPPUTILS_END_C

#endif  //  #ifdef _WIN32

#include <cinternal/undisable_compiler_warnings.h>
