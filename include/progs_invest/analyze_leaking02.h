//
// repo:            programs_investigator
// file:			analyze_leaking02.h
// path:			include/progs_invest/analyze_leaking02.h
// created on:		2023 Dec 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING02_H
#define PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING02_H

#include <progs_invest/export_symbols.h>
#include <cinternal/lw_mutex_recursive.h>
#include <cinternal/hash/dllhash.h>
#include <cinternal/thread_local_sys.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#define PR_INVEST_LEAK_ANALYZE_DEF_MAX_IN_STACK     100


CPPUTILS_BEGIN_C

typedef void* (*TypePrInvLeakAnalyzerMalloc)(size_t);
typedef void (*TypePrInvLeakAnalyzerFree)(void*);

struct StackInvestBacktrace;
struct SPrInvLeakAnalyzerData {
    void*                           pUserData;
    size_t                          countForThisStackMax;
    bool                            bStopAnalyzing;
    bool                            keepLeakingStacks;
    char                            reserved01[sizeof(void*) - 2 * sizeof(bool)];
};
typedef void (*TypePrInvLeakAnalyzerAnalyzeLeakingStack)(
    struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN, struct StackInvestBacktrace* CPPUTILS_ARG_NN);


PRINV_LEAKA_EXPORT struct SPrInvLeakAnalyzerData*  
ProgsInvestLeakAnalyzeInitialize(
    time_t a_deltaTimeToStartAnalyze, TypePrInvLeakAnalyzerAnalyzeLeakingStack a_stackAnalyzer,
    TypePrInvLeakAnalyzerMalloc a_malloc, TypePrInvLeakAnalyzerFree a_free) CPPUTILS_NOEXCEPT;
PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeCleanup(struct SPrInvLeakAnalyzerData* a_pTables) CPPUTILS_NOEXCEPT;
PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeAddResource(
    struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, void* a_resource,
    int a_goBackInTheStackCalc) CPPUTILS_NOEXCEPT;
PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeRemoveResource(
    struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, void* a_resource) CPPUTILS_NOEXCEPT;



CPPUTILS_END_C


#endif  // #ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING02_H
