//
// file:			programs_investigator_core_memory_leak02.cpp
// path:			src/core/leak_analyze/programs_investigator_core_memory_leak02.cpp
// created on:		2023 Dec 11
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <progs_invest/analyze_leaking02.h>
#include <progs_invest/alloc_free_hook.h>
#include <cinternal/logger.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


static struct SPrInvLeakAnalyzerData* s_pTable = CPPUTILS_NULL;
static TypeAllocFreeHookMalloc s_malloc_original;
static TypeAllocFreeHookCalloc s_calloc_original;
static TypeAllocFreeHookRealloc s_realloc_original;
static TypeAllocFreeHookFree s_free_original;


static void* ProgInvestLeakAnalyzeMallocStatic(size_t a_size)
{
    void* const pRet = (*s_malloc_original)(a_size);
    if (pRet) {
        ProgsInvestLeakAnalyzeAddResource(s_pTable, CPPUTILS_STATIC_CAST(void*,pRet), 1);
    }
    return pRet;
}


static void* ProgInvestLeakAnalyzeCallocStatic(size_t a_count, size_t a_elemSize)
{
    void* const pRet = (*s_calloc_original)(a_count,a_elemSize);
    if (pRet) {
        ProgsInvestLeakAnalyzeAddResource(s_pTable, pRet, 1);
    }
    return pRet;
}


static void* ProgInvestLeakAnalyzeReallocStatic(void* a_ptr, size_t a_size)
{
    void* const pRet = (*s_realloc_original)(a_ptr, a_size);
    if (a_ptr != pRet) {
        if (a_ptr) {
            ProgsInvestLeakAnalyzeRemoveResource(s_pTable, a_ptr);
        }

        if (pRet) {
            ProgsInvestLeakAnalyzeAddResource(s_pTable, pRet, 1);
        }
    }

    return pRet;
}


static void  ProgInvestLeakAnalyzeFreeStatic(void* a_ptr)
{
    if (a_ptr) {
        ProgsInvestLeakAnalyzeRemoveResource(s_pTable, CPPUTILS_STATIC_CAST(void*, a_ptr));
    }
    (*s_free_original)(a_ptr);
}


static void programs_investigator_core_memory_leak_clean(void){

    AllocFreeHookSetMallocFnc(s_malloc_original);
    AllocFreeHookSetCallocFnc(s_calloc_original);
    AllocFreeHookSetReallocFnc(s_realloc_original);
    AllocFreeHookSetFreeFnc(s_free_original);
    ProgsInvestLeakAnalyzeCleanup(s_pTable);
    s_pTable = CPPUTILS_NULL;

}


CPPUTILS_C_CODE_INITIALIZER(programs_investigator_core_memory_leak_init){

    s_pTable = ProgsInvestLeakAnalyzeInitialize(10,CPPUTILS_NULL,&AllocFreeHookCLibMalloc, &AllocFreeHookCLibFree);
    CInternalLogDebug("s_pTable = %p",s_pTable);
    if(!s_pTable){
        CInternalLogError("Table is not created");
        exit(1);
    }

    s_pTable->countForThisStackMax = 200;

    s_malloc_original = AllocFreeHookGetMallocFnc();
    s_calloc_original = AllocFreeHookGetCallocFnc();
    s_realloc_original = AllocFreeHookGetReallocFnc();
    s_free_original =  AllocFreeHookGetFreeFnc();

    AllocFreeHookSetMallocFnc(&ProgInvestLeakAnalyzeMallocStatic);
    AllocFreeHookSetCallocFnc(&ProgInvestLeakAnalyzeCallocStatic);
    AllocFreeHookSetReallocFnc(&ProgInvestLeakAnalyzeReallocStatic);
    AllocFreeHookSetFreeFnc(&ProgInvestLeakAnalyzeFreeStatic);

    atexit(&programs_investigator_core_memory_leak_clean);

}


CPPUTILS_END_C
