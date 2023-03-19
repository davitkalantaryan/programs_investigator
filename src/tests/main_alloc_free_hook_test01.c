//
// file:			main_alloc_free_hook_test01.cpp
// path:			src/tests/main_alloc_free_hook_test01.cpp
// created on:		2023 Mar 09
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <allocfreehook/alloc_free_hook.h>
#include <stdio.h>
#include <stdlib.h>


static CPPUTILS_THREAD_LOCAL int s_nIgnoreThisStack = 0;

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
	void* pMem;

	AllocFreeHookSetMallocFnc(&MyMalloc);
	AllocFreeHookSetFreeFnc(&MyFree);

	pMem = malloc(100);
	printf("pMem = %p\n", pMem);
	free(pMem);

	return 0;
}
