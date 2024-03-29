//
// file:			alloc_free_hook_core_alloc_free_hook_unix.c
// path:			src/core/alloc_free_hook_core_alloc_free_hook_unix.c
// created on:		2023 Mar 14
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/internal_header.h>

#ifndef _WIN32

#include <progs_invest/alloc_free_hook.h>


CPPUTILS_BEGIN_C

extern CPPUTILS_DLL_PRIVATE TypeAllocFreeHookMalloc  g_malloc;
extern CPPUTILS_DLL_PRIVATE TypeAllocFreeHookCalloc  g_calloc;
extern CPPUTILS_DLL_PRIVATE TypeAllocFreeHookRealloc g_realloc;
extern CPPUTILS_DLL_PRIVATE TypeAllocFreeHookFree    g_free;


CPPUTILS_DLL_PUBLIC void* malloc(size_t a_size)
{
    return (*g_malloc)(a_size);
}


CPPUTILS_DLL_PUBLIC void* calloc(size_t a_nmemb, size_t a_size)
{
    return (*g_calloc)(a_nmemb,a_size);
}


CPPUTILS_DLL_PUBLIC void* realloc(void* a_ptr, size_t a_size)
{
    return (*g_realloc)(a_ptr,a_size);
}


CPPUTILS_DLL_PUBLIC void free(void* a_ptr)
{
    (*g_free)(a_ptr);
}



CPPUTILS_END_C


#endif  //  #ifndef _WIN32
