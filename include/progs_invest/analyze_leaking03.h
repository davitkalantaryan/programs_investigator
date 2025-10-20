//
// repo:            programs_investigator
// file:			analyze_leaking02.h
// path:			include/progs_invest/analyze_leaking02.h
// created on:		2023 Dec 10
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING03_H
#define PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING03_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PROGS_INVEST_EXPORT
#ifdef _MSC_VER
#define PROGS_INVEST_EXPORT _declspec (dllimport)
#else
#define PROGS_INVEST_EXPORT
#endif
#endif


PROGS_INVEST_EXPORT void ProgsInvestLeakAnalyzeSetMaxAllowedCount(size_t a_allowedCount);
PROGS_INVEST_EXPORT void ProgsInvestLeakAnalyzeIgnoreStackAnalyze(void);
PROGS_INVEST_EXPORT void ProgsInvestLeakAnalyzeUnignoreStackAnalyze(void);


#ifdef __cplusplus
}
#endif


#endif  // #ifndef PROGS_INVEST_INCLUDE_PROGS_INVEST_ANALYZE_LEAKING03_H
