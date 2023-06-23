//
// file:			main_any_quick_test.cpp
// path:			src/tests/main_any_quick_test.cpp
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <cinternal/replace_function.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32

typedef void* (*TypeMallocFnc)(size_t);
typedef void (*TypeFreeFnc)(void*);

static TypeMallocFnc    s_originalMalloc = &malloc;
static TypeFreeFnc    s_originalFree = &free;


static void* MyMalloc(size_t a_size)
{
	void* pRet = (*s_originalMalloc)(a_size);
	return pRet;
}


//static int __declspec(thread) s_a1;
//static int thread_local s_a2;

#ifdef CPPUTILS_CPP_11_DEFINED
#endif


static void MyFree(void* a_ptr)
{
	(*s_originalFree)(a_ptr);
}


#endif  //  #ifdef _WIN32


int main(void)
{
	void* pMem;
	
#ifdef _WIN32
	struct SCInternalReplaceFunctionData aReplaceData[2];

	aReplaceData[0].funcname = "malloc";
	aReplaceData[0].newFuncAddress = &MyMalloc;

	aReplaceData[1].funcname = "free";
	aReplaceData[1].newFuncAddress = &MyFree;

	CInternalReplaceFunctions(2, aReplaceData);
#endif

	pMem = malloc(100);
	printf("pMem = %p\n", pMem);
	free(pMem);

	return 0;
}
