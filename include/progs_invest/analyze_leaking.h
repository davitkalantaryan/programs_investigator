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
#include <cinternal/lw_mutex_recursive.h>
#include <cinternal/hash/dllhash.h>
#include <cinternal/thread_local_sys.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>


CPPUTILS_BEGIN_C

struct StackInvestBacktrace;
typedef void (*TypeFinalAction)(void* a_pUserData,const struct StackInvestBacktrace* a_pStack);

struct SPrInvAnalyzeLeakingData {
    cinternal_lw_recursive_mutex_t  mutexForHashes;
    CinternalDLLHash_t              hashByAddress;
    CinternalDLLHash_t              hashByStack;
    CinternalTlsData                ignoreForThisThreadTlsKey;
    size_t                          countForThisStackMax;
    size_t                          unMaxValue;
    time_t                          initTimeSec;
    time_t                          deltaTimeToStartAnalyze;
    TypeFinalAction                 fncWhenLeak;
    void*                           pUserData;
    bool                            bInitializationTimeNotPassed;
    char                            reserved01[sizeof(void*) - sizeof(bool)];
};


PRINV_LEAKA_EXPORT int  ProgsInvestAnalyzeLeakingInitialize(struct SPrInvAnalyzeLeakingData* a_pTables,
                                                            TypeFinalAction CPPUTILS_ARG_NN a_fncWhenLeak, void* a_pUserData,
                                                            const char* a_cpcStartTimeEnv, const char* a_cpcMaxForStackEnv);
PRINV_LEAKA_EXPORT void ProgsInvestAnalyzeLeakingClean(struct SPrInvAnalyzeLeakingData* a_pTables);
PRINV_LEAKA_EXPORT int  ProgsInvestAnalyzeLeakingAddAllocedItem(int a_goBackInTheStackCalc, void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables);
PRINV_LEAKA_EXPORT void ProgsInvestAnalyzeLeakingRemoveAllocedItem(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables);
PRINV_LEAKA_EXPORT void ProgsInvestFinalActionPrintStackAndExit(void* a_pUserData,const struct StackInvestBacktrace* a_pStack);


CPPUTILS_END_C


#endif  // #ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING_H
