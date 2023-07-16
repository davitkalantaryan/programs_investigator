//
// file:			crash_investigator_linux_simple_analyze.cpp
// path:			src/core/crash_investigator_linux_simple_analyze.cpp
// created on:		2023 Mar 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <progs_invest/analyze_leaking.h>
#include <progs_invest/alloc_free_hook.h>
#include <cinternal/logger.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


static struct SPrInvAnalyzeLeakingData* s_pTable = CPPUTILS_NULL;
static TypeAllocFreeHookMalloc s_malloc_original;
static TypeAllocFreeHookCalloc s_calloc_original;
static TypeAllocFreeHookRealloc s_realloc_original;
static TypeAllocFreeHookFree s_free_original;


static void* ProgInvestLeakAnalyzeMallocStatic(size_t a_size)
{
    return ProgsInvestAnalyzeLeakingMalloc(s_pTable,1,a_size);
}


static void* ProgInvestLeakAnalyzeCallocStatic(size_t a_count, size_t a_elemSize)
{
    return ProgsInvestAnalyzeLeakingCalloc(s_pTable,1,a_count,a_elemSize);
}


static void* ProgInvestLeakAnalyzeReallocStatic(void* a_ptr, size_t a_size)
{
    return ProgsInvestAnalyzeLeakingRealloc(s_pTable,1,a_ptr,a_size);
}


static void  ProgInvestLeakAnalyzeFreeStatic(void* a_ptr)
{
    ProgsInvestAnalyzeLeakingFree(s_pTable,a_ptr);
}


static void programs_investigator_core_memory_leak_clean(void){

    AllocFreeHookSetMallocFnc(s_malloc_original);
    AllocFreeHookSetCallocFnc(s_calloc_original);
    AllocFreeHookSetReallocFnc(s_realloc_original);
    AllocFreeHookSetFreeFnc(s_free_original);
    ProgsInvestAnalyzeLeakingClean(s_pTable);
    s_pTable = CPPUTILS_NULL;

}


CPPUTILS_C_CODE_INITIALIZER(programs_investigator_core_memory_leak_init){

    s_pTable = ProgsInvestAnalyzeLeakingInitialize(CPPUTILS_NULL,
                                                   &AllocFreeHookCLibMalloc,&AllocFreeHookCLibCalloc,
                                                   &AllocFreeHookCLibRealloc, &AllocFreeHookCLibFree,
                                                   &ProgsInvestFinalActionPrintStackAndExit,
                                                   "MEMORY_LEAK_ANALYZE_INIT_TIME_SEC_DEFAULT","MEMORY_LEAK_ANALYZE_MAX_ALLOC_DEFAULT");
    CInternalLogError("s_pTable = %p",s_pTable);
    if(!s_pTable){
        CInternalLogError("Table is not created");
        exit(1);
    }

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
