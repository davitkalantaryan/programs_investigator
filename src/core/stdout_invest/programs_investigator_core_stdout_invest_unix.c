//
// file:			alloc_free_hook_core_alloc_free_hook_unix.c
// path:			src/core/alloc_free_hook_core_alloc_free_hook_unix.c
// created on:		2023 Mar 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/internal_header.h>


#ifndef _WIN32

#include <progs_invest/stdout_invest.h>
#include <progs_invest/alloc_free_hook.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>

#define PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE


CPPUTILS_BEGIN_C

typedef int (*Type_puts)(const char*);
typedef size_t (*Type_fwrite)(const void *ptr, size_t size, size_t nmemb,FILE *stream);

static int PutsInitial(const char* a_str);
static int PutsFinal(const char* a_str);
static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl,const void* a_pBuffer,size_t a_size, size_t a_count);

#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
static void ThreadLocalKeyDestructor(void* a_ptr);
static pthread_key_t*    s_pThreadLocalKey = CPPUTILS_NULL;
#endif
static Type_puts                s_puts_c_lib        = CPPUTILS_NULL;
static Type_fwrite              s_fwrite_c_lib      = CPPUTILS_NULL;
static Type_puts                s_puts              = &PutsInitial;
static Type_fwrite              s_fwrite            = &FwriteInitial;
static TypePrInvStdoutInvClbk   s_userClbk          = &PrInvStdoutInvClbkOriginal;


int puts(const char* a_cpcString)
{
    return (*s_puts)(a_cpcString);
}


size_t fwrite(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    return (*s_fwrite)(a_ptr, a_size, a_nmemb,a_stream);
}


static void PrgInvestStackInvestCleanup(void){
    s_puts = s_puts_c_lib;
    s_fwrite = s_fwrite_c_lib;
#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE

    if(s_pThreadLocalKey){
        pthread_key_t* const pThreadLocalKey = s_pThreadLocalKey;
        s_pThreadLocalKey = CPPUTILS_NULL;
        pthread_key_delete(*pThreadLocalKey);
        AllocFreeHookCLibFree(pThreadLocalKey);
    }

#endif
}


static inline void InitializeCLibPointersInline(void){
    static int snPtrsInited = 0;
    static int snPtrsInitStarted = 0;

    if(snPtrsInitStarted){return;}
    snPtrsInitStarted = 1;
    if(snPtrsInited){return;}

    //write(STDOUT_FILENO,"",1);
    //write(STDERR_FILENO,"",1);
    // Upper code works and it is better, but we will use write function to implement user space FS
    dprintf(STDOUT_FILENO," ");
    dprintf(STDERR_FILENO," ");

    s_puts = s_puts_c_lib = CPPUTILS_REINTERPRET_CAST(Type_puts,dlsym(RTLD_NEXT, "puts"));
    s_fwrite = s_fwrite_c_lib = CPPUTILS_REINTERPRET_CAST(Type_fwrite,dlsym(RTLD_NEXT, "fwrite"));

    snPtrsInited = 1;
}


static inline int InitializeStdoutInvestInline(bool a_preventRecursion){
    static int snInited = 0;
    static int snStartedInitLibrary = 0;

    if(snStartedInitLibrary){return 0;}
    snStartedInitLibrary = 1;
    if(snInited){return 0;}


#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE

    if(a_preventRecursion){
        pthread_key_t* const pThreadLocalKey = (pthread_key_t*)AllocFreeHookCLibMalloc(sizeof(pthread_key_t));
        if(!pThreadLocalKey){return 1;}
        if(pthread_key_create(pThreadLocalKey,&ThreadLocalKeyDestructor)){
            AllocFreeHookCLibFree(pThreadLocalKey);
            return 1;
        }
        s_pThreadLocalKey = pThreadLocalKey;
    }

#else

    CPPUTILS_STATIC_CAST(void,a_preventRecursion);

#endif

    InitializeCLibPointersInline();

    s_puts = &PutsFinal;
    s_fwrite = &FwriteFinal;
    snInited = 1;
    atexit(&PrgInvestStackInvestCleanup);

    return 0;
}


PRINV_STDOUTINV_EXPORT int ProgInvestStdoutInvestInitialize(bool a_preventRecursion)
{
    const int nRet = InitializeStdoutInvestInline(a_preventRecursion);
    return nRet;
}


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkCurrent(void)
{
    InitializeCLibPointersInline();
    return s_userClbk;
}


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkOriginal(void)
{
    InitializeCLibPointersInline();
    return &PrInvStdoutInvClbkOriginal;
}


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestSetStdoutInvestClbkAndGetOld(TypePrInvStdoutInvClbk a_clbk)
{
    TypePrInvStdoutInvClbk oldClbk;
    InitializeStdoutInvestInline(true);
    oldClbk = s_userClbk;
    s_userClbk = a_clbk;
    return oldClbk;
}


static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl,const void* a_pBuffer,size_t a_size, size_t a_count)
{
    switch(a_hndl){
    case PrInvStdoutHndlOut:
        return CPPUTILS_STATIC_CAST(int,(*s_fwrite_c_lib)(a_pBuffer,a_size,a_count,stdout));
    case PrInvStdoutHndlErr:
        return CPPUTILS_STATIC_CAST(int,(*s_fwrite_c_lib)(a_pBuffer,a_size,a_count,stderr));
    default:
        break;
    }  //  switch(a_hndl){
    return 0;
}


static int PutsFinal(const char* a_str)
{
    int nRet;

#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
    if(s_pThreadLocalKey){
        if(pthread_getspecific(*s_pThreadLocalKey)){
            return (*s_puts_c_lib)(a_str);
        }
        pthread_setspecific(*s_pThreadLocalKey, (void*)1);
    }
#endif
    nRet = (*s_userClbk)(PrInvStdoutHndlOut,a_str,1,strlen(a_str));    
#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
    if(s_pThreadLocalKey){
        pthread_setspecific(*s_pThreadLocalKey,CPPUTILS_NULL);
    }
#endif
    return nRet;
}



static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    int nRet;
    enum PrInvStdoutHndl outHndl;

#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
    if(s_pThreadLocalKey){
        if(pthread_getspecific(*s_pThreadLocalKey)){
            return (*s_fwrite_c_lib)(a_ptr,a_size,a_nmemb,a_stream);
        }
        pthread_setspecific(*s_pThreadLocalKey, (void*)1);
    }
#endif
    outHndl = (a_stream==stdout)?PrInvStdoutHndlOut:PrInvStdoutHndlErr;
    nRet = (*s_userClbk)(outHndl,a_ptr,a_size,a_nmemb);    
#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
    if(s_pThreadLocalKey){
        pthread_setspecific(*s_pThreadLocalKey,CPPUTILS_NULL);
    }
#endif
    return nRet;
}



#ifdef PROGS_INVEST_STDOUT_INVEST_PREVENT_RECURSION_HERE
static void ThreadLocalKeyDestructor(void* a_ptr){
    CPPUTILS_STATIC_CAST(void,a_ptr);
}
#endif


static int PutsInitial(const char* a_str)
{
    InitializeCLibPointersInline();
    return (*s_puts_c_lib)(a_str);
}


static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    InitializeCLibPointersInline();
    return (*s_fwrite_c_lib)(a_ptr, a_size, a_nmemb,a_stream);
}



CPPUTILS_END_C


#endif  //  #ifndef _WIN32
