//
// file:			entry_alloc_free_hook_test02_lib.cpp
// path:			src/tests/entry_alloc_free_hook_test02_lib.cpp
// created on:		2023 Mar 09
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <progs_invest/alloc_free_hook.h>
#include <cinternal/thread_local_sys.h>
#include <stdlib.h>
#include <stdio.h>

CPPUTILS_BEGIN_C

static CinternalTlsData s_threadSpecificKey = (CinternalTlsData)0;

static TypeAllocFreeHookMalloc s_initial_malloc = CPPUTILS_NULL;
static TypeAllocFreeHookFree s_initial_free = CPPUTILS_NULL;

//static CPPUTILS_THREAD_LOCAL int s_nIgnoreThisStack = 0;
//static int s_nIgnoreThisStack = 0;

static void* MyMalloc(size_t a_size) CPPUTILS_NOEXCEPT
{
	void* pRet;
    void* pIgnoreThisStack = CinternalTlsGetSpecific(s_threadSpecificKey);
	if (pIgnoreThisStack) {
		pRet = AllocFreeHookCLibMalloc(a_size);
	}
	else {
		//s_nIgnoreThisStack = 1;
        CinternalTlsSetSpecific(s_threadSpecificKey,(void*)1);
		pRet = AllocFreeHookCLibMalloc(a_size);
		printf(" +++++ MyMalloc pRet=%p\n", pRet);
		//s_nIgnoreThisStack = 0;
        CinternalTlsSetSpecific(s_threadSpecificKey,(void*)0);
	}
	return pRet;
}


static void MyFree(void* a_ptr) CPPUTILS_NOEXCEPT
{
    void* pIgnoreThisStack = CinternalTlsGetSpecific(s_threadSpecificKey);
	if (pIgnoreThisStack) {
		AllocFreeHookCLibFree(a_ptr);
	}
	else {
		//s_nIgnoreThisStack = 1;
        CinternalTlsSetSpecific(s_threadSpecificKey,(void*)1);
		printf(" ----- MyFree pRet=%p\n", a_ptr);
		AllocFreeHookCLibFree(a_ptr);
		//s_nIgnoreThisStack = 0;
        CinternalTlsSetSpecific(s_threadSpecificKey,(void*)0);
	}
}


static void entry_test_lib_to_change_alloc_free_clean(void)
{
	printf("Cleaning entry_test_lib_to_change_alloc_free\n");
    AllocFreeHookSetMallocFnc(s_initial_malloc);
	AllocFreeHookSetFreeFnc(s_initial_free);
	CinternalTlsDelete(s_threadSpecificKey);
}


static void ThreadSpecificKeyDestructor(void* a_pData)
{
    (void)a_pData;
}


CPPUTILS_CODE_INITIALIZER(entry_test_lib_to_change_alloc_free_initialize) {

    printf("Initializing entry_test_lib_to_change_alloc_free\n");
    
    if(CinternalTlsAlloc(&s_threadSpecificKey,&ThreadSpecificKeyDestructor)){
        exit(1);
    }
    
    s_initial_malloc = AllocFreeHookGetMallocFnc();
    s_initial_free = AllocFreeHookGetFreeFnc();
    AllocFreeHookSetMallocFnc(&MyMalloc);
	AllocFreeHookSetFreeFnc(&MyFree);
    atexit(&entry_test_lib_to_change_alloc_free_clean);
}

CPPUTILS_END_C
