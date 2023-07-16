//
// repo:            programs_investigator
// file:            main_stdout_invest_test.cpp
// path:            src/tests/main_stdout_invest_test.cpp
// created on:      2023 Mar 15
// created by:      Davit Kalantaryan (davit.kalantaryan@desy.de)
// purpose:         Implements function to make possible to replace stdout related functions
//

#include <cinternal/disable_compiler_warnings.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


int main()
{
    while(1){
        char* pVal = new char[100];
        printf("%p\n",pVal);
    }

    return 0;
}
