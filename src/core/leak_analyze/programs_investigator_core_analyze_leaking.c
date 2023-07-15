//
// file:			crash_investigator_linux_simple_analyze.cpp
// path:			src/core/crash_investigator_linux_simple_analyze.cpp
// created on:		2023 Mar 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <progs_invest/analyze_leaking.h>
#include <progs_invest/alloc_free_hook.h>
#include <stack_investigator/investigator.h>
#include <cinternal/c_raii.h>
#include <cinternal/logger.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>


CPPUTILS_BEGIN_C

#ifdef _WIN32
#define ctime_s_t(_timep,_buffer,_numberOfElements)     ctime_s(_buffer,_numberOfElements,_timep)
#else
#define ctime_s_t(_timep,_buffer,_numberOfElements)     CPPUTILS_STATIC_CAST(void,_numberOfElements);ctime_r(_timep,_buffer)
#endif

#define MEM_LEAK_ANALYZE_HASH_BY_ADR_BASKETS        131072
#define MEM_LEAK_ANALYZE_HASH_BY_STACK_BASKETS      8192
#define MEMORY_LEAK_ANALYZE_INIT_TIME_SEC_DEFAULT   8
#define MEMORY_LEAK_ANALYZE_MAX_ALLOC_DEFAULT       300



struct SMemoryLeakAnalyseItem{
    struct StackInvestBacktrace*    pStack;
    size_t                          countForThisStack;
    CinternalDLLHashItem_t          hashByStackIter;
};


static inline long int GetIntFromVariableInline(const char* a_cpcVariableName){
    char* pcEnvValue = CPPUTILS_NULL;
#ifdef _WIN32
    char  vcEnvVariable[1024];
    if(GetEnvironmentVariableA(a_cpcVariableName,vcEnvVariable,1023)>0){
        pcEnvValue = vcEnvVariable;
    }
    else{
        pcEnvValue = CPPUTILS_NULL;
    }
#else
    #ifdef secure_getenv_defined
    pcEnvValue = secure_getenv(a_cpcVariableName);
    #else
    pcEnvValue = getenv(a_cpcVariableName);
    #endif
#endif

    if(pcEnvValue){
        long int lnVal = strtol(pcEnvValue,CPPUTILS_NULL,10);
        CInternalLogDebug("!!!! %s = %ld",a_cpcVariableName,lnVal);
        return strtol(pcEnvValue,CPPUTILS_NULL,10);
    }
    return -1;
}


static void HashByStackDeallocator(void* a_pData)
{
    struct SMemoryLeakAnalyseItem*const pItem = (struct SMemoryLeakAnalyseItem*)a_pData;
    struct StackInvestBacktrace*const pStack = pItem->pStack;
    StackInvestFreeBacktraceData(pStack);
    AllocFreeHookCLibFree(pItem);
}


static size_t HashByStackHasher(const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    (void)a_keySize;
    struct StackInvestBacktrace*const pStack = (struct StackInvestBacktrace*)a_key;
    return StackInvestHashOfTheStack(pStack);
}


static bool IsTheSameStack(const void* a_key1, size_t a_keySize1, const void* a_key2, size_t a_keySize2) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_keySize1);
    CPPUTILS_STATIC_CAST(void, a_keySize2);
    const struct StackInvestBacktrace*const pStack1 = (const struct StackInvestBacktrace*)a_key1;
    const struct StackInvestBacktrace*const pStack2 = (const struct StackInvestBacktrace*)a_key2;
    return StackInvestIsTheSameStack(pStack1,pStack2);
}


static bool StoreStackHashKey(TypeCinternalAllocator a_allocator, void** a_pKeyStore, size_t* a_pKeySizeStore, const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_allocator);
    CPPUTILS_STATIC_CAST(void, a_keySize);
    CPPUTILS_STATIC_CAST(void, a_pKeySizeStore);
    *a_pKeyStore = CPPUTILS_CONST_CAST(void*,a_key);
    return true;
}


static void UnstoreStackHashKey(TypeCinternalDeallocator a_deallocator, void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_deallocator);
    CPPUTILS_STATIC_CAST(void, a_key);
    CPPUTILS_STATIC_CAST(void, a_keySize);
}


static void ClearThreadLocalKeyStatic(void* a_pData)
{
    CPPUTILS_STATIC_CAST(void,a_pData);
}


PRINV_LEAKA_EXPORT int ProgsInvestAnalyzeLeakingInitialize(struct SPrInvAnalyzeLeakingData* a_pTables,
                                                           TypeFinalAction CPPUTILS_ARG_NN a_fncWhenLeak, void* a_pUserData,
                                                           const char* a_cpcStartTimeEnv, const char* a_cpcMaxForStackEnv)
{
    int nRet;
    long int deltaTimeToStartAnalyze;
    int maxForStack;

    nRet = cinternal_lw_recursive_mutex_create(&(a_pTables->mutexForHashes));
    if(nRet){return nRet;}

    a_pTables->hashByAddress = CInternalDLLHashCreateExSmlInt(MEM_LEAK_ANALYZE_HASH_BY_ADR_BASKETS,&AllocFreeHookCLibMalloc,&AllocFreeHookCLibFree);
    if(!(a_pTables->hashByAddress)){
        cinternal_lw_recursive_mutex_destroy(&(a_pTables->mutexForHashes));
        return 1;
    }

    a_pTables->hashByStack = CInternalDLLHashCreateExAny(MEM_LEAK_ANALYZE_HASH_BY_STACK_BASKETS,&HashByStackHasher,&IsTheSameStack,
                                                         &StoreStackHashKey,&UnstoreStackHashKey,&AllocFreeHookCLibMalloc,&AllocFreeHookCLibFree);
    if(!(a_pTables->hashByStack)){
        CInternalDLLHashDestroy(a_pTables->hashByAddress);
        cinternal_lw_recursive_mutex_destroy(&(a_pTables->mutexForHashes));
        return 1;
    }

    nRet = CinternalTlsAlloc(&(a_pTables->ignoreForThisThreadTlsKey),&ClearThreadLocalKeyStatic);
    if(nRet){
        CInternalDLLHashDestroy(a_pTables->hashByStack);
        CInternalDLLHashDestroy(a_pTables->hashByAddress);
        cinternal_lw_recursive_mutex_destroy(&(a_pTables->mutexForHashes));
        return 1;
    }

    deltaTimeToStartAnalyze = GetIntFromVariableInline(a_cpcStartTimeEnv);
    maxForStack = GetIntFromVariableInline(a_cpcMaxForStackEnv);

    a_pTables->bInitializationTimeNotPassed = true;
    a_pTables->initTimeSec = time(&(a_pTables->initTimeSec));
    a_pTables->deltaTimeToStartAnalyze = (deltaTimeToStartAnalyze<0) ? MEMORY_LEAK_ANALYZE_INIT_TIME_SEC_DEFAULT : CPPUTILS_STATIC_CAST(time_t,deltaTimeToStartAnalyze);
    a_pTables->countForThisStackMax = (maxForStack<0) ? MEMORY_LEAK_ANALYZE_MAX_ALLOC_DEFAULT : CPPUTILS_STATIC_CAST(size_t,maxForStack);
    a_pTables->fncWhenLeak = a_fncWhenLeak;
    a_pTables->pUserData = a_pUserData;
    a_pTables->unMaxValue = 0;

    return 0;
}


PRINV_LEAKA_EXPORT void ProgsInvestAnalyzeLeakingClean(struct SPrInvAnalyzeLeakingData* a_pTables)
{
    CinternalTlsDelete(a_pTables->ignoreForThisThreadTlsKey);
    CInternalDLLHashDestroy(a_pTables->hashByAddress);
    CInternalDLLHashDestroyEx(a_pTables->hashByStack,&HashByStackDeallocator);
    cinternal_lw_recursive_mutex_destroy(&(a_pTables->mutexForHashes));
}


PRINV_LEAKA_EXPORT void ProgsInvestFinalActionPrintStackAndExit(void* a_pUserData,const struct StackInvestBacktrace* a_pStack)
{
    struct StackInvestStackItem*const pStackItems = (struct StackInvestStackItem*)AllocFreeHookCLibMalloc(((size_t)(a_pStack->stackDeepness))*sizeof(struct StackInvestStackItem));
    CInternalLogError("  possible memory leak!!!!!. UserData = %p",a_pUserData);
    if(pStackItems){
        if(!StackInvestConvertBacktraceToNames(a_pStack,pStackItems)){
            int i=0;
            for(;i<a_pStack->stackDeepness;++i){
                if(pStackItems[i].line>=0){
                    CInternalLogError("bin: %s, %s(%d), fn:%s",
                                      pStackItems[i].binFile, pStackItems[i].sourceFile,
                                      pStackItems[i].line, pStackItems[i].funcName);
                }
                else{
                    CInternalLogError("%s",pStackItems[i].binFile);
                }
            }  //  for(;i<pItem->pStack->stackDeepness;++i){
        }  //  if(!StackInvestConvertBacktraceToNames(pItem->pStack,pStackItems)){
    }  //  if(pStackItems){
    exit(1);
}


static inline int CrashInvestAnalyzeLeakingAddAllocedItemNoLockInline(int a_goBackInTheStackCalc, void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables){
    CinternalDLLHashItem_t hashIterAdr, hashIterStack;
    size_t unHashAdr, unHashStack;
    struct StackInvestBacktrace* pCurStack;
    struct SMemoryLeakAnalyseItem* pItem;

    pCurStack = StackInvestInitBacktraceDataForCurrentStack(a_goBackInTheStackCalc+1);
    if(!pCurStack){
        return 1;
    }

    hashIterStack = CInternalDLLHashFindEx(a_pTables->hashByStack,pCurStack,0,&unHashStack);
    if(hashIterStack){  // this stack is there
        char* pcTemp;
        char vcCurTimeStr[128];
        time_t currentTime;

        currentTime = time(&currentTime);
        ctime_s_t(&currentTime,vcCurTimeStr,127);
        pcTemp = strchr(vcCurTimeStr,'\n');
        StackInvestFreeBacktraceData(pCurStack);  //  no need to keep this stack data twce
        pItem = (struct SMemoryLeakAnalyseItem*)(hashIterStack->data);
        if((++(pItem->countForThisStack))>(a_pTables->countForThisStackMax)){
            (*(a_pTables->fncWhenLeak))(a_pTables->pUserData,pItem->pStack);
        }
        else if(pItem->countForThisStack>(a_pTables->unMaxValue)){
            a_pTables->unMaxValue = pItem->countForThisStack;
            if(pcTemp){
                *pcTemp = 0;
                CInternalLogDebug("!!! %s new max =>  %d",vcCurTimeStr,(int)(a_pTables->unMaxValue));
            }
            else{
                CInternalLogDebug("!!!!!!! new max =>  %d",(int)(a_pTables->unMaxValue));
            }
        }
    }  //  if(hashIterStack){
    else{
        pItem = (struct SMemoryLeakAnalyseItem*)AllocFreeHookCLibMalloc(sizeof(struct SMemoryLeakAnalyseItem));
        if(!pItem){
            StackInvestFreeBacktraceData(pCurStack);
            AllocFreeHookCLibFree(a_ptr);
            return 1;
        }
        pItem->pStack = pCurStack;
        pItem->countForThisStack = 1;
        pItem->hashByStackIter = CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByStack,pItem,pCurStack,0,unHashStack);
    }

    hashIterAdr = CInternalDLLHashFindEx(a_pTables->hashByAddress,a_ptr,0,&unHashAdr);
    assert(hashIterAdr==CPPUTILS_NULL);
    CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByAddress,pItem,a_ptr,0,unHashAdr);

    return 0;
}


static inline void CrashInvestAnalyzeLeakingRemoveAllocedItemNoLockInline(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables){
    CinternalDLLHashItem_t hashIterAdr;
    size_t unHashAdr;

    if(!a_ptr){return;}

    hashIterAdr = CInternalDLLHashFindEx(a_pTables->hashByAddress,a_ptr,0,&unHashAdr);
    // assert(hashIterAdr);
    // `assert` is not ok, because of initialization time, some memories are not here  // this is not correct
    // some memories are not here because when locking done with s_nIgnoreForThisThread, then it is not inserted
    if(hashIterAdr){
        struct SMemoryLeakAnalyseItem* pItem;

        pItem = (struct SMemoryLeakAnalyseItem*)(hashIterAdr->data);
        CInternalDLLHashRemoveDataEx(a_pTables->hashByAddress,hashIterAdr);
        assert((pItem->countForThisStack)>0);
        if((--(pItem->countForThisStack))==0){
            struct StackInvestBacktrace*const pStack = pItem->pStack;
            CInternalDLLHashRemoveDataEx(a_pTables->hashByStack,pItem->hashByStackIter);
            StackInvestFreeBacktraceData(pStack);
            AllocFreeHookCLibFree(pItem);
        }
    }  //  if(hashIterAdr){
}


//#define PR_INV_INCREMENT_PTR(_ptr_ptr)  (++((char*)(*(_ptr_ptr))))
//#define PR_INV_DECREMENT_PTR(_ptr_ptr)  (--((char*)(*(_ptr_ptr))))

static inline void PR_INV_INCREMENT_PTR(CinternalTlsData a_tlsKey){
    char* pcCurrentPtr = (char*)CinternalTlsGetSpecific(a_tlsKey);
    CinternalTlsSetSpecific(a_tlsKey,++pcCurrentPtr);
}
static inline void PR_INV_DECREMENT_PTR(CinternalTlsData a_tlsKey){
    char* pcCurrentPtr = (char*)CinternalTlsGetSpecific(a_tlsKey);
    CinternalTlsSetSpecific(a_tlsKey,--pcCurrentPtr);
}



PRINV_LEAKA_EXPORT int  ProgsInvestAnalyzeLeakingAddAllocedItem(int a_goBackInTheStackCalc, void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables)
{
    int nRet;

    if(a_pTables->ignoreForThisThreadTlsKey){
        return 0;
    }
    else {
        if(!a_ptr){return 0;}
        PR_INV_INCREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
        if(a_pTables->bInitializationTimeNotPassed){
            time_t currentTime;
            currentTime = time(&currentTime);
            if((currentTime - (a_pTables->initTimeSec))>(a_pTables->deltaTimeToStartAnalyze)){
                a_pTables->bInitializationTimeNotPassed = false;
            }
            else{
                PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
                return 0;
            }
        }
    }  //  else of if((*(a_pTables->pnIgnoreForThisThread))>0){

    cinternal_lw_recursive_mutex_lock(&(a_pTables->mutexForHashes));
    nRet = CrashInvestAnalyzeLeakingAddAllocedItemNoLockInline(a_goBackInTheStackCalc+1,a_ptr,a_pTables);
    cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
    PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    return nRet;
}


PRINV_LEAKA_EXPORT void ProgsInvestAnalyzeLeakingRemoveAllocedItem(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables)
{
    PR_INV_INCREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    cinternal_lw_recursive_mutex_lock(&(a_pTables->mutexForHashes));
    CrashInvestAnalyzeLeakingRemoveAllocedItemNoLockInline(a_ptr,a_pTables);
    cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
    PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
}


CPPUTILS_END_C
