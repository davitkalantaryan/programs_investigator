//
// file:            cinternal_core_replace_function_unix.c
// path:			src/core/windows/cinternal_core_replace_function_unix.c
// created on:		2023 Mar 18
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <cinternal/internal_header.h>

#ifndef _WIN32

#ifndef _GNU_SOURCE
#define  _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define  __USE_GNU
#endif
#include <dlfcn.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C



CPPUTILS_DLL_PUBLIC void* dummy_dlfnc_initialize_for_dynalic_load(void)
{
    return dlsym(RTLD_NEXT, "malloc");
}


CPPUTILS_END_C


#endif  //  #ifndef _WIN32
