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
#include <stdint.h>
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
#define MEMORY_LEAK_ANALYZE_MAX_ALLOC_DEFAULT       250


struct SPrInvAnalyzeLeakingData {
    cinternal_lw_recursive_mutex_t  mutexForHashes;
    CinternalDLLHash_t              hashByAddress;
    CinternalDLLHash_t              hashByStack;
    CinternalTlsData                ignoreForThisThreadTlsKey;
    size_t                          countForThisStackMax;
    size_t                          unMaxValue;
    time_t                          initTimeSec;
    time_t                          deltaTimeToStartAnalyze;
    void*                           pUserData;
    TypeAllocFreeHookMalloc         m_malloc;
    TypeAllocFreeHookCalloc         m_calloc;
    TypeAllocFreeHookRealloc        m_realloc;
    TypeAllocFreeHookFree           m_free;
    TypeFinalAction                 fncWhenLeak;
    uint16_t                        hasRecoveryStack;
    bool                            bInitializationTimeNotPassed;
    bool                            keepLeakingStacks;
    bool                            analyzeOngoing;
    char                            reserved01[sizeof(void*) - 3*sizeof(bool)-sizeof(uint16_t)];
};


struct SPrInvLeakAnlzStackItemPriv;

struct SPrInvLeakAnlzMem{
    struct SPrInvLeakAnlzStackItemPriv* pStacItem;
    struct SPrInvLeakAnlzMem* prev;
    struct SPrInvLeakAnlzMem* next;
    void* pPtr;
    size_t  size;
};



struct SPrInvLeakAnlzStackItemPriv{
    struct SPrInvLeakAnlzStackItem2  pbl;
    CinternalDLLHashItem_t          hashByStackIter;
    struct SPrInvLeakAnlzMem*       first;
    struct SPrInvLeakAnlzMem*       last;
    bool                            isRecovery;
    bool                            reserved01[sizeof(void*)-sizeof(bool)];
};

static void ProgsInvestAnalyzeLeakingAddAllocedItemStatic(int a_goBackInTheStackCalc, void* a_ptr, size_t a_size, struct SPrInvAnalyzeLeakingData* a_pTables);
static void ProgsInvestAnalyzeLeakingRemoveAllocedItemStatic(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables);


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


static inline bool ProgsInvestAnalyzeLeakingHasReasonForEarlyReturnInline(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables){
    if(a_pTables->analyzeOngoing){
        if(CinternalTlsGetSpecific(a_pTables->ignoreForThisThreadTlsKey)){
            return true;
        }
        else {
            if(!a_ptr){return true;}
            PR_INV_INCREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
            if(a_pTables->bInitializationTimeNotPassed){
                time_t currentTime;
                currentTime = time(&currentTime);
                if((currentTime - (a_pTables->initTimeSec))>(a_pTables->deltaTimeToStartAnalyze)){
                    a_pTables->bInitializationTimeNotPassed = false;
                }
                else{
                    PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
                    return true;
                }
            }
        }  //  else of if((*(a_pTables->pnIgnoreForThisThread))>0){
        return false;
    }
    return true;
}


static void HashByStackDeallocator(void* a_pData)
{
    struct SPrInvLeakAnlzStackItemPriv*const pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)a_pData;
    struct StackInvestBacktrace*const pStack = pStackItem->pbl.pStack;
    StackInvestFreeBacktraceData(pStack);
    AllocFreeHookCLibFree(pStackItem);
}


static void HashByAddressDeallocator(void* a_pData)
{
    struct SPrInvLeakAnlzMem* const pAddrItem = (struct SPrInvLeakAnlzMem*)a_pData;
    AllocFreeHookCLibFree(pAddrItem);
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


static inline struct SPrInvLeakAnlzMem* CreateMemoryItemInline(struct SPrInvLeakAnlzStackItemPriv* CPPUTILS_ARG_NN a_pStackItem, void* CPPUTILS_ARG_NN a_ptr, size_t a_size){
    struct SPrInvLeakAnlzMem* pMemItem = (struct SPrInvLeakAnlzMem*)AllocFreeHookCLibMalloc(sizeof(struct SPrInvLeakAnlzMem));
    if(!pMemItem){
        return CPPUTILS_NULL;
    }
    if(a_pStackItem->last){
        a_pStackItem->last->next = pMemItem;
    }
    else{
        a_pStackItem->first = pMemItem;
    }
    pMemItem->prev = a_pStackItem->last;
    pMemItem->next = CPPUTILS_NULL;
    pMemItem->pStacItem = a_pStackItem;
    pMemItem->pPtr = a_ptr;
    pMemItem->size = a_size;
    a_pStackItem->last = pMemItem;
    return pMemItem;
}


static inline void RemoveMemoryItemInline(struct SPrInvLeakAnlzMem* CPPUTILS_ARG_NN a_pMemItem){
    if(a_pMemItem->next){a_pMemItem->next->prev = a_pMemItem->prev;}
    if(a_pMemItem->prev){a_pMemItem->prev->next = a_pMemItem->next;}
    if(a_pMemItem==(a_pMemItem->pStacItem->first)){a_pMemItem->pStacItem->first = a_pMemItem->next;}
    if(a_pMemItem==(a_pMemItem->pStacItem->last)){a_pMemItem->pStacItem->last = a_pMemItem->prev;}
    AllocFreeHookCLibFree(a_pMemItem);
}


static inline void  PrInvLeakAnalyzeAnalyzeWhenHasStackItemInline(struct SPrInvAnalyzeLeakingData* CPPUTILS_ARG_NN a_pTables,
                                                                 struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pCurStack,
                                                                 void* CPPUTILS_ARG_NN a_ptr, size_t a_size,
                                                                 CinternalDLLHashItem_t a_hashIterStack){
    size_t unHashAdr;
    CinternalDLLHashItem_t hashIterAdr;
    struct SPrInvLeakAnlzStackItemPriv* const pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)(a_hashIterStack->data);
    struct SPrInvLeakAnlzMem* const pMemItem = CreateMemoryItemInline(pStackItem,a_ptr,a_size);
    StackInvestFreeBacktraceData(a_pCurStack);  //  no need to keep this stack data twce
    if(!pMemItem){
        CInternalLogError("Unable create memory!");
        exit(1);
    }
    hashIterAdr = CInternalDLLHashFindEx(a_pTables->hashByAddress,a_ptr,0,&unHashAdr);
    assert(hashIterAdr==CPPUTILS_NULL);
    hashIterAdr = CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByAddress,pMemItem,a_ptr,0,unHashAdr);
    if(!hashIterAdr){
        RemoveMemoryItemInline(pMemItem);
        CInternalLogError("Unable add memory to hash!");
        exit(1);
    }
    if((++(pStackItem->pbl.countForThisStack))>(a_pTables->countForThisStackMax)){
        (*(a_pTables->fncWhenLeak))(a_pTables,&(pStackItem->pbl));
    }
    else if(pStackItem->pbl.countForThisStack>(a_pTables->unMaxValue)){
        a_pTables->unMaxValue = pStackItem->pbl.countForThisStack;
        CInternalLogDebug("!!!!!!! new max =>  %d",(int)(a_pTables->unMaxValue));
    }
}


static inline void PrInvLeakAnalyzeAnalyzeWhenNoStackItemInline(struct SPrInvAnalyzeLeakingData* CPPUTILS_ARG_NN a_pTables,
                                                                struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pCurStack,
                                                                void* CPPUTILS_ARG_NN a_ptr, size_t a_size, size_t a_unHashStack){
    size_t unHashAdr;
    CinternalDLLHashItem_t hashIterAdr;
    struct SPrInvLeakAnlzMem* pMemItem;
    struct SPrInvLeakAnlzStackItemPriv* const pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)AllocFreeHookCLibMalloc(sizeof(struct SPrInvLeakAnlzStackItemPriv));
    if(!pStackItem){
        StackInvestFreeBacktraceData(a_pCurStack);
        CInternalLogError("Unable create memory!");
        exit(1);
    }

    pStackItem->pbl.pStack = a_pCurStack;
    pStackItem->pbl.countForThisStack = 1;
    pStackItem->hashByStackIter = CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByStack,pStackItem,a_pCurStack,0,a_unHashStack);
    pStackItem->first = CPPUTILS_NULL;
    pStackItem->last = CPPUTILS_NULL;
    pStackItem->isRecovery = false;

    pMemItem = CreateMemoryItemInline(pStackItem,a_ptr,a_size);
    if(!pMemItem){
        CInternalLogError("Unable create memory!");
        exit(1);
    }
    hashIterAdr = CInternalDLLHashFindEx(a_pTables->hashByAddress,a_ptr,0,&unHashAdr);
    assert(hashIterAdr==CPPUTILS_NULL);
    hashIterAdr = CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByAddress,pMemItem,a_ptr,0,unHashAdr);
}


static inline void CrashInvestAnalyzeLeakingAddAllocedItemNoLockInline(void* a_ptr, size_t a_size,
                                                                      struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pCurStack,
                                                                      struct SPrInvAnalyzeLeakingData* CPPUTILS_ARG_NN a_pTables){
    CinternalDLLHashItem_t hashIterStack;
    size_t unHashStack;

    hashIterStack = CInternalDLLHashFindEx(a_pTables->hashByStack,a_pCurStack,0,&unHashStack);
    if(hashIterStack){  // this stack is there
        PrInvLeakAnalyzeAnalyzeWhenHasStackItemInline(a_pTables,a_pCurStack,a_ptr,a_size,hashIterStack);
    }
    else{
        PrInvLeakAnalyzeAnalyzeWhenNoStackItemInline(a_pTables,a_pCurStack,a_ptr, a_size,unHashStack);
    }
}


static inline void* ProgsInvestAnalyzeLeakingMallocCallocBelowInline(struct SPrInvAnalyzeLeakingData* CPPUTILS_ARG_NN a_pTables,
                                                                     struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pCurStack,
                                                                     size_t a_count, size_t a_elemSize,
                                                                     void* (*a_fpAlloc)(const struct SPrInvAnalyzeLeakingData* a_pTables, size_t a_count, size_t a_elemSize)){
    void* pRet;
    const size_t cunSizeAll = a_count * a_elemSize;
    if(a_pTables->hasRecoveryStack){
        size_t unHashStack;
        const CinternalDLLHashItem_t hashIterStack = CInternalDLLHashFindEx(a_pTables->hashByStack,a_pCurStack,0,&unHashStack);
        if(hashIterStack){  // this stack is there SPrInvLeakAnlzMem
            struct SPrInvLeakAnlzStackItemPriv* const pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)(hashIterStack->data);
            if(pStackItem->isRecovery){
                struct SPrInvLeakAnlzMem* pMemItem;
                pMemItem = pStackItem->first;
                while(pMemItem){
                    if(pMemItem->size>=cunSizeAll){
                        return pMemItem->pPtr;
                    }
                }  //  while(pMemItem){
            }  //  if(pStackItem->isRecovery){
        }  //  if(hashIterStack){  // this stack is there SPrInvLeakAnlzMem
    }  // if(a_pTables->hasRecoveryStack){

    pRet = (*a_fpAlloc)(a_pTables,a_count, a_elemSize);
    if(pRet){
        CrashInvestAnalyzeLeakingAddAllocedItemNoLockInline(pRet,cunSizeAll,a_pCurStack,a_pTables);
    }
    return pRet;
}


static inline void* ProgsInvestAnalyzeLeakingMallocCallocInline(struct SPrInvAnalyzeLeakingData* a_pTables,
                                                                int a_goBackInTheStackCalc, size_t a_count, size_t a_elemSize,
                                                                void* (*a_fpAlloc)(const struct SPrInvAnalyzeLeakingData* a_pTables, size_t a_count, size_t a_elemSize))
{
    void* pRet;
    struct StackInvestBacktrace* pCurStack;

    if(ProgsInvestAnalyzeLeakingHasReasonForEarlyReturnInline((void*)1,a_pTables)){
        return (*a_fpAlloc)(a_pTables,a_count, a_elemSize);
    }

    pCurStack= StackInvestInitBacktraceDataForCurrentStack(++a_goBackInTheStackCalc);
    if(!pCurStack){
        PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
        return CPPUTILS_NULL;
    }

    cinternal_lw_recursive_mutex_lock(&(a_pTables->mutexForHashes));
    pRet = ProgsInvestAnalyzeLeakingMallocCallocBelowInline(a_pTables,pCurStack,a_count,a_elemSize,a_fpAlloc);
    if(CinternalTlsGetSpecific(a_pTables->ignoreForThisThreadTlsKey)){
        cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
        PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    }
    return pRet;
}


static void* MallocAsCallocStatic(const struct SPrInvAnalyzeLeakingData* a_pTables,size_t a_count, size_t a_elemSize)
{
    assert(a_elemSize==1);
    return (*(a_pTables->m_malloc))(a_count);
}


static void* CallocAsCallocStatic(const struct SPrInvAnalyzeLeakingData* a_pTables,size_t a_count, size_t a_elemSize)
{
    return (*(a_pTables->m_calloc))(a_count,a_elemSize);
}


PRINV_LEAKA_EXPORT struct SPrInvAnalyzeLeakingData* ProgsInvestAnalyzeLeakingInitialize(
    void* a_pUserData,
    TypeAllocFreeHookMalloc CPPUTILS_ARG_NN a_malloc,TypeAllocFreeHookCalloc CPPUTILS_ARG_NN a_calloc,
    TypeAllocFreeHookRealloc CPPUTILS_ARG_NN a_realloc, TypeAllocFreeHookFree CPPUTILS_ARG_NN a_free,
    TypeFinalAction a_fncWhenLeak,const char* a_cpcStartTimeEnv, const char* a_cpcMaxForStackEnv)
{
    int nRet;
    long int deltaTimeToStartAnalyze;
    int maxForStack;
    struct SPrInvAnalyzeLeakingData* const pTables = (struct SPrInvAnalyzeLeakingData*)AllocFreeHookCLibMalloc(sizeof(struct SPrInvAnalyzeLeakingData));
    if(!pTables){
        return CPPUTILS_NULL;
    }

    nRet = cinternal_lw_recursive_mutex_create(&(pTables->mutexForHashes));
    if(nRet){
        AllocFreeHookCLibFree(pTables);
        return CPPUTILS_NULL;
    }

    pTables->hashByAddress = CInternalDLLHashCreateExSmlInt(MEM_LEAK_ANALYZE_HASH_BY_ADR_BASKETS,&AllocFreeHookCLibMalloc,&AllocFreeHookCLibFree);
    if(!(pTables->hashByAddress)){
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        AllocFreeHookCLibFree(pTables);
        return CPPUTILS_NULL;
    }

    pTables->hashByStack = CInternalDLLHashCreateExAny(MEM_LEAK_ANALYZE_HASH_BY_STACK_BASKETS,&HashByStackHasher,&IsTheSameStack,
                                                         &StoreStackHashKey,&UnstoreStackHashKey,&AllocFreeHookCLibMalloc,&AllocFreeHookCLibFree);
    if(!(pTables->hashByStack)){
        CInternalDLLHashDestroy(pTables->hashByAddress);
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        AllocFreeHookCLibFree(pTables);
        return CPPUTILS_NULL;
    }

    nRet = CinternalTlsAlloc(&(pTables->ignoreForThisThreadTlsKey),&ClearThreadLocalKeyStatic);
    if(nRet){
        CInternalDLLHashDestroy(pTables->hashByStack);
        CInternalDLLHashDestroy(pTables->hashByAddress);
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        AllocFreeHookCLibFree(pTables);
        return CPPUTILS_NULL;
    }

    deltaTimeToStartAnalyze = GetIntFromVariableInline(a_cpcStartTimeEnv);
    maxForStack = GetIntFromVariableInline(a_cpcMaxForStackEnv);

    pTables->bInitializationTimeNotPassed = true;
    pTables->initTimeSec = time(&(pTables->initTimeSec));
    pTables->deltaTimeToStartAnalyze = (deltaTimeToStartAnalyze<0) ? MEMORY_LEAK_ANALYZE_INIT_TIME_SEC_DEFAULT : CPPUTILS_STATIC_CAST(time_t,deltaTimeToStartAnalyze);
    pTables->countForThisStackMax = (maxForStack<0) ? MEMORY_LEAK_ANALYZE_MAX_ALLOC_DEFAULT : CPPUTILS_STATIC_CAST(size_t,maxForStack);
    pTables->pUserData = a_pUserData;
    pTables->m_malloc = a_malloc;
    pTables->m_calloc = a_calloc;
    pTables->m_realloc = a_realloc;
    pTables->m_free = a_free;
    pTables->fncWhenLeak = a_fncWhenLeak?a_fncWhenLeak:(&ProgsInvestFinalActionPrintStackAndExit);
    pTables->keepLeakingStacks = ((pTables->fncWhenLeak)==(&ProgsInvestFinalActionTryFixLeak))?true:false;
    pTables->hasRecoveryStack = 0;
    pTables->unMaxValue = 0;
    pTables->analyzeOngoing = true;

    return pTables;
}


PRINV_LEAKA_EXPORT void ProgsInvestAnalyzeLeakingClean(struct SPrInvAnalyzeLeakingData* a_pTables)
{
    CinternalTlsDelete(a_pTables->ignoreForThisThreadTlsKey);
    CInternalDLLHashDestroyEx(a_pTables->hashByAddress,&HashByAddressDeallocator);
    CInternalDLLHashDestroyEx(a_pTables->hashByStack,&HashByStackDeallocator);
    cinternal_lw_recursive_mutex_destroy(&(a_pTables->mutexForHashes));
}


PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingMalloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, size_t a_size)
{
    return ProgsInvestAnalyzeLeakingMallocCallocInline(a_pTables,++a_goBackInTheStackCalc,a_size,1,&MallocAsCallocStatic);
}


PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingCalloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, size_t a_count, size_t a_elemSize)
{
    return ProgsInvestAnalyzeLeakingMallocCallocInline(a_pTables,++a_goBackInTheStackCalc,a_count,a_elemSize,&CallocAsCallocStatic);
}


PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingRealloc(struct SPrInvAnalyzeLeakingData* a_pTables, int a_goBackInTheStackCalc, void* a_ptr, size_t a_newSize)
{
    void*const pRet = (*(a_pTables->m_realloc))(a_ptr,a_newSize);
    if(a_newSize){
        if(pRet!=a_ptr){
            ProgsInvestAnalyzeLeakingRemoveAllocedItemStatic(a_ptr,a_pTables);
            ProgsInvestAnalyzeLeakingAddAllocedItemStatic(++a_goBackInTheStackCalc,pRet,a_newSize,a_pTables);
        }
    }
    else{
        ProgsInvestAnalyzeLeakingRemoveAllocedItemStatic(a_ptr,a_pTables);
    }

    return pRet;
}


PRINV_LEAKA_EXPORT void  ProgsInvestAnalyzeLeakingFree(struct SPrInvAnalyzeLeakingData* a_pTables, void* a_ptr)
{
    ProgsInvestAnalyzeLeakingRemoveAllocedItemStatic(a_ptr,a_pTables);
    (*(a_pTables->m_free))(a_ptr);
}


PRINV_LEAKA_EXPORT void* ProgsInvestAnalyzeLeakingGetUserData(struct SPrInvAnalyzeLeakingData* a_pTables)
{
    return a_pTables->pUserData;
}


PRINV_LEAKA_EXPORT void ProgsInvestFinalActionPrintStackAndExit(struct SPrInvAnalyzeLeakingData* a_pTables,struct SPrInvLeakAnlzStackItem2* a_pStackItem)
{
    const struct StackInvestOptimalPrint* pStackOptPrint;

    CInternalLogError("  possible memory leak!!!!!  pUserData = %p. Stack will be printed and pogram will exit",a_pTables->pUserData);

    a_pTables->analyzeOngoing = false;
    if(CinternalTlsGetSpecific(a_pTables->ignoreForThisThreadTlsKey)){
        cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
        PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    }

    pStackOptPrint = StackInvestOptimalPrintCreate(a_pStackItem->pStack,0,a_pStackItem->pStack->stackDeepness);
    if(pStackOptPrint){
        StackInvestOptimalPrintPrint(pStackOptPrint);
        StackInvestOptimalPrintClean(pStackOptPrint);
    }

    exit(1);
}


PRINV_LEAKA_EXPORT void ProgsInvestFinalActionTryFixLeak(struct SPrInvAnalyzeLeakingData* a_pTables,struct SPrInvLeakAnlzStackItem2* a_pStackItem)
{
    struct SPrInvLeakAnlzStackItemPriv* pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)a_pStackItem;
    if(!(pStackItem->isRecovery)){
        pStackItem->isRecovery = true;
        ++(a_pTables->hasRecoveryStack);
    }
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
        struct SPrInvLeakAnlzMem* const pMemItem = (struct SPrInvLeakAnlzMem*)(hashIterAdr->data);
        struct SPrInvLeakAnlzStackItemPriv* const pStackItem = pMemItem->pStacItem;
        CInternalDLLHashRemoveDataEx(a_pTables->hashByAddress,hashIterAdr);
        assert((pMemItem->pStacItem->pbl.countForThisStack)>0);
        RemoveMemoryItemInline(pMemItem);
        if((--(pStackItem->pbl.countForThisStack))<3){
            if(pStackItem->isRecovery){
                pStackItem->isRecovery = false;
                --(a_pTables->hasRecoveryStack);
            }

            if((pStackItem->pbl.countForThisStack)==0){
                struct StackInvestBacktrace*const pStack = pStackItem->pbl.pStack;
                CInternalDLLHashRemoveDataEx(a_pTables->hashByStack,pStackItem->hashByStackIter);
                StackInvestFreeBacktraceData(pStack);
                AllocFreeHookCLibFree(pStackItem);
            }
        }
    }  //  if(hashIterAdr){
}



static void ProgsInvestAnalyzeLeakingAddAllocedItemStatic(int a_goBackInTheStackCalc, void* a_ptr, size_t a_size, struct SPrInvAnalyzeLeakingData* a_pTables)
{
    struct StackInvestBacktrace* pCurStack;

    if(ProgsInvestAnalyzeLeakingHasReasonForEarlyReturnInline(a_ptr,a_pTables)){
        return;
    }

    pCurStack = StackInvestInitBacktraceDataForCurrentStack(++a_goBackInTheStackCalc);
    if(!pCurStack){
        exit(1);
    }

    cinternal_lw_recursive_mutex_lock(&(a_pTables->mutexForHashes));
    CrashInvestAnalyzeLeakingAddAllocedItemNoLockInline(a_ptr,a_size,pCurStack,a_pTables);
    if(CinternalTlsGetSpecific(a_pTables->ignoreForThisThreadTlsKey)){
        cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
        PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    }
}


static void ProgsInvestAnalyzeLeakingRemoveAllocedItemStatic(void* a_ptr, struct SPrInvAnalyzeLeakingData* a_pTables)
{
    PR_INV_INCREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    cinternal_lw_recursive_mutex_lock(&(a_pTables->mutexForHashes));
    CrashInvestAnalyzeLeakingRemoveAllocedItemNoLockInline(a_ptr,a_pTables);
    if(CinternalTlsGetSpecific(a_pTables->ignoreForThisThreadTlsKey)){
        cinternal_lw_recursive_mutex_unlock(&(a_pTables->mutexForHashes));
        PR_INV_DECREMENT_PTR(a_pTables->ignoreForThisThreadTlsKey);
    }
}


#ifndef _WIN32


CPPUTILS_DLL_PRIVATE void* STACK_INVEST_RW_MUTEX_CREATE_function(void){
    return (void*)1;
}


CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_DESTROY_function(void* a_pRwMutex){
    CPPUTILS_STATIC_CAST(void,a_pRwMutex);
}


CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_LOCK_function(void* a_pRwMutex){
    CPPUTILS_STATIC_CAST(void,a_pRwMutex);
}


CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_LOCK_function(void* a_pRwMutex){
    CPPUTILS_STATIC_CAST(void,a_pRwMutex);
}


CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_RD_UNLOCK_function(void* a_pRwMutex){
    CPPUTILS_STATIC_CAST(void,a_pRwMutex);
}


CPPUTILS_DLL_PRIVATE void STACK_INVEST_RW_MUTEX_WR_UNLOCK_function(void* a_pRwMutex){
    CPPUTILS_STATIC_CAST(void,a_pRwMutex);
}


#endif  //  #ifndef _WIN32


CPPUTILS_END_C
