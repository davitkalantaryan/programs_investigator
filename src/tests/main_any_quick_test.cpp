//
// file:			main_any_quick_test.cpp
// path:			src/tests/main_any_quick_test.cpp
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <cinternal/replace_function.h>
#include <stdlib.h>
#include <stdio.h>

typedef void* (*TypeMallocFnc)(size_t);
typedef void (*TypeFreeFnc)(void*);

static TypeMallocFnc    s_originalMalloc = &malloc;
static TypeFreeFnc    s_originalFree = &free;


static void* MyMalloc(size_t a_size)
{
	void* pRet = (*s_originalMalloc)(a_size);
	return pRet;
}


static void MyFree(void* a_ptr)
{
	(*s_originalFree)(a_ptr);
}

int main(void)
{
	void* pMem;
	struct SCInternalReplaceFunctionData aReplaceData[2];

	aReplaceData[0].funcname = "malloc";
	aReplaceData[0].newFuncAddress = &MyMalloc;

	aReplaceData[1].funcname = "free";
	aReplaceData[1].newFuncAddress = &MyFree;

	CInternalReplaceFunctions(2, aReplaceData);

	pMem = malloc(100);
	printf("pMem = %p\n", pMem);
	free(pMem);

	return 0;
}
