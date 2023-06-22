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


static void* s_pLibDlhandle = CPPUTILS_NULL;


static void entry_cinternal_core_init_libdl_clean(void)
{
	if(s_pLibDlhandle){
		dlclose(s_pLibDlhandle);
		s_pLibDlhandle = CPPUTILS_NULL;
	}
}


CPPUTILS_CODE_INITIALIZER(entry_cinternal_core_init_libdl_initialize){

    s_pLibDlhandle = dlopen("libdl.so",RTLD_NOW);
	atexit(&entry_cinternal_core_init_libdl_clean);
}


#endif  //  #ifndef _WIN32


CPPUTILS_END_C
