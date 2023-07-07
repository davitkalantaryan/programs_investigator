//
// repo:            programs_investigator
// file:			stdout_invest.h
// path:			include/progs_invest/stdout_invest.h
// created on:		2023 Jul 07
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef PROGRAMS_INVESTIGATOR_INCLUDE_PROGS_INVEST_STDOUT_INVEST_H
#define PROGRAMS_INVESTIGATOR_INCLUDE_PROGS_INVEST_STDOUT_INVEST_H

#include <progs_invest/export_symbols.h>
#include <stddef.h>


CPPUTILS_BEGIN_C


enum PrInvStdoutHndl{PrInvStdoutHndlNone,PrInvStdoutHndlOut,PrInvStdoutHndlErr};


typedef int (*TypePrInvStdoutInvClbk)(enum PrInvStdoutHndl,const void*,size_t size,size_t count);


PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkCurrent(void);
PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestGetStdoutInvestClbkOriginal(void);
PRINV_STDOUTINV_EXPORT TypePrInvStdoutInvClbk ProgInvestSetStdoutInvestClbkAndGetOld(TypePrInvStdoutInvClbk a_clbk);


CPPUTILS_END_C


#endif  // #ifndef PROGRAMS_INVESTIGATOR_INCLUDE_PROGS_INVEST_STDOUT_INVEST_H
