//
// file:			alloc_free_hook_core_alloc_free_hook_windows.cpp
// path:			src/core/alloc_free_hook/windows/alloc_free_hook_core_alloc_free_hook_windows.cpp
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <cinternal/internal_header.h>

#ifdef _WIN32


#include <allocfreehook/alloc_free_hook.h>
#include <cinternal/replace_function.h>
#include <stdlib.h>


CPPUTILS_BEGIN_C


static int s_nIsInited = 0;

static TypeAllocFreeHookMalloc  s_malloc_c_lib	= &malloc;
static TypeAllocFreeHookCalloc  s_calloc_c_lib	= &calloc;
static TypeAllocFreeHookRealloc	s_realloc_c_lib = &realloc;
static TypeAllocFreeHookFree	s_free_c_lib	= &free;

static TypeAllocFreeHookMalloc  g_malloc;
static TypeAllocFreeHookCalloc  g_calloc;
static TypeAllocFreeHookRealloc g_realloc;
static TypeAllocFreeHookFree    g_free;


static int alloc_free_hook_cleanup(void) CPPUTILS_NOEXCEPT;


static void* AllocFreeHookMalloc(size_t a_size)
{
	return (*g_malloc)(a_size);
}


static void* AllocFreeHookCalloc(size_t a_nmemb, size_t a_size)
{
	return (*g_calloc)(a_nmemb,a_size);
}


static void* AllocFreeHookRealloc(void* a_ptr, size_t a_size)
{
	return (*g_realloc)(a_ptr, a_size);
}


static void AllocFreeHookFree(void* a_ptr)
{
	(*g_free)(a_ptr);
}


static inline void alloc_free_hook_initialize_inline(void) {
	if (!s_nIsInited) {
		struct SCInternalReplaceFunctionData vReplaceData[4];

		s_malloc_c_lib	= &malloc;
		s_calloc_c_lib	= &calloc;
		s_realloc_c_lib	= &realloc;
		s_free_c_lib	= &free;

		g_malloc	= s_malloc_c_lib;
		g_calloc	= s_calloc_c_lib;
		g_realloc	= s_realloc_c_lib;
		g_free		= s_free_c_lib;

		vReplaceData[0].funcname = "malloc";
		vReplaceData[0].newFuncAddress = &AllocFreeHookMalloc;

		vReplaceData[1].funcname = "calloc";
		vReplaceData[1].newFuncAddress = &AllocFreeHookCalloc;

		vReplaceData[2].funcname = "realloc";
		vReplaceData[2].newFuncAddress = &AllocFreeHookRealloc;


		vReplaceData[3].funcname = "free";
		vReplaceData[3].newFuncAddress = &AllocFreeHookFree;

		CInternalReplaceFunctions(4, vReplaceData);

		_onexit(&alloc_free_hook_cleanup);

		s_nIsInited = 1;
	}
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ALLOCFREEHOOK_EXPORT void* MemoryHandlerCLibMalloc(size_t a_size)
{
	return (*s_malloc_c_lib)(a_size);
}


ALLOCFREEHOOK_EXPORT void* MemoryHandlerCLibCalloc(size_t a_nmemb, size_t a_size)
{
	return (*s_calloc_c_lib)(a_nmemb, a_size);
}


ALLOCFREEHOOK_EXPORT void* MemoryHandlerCLibRealloc(void* a_ptr, size_t a_size)
{
	return (*s_realloc_c_lib)(a_ptr, a_size);
}


ALLOCFREEHOOK_EXPORT void MemoryHandlerCLibFree(void* a_ptr)
{
	(*s_free_c_lib)(a_ptr);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ALLOCFREEHOOK_EXPORT void AllocFreeHookSetMallocFnc(TypeAllocFreeHookMalloc a_malloc)
{
	alloc_free_hook_initialize_inline();
	g_malloc = a_malloc;
}


ALLOCFREEHOOK_EXPORT void AllocFreeHookSetCallocFnc(TypeAllocFreeHookCalloc a_calloc)
{
	alloc_free_hook_initialize_inline();
	g_calloc = a_calloc;
}


ALLOCFREEHOOK_EXPORT void AllocFreeHookSetReallocFnc(TypeAllocFreeHookRealloc a_realloc)
{
	alloc_free_hook_initialize_inline();
	g_realloc = a_realloc;
}


ALLOCFREEHOOK_EXPORT void AllocFreeHookSetFreeFnc(TypeAllocFreeHookFree a_free)
{
	alloc_free_hook_initialize_inline();
	g_free = a_free;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static int alloc_free_hook_cleanup(void) CPPUTILS_NOEXCEPT
{
	if (s_nIsInited) {
		struct SCInternalReplaceFunctionData vReplaceData[4];

		vReplaceData[0].funcname = "malloc";
		vReplaceData[0].newFuncAddress = s_malloc_c_lib;

		vReplaceData[1].funcname = "calloc";
		vReplaceData[1].newFuncAddress = s_calloc_c_lib;

		vReplaceData[2].funcname = "realloc";
		vReplaceData[2].newFuncAddress = s_realloc_c_lib;


		vReplaceData[3].funcname = "free";
		vReplaceData[3].newFuncAddress = s_free_c_lib;

		CInternalReplaceFunctions(4, vReplaceData);
		s_nIsInited = 0;
	}

	return 0;
}


CPPUTILS_CODE_INITIALIZER(alloc_free_hook_initialize) {
	alloc_free_hook_initialize_inline();
}


CPPUTILS_END_C



#endif  //  #ifdef _WIN32
