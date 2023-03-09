//
// file:			entry_alloc_free_hook_test02_lib.cpp
// path:			src/tests/entry_alloc_free_hook_test02_lib.cpp
// created on:		2023 Mar 09
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <cinternal/internal_header.h>
#include <stdlib.h>
#include <stdio.h>

CPPUTILS_BEGIN_C

CPPUTILS_DLL_PUBLIC void dummy_alloc_free_hook_test02(void)
{
	void* pMem;
	pMem = malloc(200);
	printf("%s malloc => %p\n",__FUNCTION__,pMem);
	free(pMem);
}

CPPUTILS_END_C
