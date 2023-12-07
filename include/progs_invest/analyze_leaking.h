//
// repo:            programs_investigator
// file:			analyze_leaking.h
// path:			include/progs_invest/analyze_leaking.h
// created on:		2023 Jul 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING_H
#define PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING_H

#include <progs_invest/export_symbols.h>
#include <progs_invest/alloc_free_hook.h>
#include <cinternal/lw_mutex_recursive.h>
#include <cinternal/hash/dllhash.h>
#include <cinternal/thread_local_sys.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>


CPPUTILS_BEGIN_C

struct StackInvestBacktrace;
struct SPrInvAnalyzeLeakingData;
struct SPrInvLeakAnlzStackItem2{
    struct StackInvestBacktrace*    pStack;
    size_t                          countForThisStack;
};
typedef void (*TypeFinalAction)(struct SPrInvAnalyzeLeakingData* a_pTables, struct SPrInvLeakAnlzStackItem2* a_pStackItem);


PRINV_LEAKA_EXPORT struct SPrInvAnalyzeLeakingData*  ProgsInvestAnalyzeLeakingInitialize(
    void* a_pUserData,
    TypeAllocFreeHookMalloc CPPUTILS_ARG_NN a_maloc,TypeAllocFreeHookCalloc CPPUTILS_ARG_NN a_calloc,
    TypeAllocFreeHookRealloc CPPUTILS_ARG_NN a_realloc, TypeAllocFreeHookFree CPPUTILS_ARG_NN a_free,
    TypeFinalAction a_fncWhenLeak,const char* a_cpcStartTimeEnv, const char* a_cpcMaxForStackEnv);
PRINV_LEAKA_EXPORT void  ProgsInvestAnalyzeLeakingClean(struct SPrInvAnalyzeLeakingData* a_pTables);
PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingMalloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, size_t a_size);
PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingCalloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, size_t a_count, size_t a_elemSize);
PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingRealloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, void* a_ptr, size_t a_newSize);
PRINV_LEAKA_EXPORT void  ProgsInvestAnalyzeLeakingFree(struct SPrInvAnalyzeLeakingData* a_pTables, void* a_ptr);

PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingGetUserData(struct SPrInvAnalyzeLeakingData* a_pTables);
PRINV_LEAKA_EXPORT void  ProgsInvestFinalActionTryFixLeak(struct SPrInvAnalyzeLeakingData* a_pTables,struct SPrInvLeakAnlzStackItem2* a_pStackItem);
PRINV_LEAKA_EXPORT void  ProgsInvestFinalActionPrintStackAndExit(struct SPrInvAnalyzeLeakingData* a_pTables,struct SPrInvLeakAnlzStackItem2* a_pStackItem);


CPPUTILS_END_C


#endif  // #ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING_H
