//
// file:			crash_investigator_linux_simple_analyze.cpp
// path:			src/core/crash_investigator_linux_simple_analyze.cpp
// created on:		2023 Mar 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <progs_invest/analyze_leaking02.h>
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


struct SPrInvLeakAnalyzerDataAll {
    struct SPrInvLeakAnalyzerData              publ;
    cinternal_lw_recursive_mutex_t              mutexForHashes;
    TypePrInvLeakAnalyzerAnalyzeLeakingStack    m_stackAnalyzer;
    TypePrInvLeakAnalyzerMalloc                 m_malloc;
    TypePrInvLeakAnalyzerFree                   m_free;
    CinternalDLLHash_t                          hashByResource;
    CinternalDLLHash_t                          hashByStack;
    CinternalTlsData                            ignoreForThisThreadTlsKey;
    size_t                                      unMaxValue;
    time_t                                      initTimeSec;
    time_t                                      deltaTimeToStartAnalyze;
    bool                                        bInitializationTimeNotPassed;
    char                                        reserved01[sizeof(void*) - 1*sizeof(bool)];
};


struct SStackItem {
    CinternalDLLHashItem_t hashIterStack;
    struct StackInvestBacktrace* m_pStack;
    size_t  countForThisStack;
};


#define PUBL_TABLE_TO_ALL_TABLE(_pub_table) ((struct SPrInvLeakAnalyzerDataAll*)(_pub_table))
#define TABLE_ITEM_TO_DATA(_item)           ((struct SStackItem*)((_item)->data))
#define INLINE_RETURN_EXIT                  -1
#define INLINE_RETURN_KEEP_STACK            0
#define INLINE_RETURN_FREE_STACK            2
#define INLINE_RETURN_CALL_STACK_ANALYZE    3


static size_t HashByStackHasher(const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT;
static bool IsTheSameStack(const void* a_key1, size_t a_keySize1, const void* a_key2, size_t a_keySize2) CPPUTILS_NOEXCEPT;
static bool StoreStackHashKey(TypeCinternalAllocator a_allocator, void** a_pKeyStore, size_t* a_pKeySizeStore, const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT;
static void UnstoreStackHashKey(TypeCinternalDeallocator a_deallocator, void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT;
static void ClearThreadLocalKeyStatic(void* a_pData) CPPUTILS_NOEXCEPT;
static void HashByStackDeallocator(void* a_pData) CPPUTILS_NOEXCEPT;
static void HashByAddressDeallocator(void* a_pData) CPPUTILS_NOEXCEPT;
static void PrInvLeakAnalyzerAnalyzeLeakingStackDefault(struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pStack) CPPUTILS_NOEXCEPT;

static inline void PR_INV_INCREMENT_PTR(CinternalTlsData a_tlsKey) CPPUTILS_NOEXCEPT {
    char* pcCurrentPtr = (char*)CinternalTlsGetSpecific(a_tlsKey);
    CinternalTlsSetSpecific(a_tlsKey, ++pcCurrentPtr);
}
static inline void PR_INV_DECREMENT_PTR(CinternalTlsData a_tlsKey) CPPUTILS_NOEXCEPT {
    char* pcCurrentPtr = (char*)CinternalTlsGetSpecific(a_tlsKey);
    CinternalTlsSetSpecific(a_tlsKey, --pcCurrentPtr);
}


static inline int AddResourceToStackItemInline(
    struct SPrInvLeakAnalyzerDataAll* CPPUTILS_ARG_NN a_pTables,
    struct SStackItem* CPPUTILS_ARG_NN a_pStackItem, void* a_resource, int a_desiredRet) CPPUTILS_NOEXCEPT
{
    size_t unHashResource;
    const CinternalDLLHashItem_t hashIterResource = CInternalDLLHashFindEx(a_pTables->hashByResource, a_resource, 0, &unHashResource);
    assert(hashIterResource==CPPUTILS_NULL);
    if (!CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByResource, a_pStackItem, a_resource, 0, unHashResource)) {
        return INLINE_RETURN_EXIT;
    }
    ++(a_pStackItem->countForThisStack);
    return a_desiredRet;
}


static inline void ProgsInvestLeakAnalyzeRemoveResourceInline(
    struct SPrInvLeakAnalyzerDataAll* CPPUTILS_ARG_NN a_pTables, void* a_resource) CPPUTILS_NOEXCEPT
{
    size_t unHashResource;
    const CinternalDLLHashItem_t hashIterResource = CInternalDLLHashFindEx(a_pTables->hashByResource, a_resource, 0, &unHashResource);

    if (hashIterResource) {
        struct SStackItem* const pStackItem = TABLE_ITEM_TO_DATA(hashIterResource);
        CInternalDLLHashRemoveDataEx(a_pTables->hashByResource,hashIterResource);
        if ((--(pStackItem->countForThisStack)) == 0) {
            CInternalDLLHashRemoveDataEx(a_pTables->hashByStack, pStackItem->hashIterStack);
            (*a_pTables->m_free)(pStackItem);
        }
    }  //  if (hashIterResource) {
}


static inline int ProgsInvestLeakAnalyzeAddResourceNoLockInline(
    struct SPrInvLeakAnalyzerDataAll* CPPUTILS_ARG_NN a_pTables, void* a_resource,
    struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pCurStack) CPPUTILS_NOEXCEPT
{
    struct SStackItem* pStackItem;
    size_t unHashStack;
    const CinternalDLLHashItem_t hashIterStack = CInternalDLLHashFindEx(a_pTables->hashByStack, a_pCurStack, 0, &unHashStack);

    if (hashIterStack) {
        int nRetFromAddResource;

        pStackItem = TABLE_ITEM_TO_DATA(hashIterStack);
        //return AddResourceToStackItemInline(a_pTables, pStackItem, a_resource, INLINE_RETURN_FREE_STACK);


        nRetFromAddResource = AddResourceToStackItemInline(a_pTables, pStackItem, a_resource, INLINE_RETURN_FREE_STACK);
        if (nRetFromAddResource != INLINE_RETURN_FREE_STACK) {
            return nRetFromAddResource;
        }
        if ((pStackItem->countForThisStack) > (a_pTables->publ.countForThisStackMax)) {
            return INLINE_RETURN_CALL_STACK_ANALYZE;
        }
        return INLINE_RETURN_FREE_STACK;

    }
    
    pStackItem = (struct SStackItem*)(*a_pTables->m_malloc)(sizeof(struct SStackItem));
    if (!pStackItem) {
        CInternalLogError("Low memory!");
        return INLINE_RETURN_EXIT;
    }
    pStackItem->countForThisStack = 0;
    pStackItem->m_pStack = a_pCurStack;
    pStackItem->hashIterStack = CInternalDLLHashAddDataWithKnownHash(a_pTables->hashByStack, pStackItem, a_pCurStack, 0, unHashStack);
    if (!(pStackItem->hashIterStack)) {
        CInternalLogError("Low memory!");
        return INLINE_RETURN_EXIT;
    }
    return AddResourceToStackItemInline(a_pTables, pStackItem, a_resource, INLINE_RETURN_KEEP_STACK);
}


PRINV_LEAKA_EXPORT struct SPrInvLeakAnalyzerData*
ProgsInvestLeakAnalyzeInitialize(
    time_t a_deltaTimeToStartAnalyze, TypePrInvLeakAnalyzerAnalyzeLeakingStack a_stackAnalyzer,
    TypePrInvLeakAnalyzerMalloc a_malloc, TypePrInvLeakAnalyzerFree a_free) CPPUTILS_NOEXCEPT
{
    int nRet;
    const TypePrInvLeakAnalyzerMalloc aMalloc = a_malloc ? a_malloc : (&malloc);
    struct SPrInvLeakAnalyzerDataAll* const pTables = (struct SPrInvLeakAnalyzerDataAll*)(*aMalloc)(sizeof(struct SPrInvLeakAnalyzerDataAll));
    if(!pTables){
        return CPPUTILS_NULL;
    }

    pTables->m_stackAnalyzer = a_stackAnalyzer ? a_stackAnalyzer : (&PrInvLeakAnalyzerAnalyzeLeakingStackDefault);
    pTables->m_malloc = aMalloc;
    pTables->m_free = a_free ? a_free : (&free);

    nRet = cinternal_lw_recursive_mutex_create(&(pTables->mutexForHashes));
    if(nRet){
        (*pTables->m_free)(pTables);
        return CPPUTILS_NULL;
    }

    pTables->hashByResource = CInternalDLLHashCreateExSmlInt(MEM_LEAK_ANALYZE_HASH_BY_ADR_BASKETS, pTables->m_malloc, pTables->m_free);
    if(!(pTables->hashByResource)){
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        (*pTables->m_free)(pTables);
        return CPPUTILS_NULL;
    }

    pTables->hashByStack = CInternalDLLHashCreateExAny(MEM_LEAK_ANALYZE_HASH_BY_STACK_BASKETS,&HashByStackHasher,&IsTheSameStack,
                                                         &StoreStackHashKey,&UnstoreStackHashKey, pTables->m_malloc, pTables->m_free);
    if(!(pTables->hashByStack)){
        CInternalDLLHashDestroy(pTables->hashByResource);
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        (*pTables->m_free)(pTables);
        return CPPUTILS_NULL;
    }

    nRet = CinternalTlsAlloc(&(pTables->ignoreForThisThreadTlsKey),&ClearThreadLocalKeyStatic);
    if(nRet){
        CInternalDLLHashDestroy(pTables->hashByStack);
        CInternalDLLHashDestroy(pTables->hashByResource);
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        (*pTables->m_free)(pTables);
        return CPPUTILS_NULL;
    }

    pTables->publ.pUserData = CPPUTILS_NULL;
    pTables->publ.countForThisStackMax = PR_INVEST_LEAK_ANALYZE_DEF_MAX_IN_STACK;
    pTables->publ.bStopAnalyzing = false;
    pTables->publ.keepLeakingStacks = false;
    pTables->bInitializationTimeNotPassed = true;
    pTables->unMaxValue = 0;
    pTables->initTimeSec = time(&(pTables->initTimeSec));
    pTables->deltaTimeToStartAnalyze = a_deltaTimeToStartAnalyze;
    CPPUTILS_STATIC_CAST(void, pTables->publ.reserved01);
    CPPUTILS_STATIC_CAST(void, pTables->reserved01);

    return &(pTables->publ);
}


PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeCleanup(struct SPrInvLeakAnalyzerData* a_pTables) CPPUTILS_NOEXCEPT
{
    if (a_pTables) {
        struct SPrInvLeakAnalyzerDataAll* const pTables = PUBL_TABLE_TO_ALL_TABLE(a_pTables);
        CinternalTlsDelete(pTables->ignoreForThisThreadTlsKey);
        CInternalDLLHashDestroyEx(pTables->hashByResource, &HashByAddressDeallocator);
        CInternalDLLHashDestroyEx(pTables->hashByStack, &HashByStackDeallocator);
        cinternal_lw_recursive_mutex_destroy(&(pTables->mutexForHashes));
        (*pTables->m_free)(a_pTables);
    }
}


PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeAddResource(
    struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, void* a_resource,
    int a_goBackInTheStackCalc) CPPUTILS_NOEXCEPT
{
    int nReturnFromInline;
    struct StackInvestBacktrace* pCurStack;
    struct SPrInvLeakAnalyzerDataAll* const pTables = PUBL_TABLE_TO_ALL_TABLE(a_pTables);

    if ((pTables->bInitializationTimeNotPassed)||(pTables->publ.bStopAnalyzing)) {
        time_t currentTime;
        currentTime = time(&currentTime);
        if (currentTime < (pTables->initTimeSec + pTables->deltaTimeToStartAnalyze)) {
            return;
        }
        pTables->bInitializationTimeNotPassed = false;
    }

    if (CinternalTlsGetSpecific(pTables->ignoreForThisThreadTlsKey)) {
        return;
    }

    PR_INV_INCREMENT_PTR(pTables->ignoreForThisThreadTlsKey);

    pCurStack = StackInvestInitBacktraceDataForCurrentStack(++a_goBackInTheStackCalc);
    if (!pCurStack) {
        PR_INV_DECREMENT_PTR(pTables->ignoreForThisThreadTlsKey);
        exit(1);
    }

    cinternal_lw_recursive_mutex_lock(&(pTables->mutexForHashes));
    nReturnFromInline = ProgsInvestLeakAnalyzeAddResourceNoLockInline(pTables, a_resource, pCurStack);
    cinternal_lw_recursive_mutex_unlock(&(pTables->mutexForHashes));

    switch (nReturnFromInline) {
    case INLINE_RETURN_CALL_STACK_ANALYZE:
        pTables->publ.bStopAnalyzing = true;
        (*pTables->m_stackAnalyzer)(a_pTables, pCurStack);
        CPPUTILS_FALLTHROUGH
    case INLINE_RETURN_FREE_STACK:
        StackInvestFreeBacktraceData(pCurStack);
        break;
    case INLINE_RETURN_EXIT:
        StackInvestFreeBacktraceData(pCurStack);
        PR_INV_DECREMENT_PTR(pTables->ignoreForThisThreadTlsKey);
        exit(1);
        break;
    default:
        break;
    }

    PR_INV_DECREMENT_PTR(pTables->ignoreForThisThreadTlsKey);
}


PRINV_LEAKA_EXPORT void ProgsInvestLeakAnalyzeRemoveResource(
    struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, void* a_resource) CPPUTILS_NOEXCEPT
{
    struct SPrInvLeakAnalyzerDataAll* const pTables = PUBL_TABLE_TO_ALL_TABLE(a_pTables);
    cinternal_lw_recursive_mutex_lock(&(pTables->mutexForHashes));
    ProgsInvestLeakAnalyzeRemoveResourceInline(pTables, a_resource);
    cinternal_lw_recursive_mutex_unlock(&(pTables->mutexForHashes));
}


/*////////////////////////////////////////////////////////////////////////////////////////*/


static size_t HashByStackHasher(const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    (void)a_keySize;
    struct StackInvestBacktrace* const pStack = (struct StackInvestBacktrace*)a_key;
    return StackInvestHashOfTheStack(pStack);
}


static bool IsTheSameStack(const void* a_key1, size_t a_keySize1, const void* a_key2, size_t a_keySize2) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_keySize1);
    CPPUTILS_STATIC_CAST(void, a_keySize2);
    const struct StackInvestBacktrace* const pStack1 = (const struct StackInvestBacktrace*)a_key1;
    const struct StackInvestBacktrace* const pStack2 = (const struct StackInvestBacktrace*)a_key2;
    return StackInvestIsTheSameStack(pStack1, pStack2);
}


static bool StoreStackHashKey(TypeCinternalAllocator a_allocator, void** a_pKeyStore, size_t* a_pKeySizeStore, const void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_allocator);
    CPPUTILS_STATIC_CAST(void, a_keySize);
    CPPUTILS_STATIC_CAST(void, a_pKeySizeStore);
    *a_pKeyStore = CPPUTILS_CONST_CAST(void*, a_key);
    return true;
}


static void UnstoreStackHashKey(TypeCinternalDeallocator a_deallocator, void* a_key, size_t a_keySize) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_deallocator);
    CPPUTILS_STATIC_CAST(void, a_key);
    CPPUTILS_STATIC_CAST(void, a_keySize);
}


static void ClearThreadLocalKeyStatic(void* a_pData) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_pData);
}


static void HashByStackDeallocator(void* a_pData) CPPUTILS_NOEXCEPT
{
    //struct SPrInvLeakAnlzStackItemPriv* const pStackItem = (struct SPrInvLeakAnlzStackItemPriv*)a_pData;
    //struct StackInvestBacktrace* const pStack = pStackItem->pbl.pStack;
    //StackInvestFreeBacktraceData(pStack);
    //AllocFreeHookCLibFree(pStackItem);
    (void)a_pData;
}


static void HashByAddressDeallocator(void* a_pData) CPPUTILS_NOEXCEPT
{
    //struct SPrInvLeakAnlzMem* const pAddrItem = (struct SPrInvLeakAnlzMem*)a_pData;
    //AllocFreeHookCLibFree(pAddrItem);
    (void)a_pData;
}


static void PrInvLeakAnalyzerAnalyzeLeakingStackDefault(struct SPrInvLeakAnalyzerData* CPPUTILS_ARG_NN a_pTables, struct StackInvestBacktrace* CPPUTILS_ARG_NN a_pStack) CPPUTILS_NOEXCEPT
{
    CPPUTILS_STATIC_CAST(void, a_pTables);
    CPPUTILS_STATIC_CAST(void, a_pStack);
}


CPPUTILS_END_C
