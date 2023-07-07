//
// file:			alloc_free_hook_core_alloc_free_hook_unix.c
// path:			src/core/alloc_free_hook_core_alloc_free_hook_unix.c
// created on:		2023 Mar 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/internal_header.h>


#ifndef _WIN32

#include <progs_invest/stdout_invest.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>


CPPUTILS_BEGIN_C

typedef int (*Type_puts)(const char*);
typedef size_t (*Type_fwrite)(const void *ptr, size_t size, size_t nmemb,FILE *stream);

static int PutsInitial(const char* a_str);
static int PutsFinal(const char* a_str);
static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl,const void* a_pBuffer,size_t a_size, size_t a_count);

static pthread_key_t    s_threadLocalKey;
static Type_puts s_original_puts = &PutsInitial;
static Type_puts s_puts = &PutsInitial;
static Type_fwrite s_original_fwrite = &FwriteInitial;
static Type_fwrite s_fwrite = &FwriteInitial;
static TypePrInvStdoutInvClbk  s_userClbk = &PrInvStdoutInvClbkOriginal;


int puts(const char* a_cpcString)
{
    return (*s_puts)(a_cpcString);
}


size_t fwrite(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    return (*s_fwrite)(a_ptr, a_size, a_nmemb,a_stream);
}


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


static int PrInvStdoutInvClbkOriginal(enum PrInvStdoutHndl a_hndl,const void* a_pBuffer,size_t a_size, size_t a_count)
{
    switch(a_hndl){
    case PrInvStdoutHndlOut:
        return (*s_original_fwrite)(a_pBuffer,a_size,a_count,stdout);
    case PrInvStdoutHndlErr:
        return (*s_original_fwrite)(a_pBuffer,a_size,a_count,stderr);
    default:
        break;
    }  //  switch(a_hndl){
    return 0;
}


static int PutsFinal(const char* a_str)
{
    int nRet;

    if(pthread_getspecific(s_threadLocalKey)){
        return (*s_original_puts)(a_str);
    }
    pthread_setspecific(s_threadLocalKey,CPPUTILS_STATIC_CAST(void*,1));
    nRet = (*s_userClbk)(PrInvStdoutHndlOut,a_str,1,strlen(a_str));
    pthread_setspecific(s_threadLocalKey,CPPUTILS_NULL);
    return nRet;
}



static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    int nRet;
    enum PrInvStdoutHndl outHndl;

    if(pthread_getspecific(s_threadLocalKey)){
        return (*s_original_fwrite)(a_ptr,a_size,a_nmemb,a_stream);
    }
    pthread_setspecific(s_threadLocalKey,CPPUTILS_STATIC_CAST(void*,1));
    outHndl = (a_stream==stdout)?PrInvStdoutHndlOut:PrInvStdoutHndlErr;
    nRet = (*s_userClbk)(outHndl,a_ptr,a_size,a_nmemb);
    return nRet;
}


static void PrgInvestStackInvestCleanup(void){
    s_puts = s_original_puts;
    s_fwrite = s_original_fwrite;
    pthread_key_delete(s_threadLocalKey);
}


static void ThreadLocalKeyDestructor(void* a_ptr){
    CPPUTILS_STATIC_CAST(void,a_ptr);
}


static inline void InitializeStdoutInvestInline(void){
    static int snInited = 0;
    if(snInited){return;}
    snInited = 1;
    if(pthread_key_create(&s_threadLocalKey,&ThreadLocalKeyDestructor)){
        exit(1);
    }
    s_puts = &PutsInitial;
    s_fwrite = &FwriteInitial;
    s_userClbk = &PrInvStdoutInvClbkOriginal;
    s_original_puts = CPPUTILS_REINTERPRET_CAST(Type_puts,dlsym(RTLD_NEXT, "puts"));
    s_original_fwrite = CPPUTILS_REINTERPRET_CAST(Type_fwrite,dlsym(RTLD_NEXT, "fwrite"));
    s_puts = &PutsFinal;
    s_fwrite = &FwriteFinal;
    atexit(&PrgInvestStackInvestCleanup);
}


static int PutsInitial(const char* a_str)
{
    InitializeStdoutInvestInline();
    return PutsFinal(a_str);
}


static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    InitializeStdoutInvestInline();
    return FwriteFinal(a_ptr, a_size, a_nmemb,a_stream);
}


CPPUTILS_C_CODE_INITIALIZER(InitializeStdoutInvest){

    InitializeStdoutInvestInline();

}



CPPUTILS_END_C


#endif  //  #ifndef _WIN32
