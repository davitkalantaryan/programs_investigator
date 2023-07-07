
#include <progs_invest/stdout_invest.h>
#include <iostream>
#include <stdio.h>


static int PrInvStdoutInvTestClbk(enum PrInvStdoutHndl a_hndl,const void* a_buf,size_t a_size,size_t a_count);

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


static int PrInvStdoutInvTestClbk(enum PrInvStdoutHndl a_hndl,const void* a_buf,size_t a_size,size_t a_count)
{
    FILE*const pFile = (a_hndl==PrInvStdoutHndlErr)?stdout:stderr;
    return fwrite(a_buf,a_size,a_count,pFile);
}
