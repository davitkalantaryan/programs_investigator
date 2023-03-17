//
// file:			entry_alloc_free_hook_test02_lib.cpp
// path:			src/tests/entry_alloc_free_hook_test02_lib.cpp
// created on:		2023 Mar 09
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <allocfreehook/alloc_free_hook.h>
#include <stdlib.h>
#include <stdio.h>

CPPUTILS_BEGIN_C

CPPUTILS_DLL_PUBLIC void entry_test_lib_to_change_alloc_free(void)
{
	void* pMem;
	pMem = malloc(200);
	printf("%s malloc => %p\n",__FUNCTION__,pMem);
	free(pMem);
}

static int CPPUTILS_THREAD_LOCAL  s_nIgnoreThisStack = 0;

static void* MyMalloc(size_t a_size) CPPUTILS_NOEXCEPT
{
	void* pRet;
	if (s_nIgnoreThisStack) {
		pRet = AllocFreeHookCLibMalloc(a_size);
	}
	else {
		s_nIgnoreThisStack = 1;
		pRet = AllocFreeHookCLibMalloc(a_size);
		printf(" +++++ MyMalloc pRet=%p\n", pRet);
		s_nIgnoreThisStack = 0;
	}
	return pRet;
}


static void MyFree(void* a_ptr) CPPUTILS_NOEXCEPT
{
	if (s_nIgnoreThisStack) {
		AllocFreeHookCLibFree(a_ptr);
	}
	else {
		s_nIgnoreThisStack = 1;
		printf(" ----- MyFree pRet=%p\n", a_ptr);
		AllocFreeHookCLibFree(a_ptr);
		s_nIgnoreThisStack = 0;
	}
}


static int entry_test_lib_to_change_alloc_free_clean(void)
{
	printf("Cleaning entry_test_lib_to_change_alloc_free\n");
	return 0;
}


CPPUTILS_CODE_INITIALIZER(entry_test_lib_to_change_alloc_free_initialize) {
	printf("Initializing entry_test_lib_to_change_alloc_free\n");
	AllocFreeHookSetMallocFnc(&MyMalloc);
	AllocFreeHookSetFreeFnc(&MyFree);
	_onexit(&entry_test_lib_to_change_alloc_free_clean);
}

CPPUTILS_END_C
