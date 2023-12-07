//
// repo:            programs_investigator
// file:            main_stdout_invest_test.cpp
// path:            src/tests/main_stdout_invest_test.cpp
// created on:      2023 Mar 15
// created by:      Davit Kalantaryan (davit.kalantaryan@desy.de)
// purpose:         Implements function to make possible to replace stdout related functions
//

#include <progs_invest/stdout_invest.h>
#include <cinternal/disable_compiler_warnings.h>
#include <iostream>
#include <stdio.h>
#include <cinternal/undisable_compiler_warnings.h>


static int PrInvStdoutInvTestClbk(enum PrInvStdoutHndl a_hndl,const void* a_buf,size_t a_size,size_t a_count) CPPUTILS_NOEXCEPT;

int main()
{
    ProgInvestSetStdoutInvestClbkAndGetOld(&PrInvStdoutInvTestClbk);

    printf("Hellow world 01!\n");
    fflush(stdout);
    fprintf(stdout,"Hellow world 02!\n");
    fprintf(stderr,"Hellow world 03!\n");
    ::std::cout<< "Hellow world 04!\n";
    ::std::cerr<< "Hellow world 05!\n";

    return 0;
}


static int PrInvStdoutInvTestClbk(enum PrInvStdoutHndl a_hndl,const void* a_buf,size_t a_size,size_t a_count) CPPUTILS_NOEXCEPT
{
    FILE*const pFile = (a_hndl==PrInvStdoutHndlErr)?stdout:stderr;
    return (int)fwrite(a_buf,a_size,a_count,pFile);
}
