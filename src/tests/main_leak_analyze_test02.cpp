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


#ifdef __INTELLISENSE__


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <Psapi.h>

static void MySocketFunc(void) {}



void ReplaceFunctionsForModule(HMODULE a_hModule)
{
    MODULEINFO modInfo = { 0 };
    // Find the base address
    GetModuleInformation(GetCurrentProcess(), a_hModule, &modInfo, sizeof(MODULEINFO));

    LPBYTE                  pAddress = (LPBYTE)(modInfo.lpBaseOfDll);
    PIMAGE_DOS_HEADER       pIDH = (PIMAGE_DOS_HEADER)(pAddress);
    PIMAGE_NT_HEADERS       pINH = (PIMAGE_NT_HEADERS)(pAddress + pIDH->e_lfanew);
    PIMAGE_OPTIONAL_HEADER  pIOH = (PIMAGE_OPTIONAL_HEADER) & (pINH->OptionalHeader);
    PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(pAddress + pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    if ((pIID->Name) != 0xffff) {
        for (; pIID->Characteristics; ++pIID) {
            // Find ntdll.dll
            if (!strcmp("Ws2_32.dll", (char*)(pAddress + pIID->Name))) {
                break;
            }
        }  //  for (; moduleData.pIID->Characteristics; ++(moduleData.pIID)) {
    }  // if ((moduleData.pIID->Name) != 0xffff) {
}


void MakeHookForModule(LPBYTE a_pAddress, PIMAGE_IMPORT_DESCRIPTOR a_pIID)
{
    DWORD dwOld, dwOldTmp;
    size_t ind;
    // Search for a_funcname
    PIMAGE_THUNK_DATA pITD;
    PIMAGE_THUNK_DATA pFirstThunkTest;
    PIMAGE_IMPORT_BY_NAME pIIBM;

    pITD = (PIMAGE_THUNK_DATA)(a_pAddress + a_pIID->OriginalFirstThunk);
    pFirstThunkTest = (PIMAGE_THUNK_DATA)((a_pAddress + a_pIID->FirstThunk));

    //for (; !(pITD->u1.Ordinal & IMAGE_ORDINAL_FLAG) && pITD->u1.AddressOfData; ++pITD) {
    for (; pITD->u1.AddressOfData; ++pITD) {
        pIIBM = (PIMAGE_IMPORT_BY_NAME)(a_pAddress + pITD->u1.AddressOfData);
        if (!strcmp("socket", (const char*)(pIIBM->Name))) {
            VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(size_t), PAGE_READWRITE, &dwOld);
            if ((((const void*)(pFirstThunkTest->u1.Function)) == (&socket))) {
                //a_replaceData[ind].replaceIfAddressIs = (const void*)pFirstThunkTest->u1.Function;
                pFirstThunkTest->u1.Function = (size_t)(&MySocketFunc);
                //a_replaceData[ind].bFound = true;
            }
            VirtualProtect((LPVOID) & (pFirstThunkTest->u1.Function), sizeof(size_t), dwOld, &dwOldTmp);
            break;
        }  //  if (!strcmp(a_replaceData->funcname, (const char*)(pIIBM->Name))) {
        pFirstThunkTest++;
    }  //  for (; !(pITD->u1.Ordinal & IMAGE_ORDINAL_FLAG) && pITD->u1.AddressOfData; pITD++) {

}


#endif
