
#include <cinternal/internal_header.h>
#include <iostream>
#include <stdio.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>


int main()
{
    printf("Hellow world 01!\n");
    fflush(stdout);
    fprintf(stdout,"Hellow world 02!\n");
    fprintf(stderr,"Hellow world 03!\n");
    ::std::cout<< "Hellow world 04!\n";
    ::std::cerr<< "Hellow world 05!\n";

    return 0;
}

CPPUTILS_BEGIN_C

static int PutsInitial(const char* a_str);
static int PutsFinal(const char* a_str);
static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream);
typedef int (*Type_puts)(const char*);
typedef size_t (*Type_fwrite)(const void *ptr, size_t size, size_t nmemb,FILE *stream);
static Type_puts s_original_puts = &PutsInitial;
static Type_puts s_puts = &PutsInitial;
static Type_fwrite s_original_fwrite = &FwriteInitial;
static Type_fwrite s_fwrite = &FwriteInitial;


int puts(const char* a_cpcString)
{
    return (*s_puts)(a_cpcString);
}


size_t fwrite(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    return (*s_fwrite)(a_ptr, a_size, a_nmemb,a_stream);
}


static inline void InitializeStdoutInvestInline(void){
    static int snInited = 0;
    if(snInited){return;}
    snInited = 1;
    s_puts = &PutsInitial;
    s_fwrite = &FwriteInitial;
    s_original_puts = CPPUTILS_REINTERPRET_CAST(Type_puts,dlsym(RTLD_NEXT, "puts"));
    s_original_fwrite = CPPUTILS_REINTERPRET_CAST(Type_fwrite,dlsym(RTLD_NEXT, "fwrite"));
    s_puts = &PutsFinal;
    s_fwrite = &FwriteFinal;
}


static int PutsInitial(const char* a_str)
{
    InitializeStdoutInvestInline();
    return PutsFinal(a_str);
}


static int PutsFinal(const char* a_str)
{
    return (*s_original_puts)(a_str);
}


static size_t FwriteInitial(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    InitializeStdoutInvestInline();
    return FwriteFinal(a_ptr, a_size, a_nmemb,a_stream);
}


static size_t FwriteFinal(const void* a_ptr, size_t a_size, size_t a_nmemb,FILE* a_stream)
{
    return (*s_original_fwrite)(a_ptr, a_size, a_nmemb,a_stream);
}


CPPUTILS_END_C


CPPUTILS_CODE_INITIALIZER(main_stdout_invest_test_init){

    InitializeStdoutInvestInline();

}
