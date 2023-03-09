//
// file:			main_alloc_free_hook_test02_exe.cpp
// path:			src/tests/main_alloc_free_hook_test02_exe.cpp
// created on:		2023 Mar 09
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <cinternal/replace_function.h>
#include <allocfreehook/alloc_free_hook.h>
#include <stdio.h>
#include <stdlib.h>

CPPUTILS_BEGIN_C

CPPUTILS_IMPORT_FROM_DLL void dummy_alloc_free_hook_test02(void);

CPPUTILS_END_C



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



int main(void)
{
	AllocFreeHookSetMallocFnc(&MyMalloc);
	AllocFreeHookSetFreeFnc(&MyFree);

	dummy_alloc_free_hook_test02();

	return 0;
}
