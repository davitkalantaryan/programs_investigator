//
// file:			alloc_free_hook.h
// path:			include/allocfreehook/alloc_free_hook.h
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_ALLOC_FREE_HOOK_H
#define ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_ALLOC_FREE_HOOK_H

#include <allocfreehook/export_symbols.h>
#include <stddef.h>


CPPUTILS_BEGIN_C


typedef void* (*TypeAllocFreeHookMalloc)(size_t);
typedef void* (*TypeAllocFreeHookCalloc)(size_t, size_t);
typedef void* (*TypeAllocFreeHookRealloc)(void*, size_t);
typedef void  (*TypeAllocFreeHookFree)(void*);


ALLOCFREEHOOK_EXPORT void AllocFreeHookSetMallocFnc(TypeAllocFreeHookMalloc a_malloc);
ALLOCFREEHOOK_EXPORT void AllocFreeHookSetCallocFnc(TypeAllocFreeHookCalloc a_calloc);
ALLOCFREEHOOK_EXPORT void AllocFreeHookSetReallocFnc(TypeAllocFreeHookRealloc a_realloc);
ALLOCFREEHOOK_EXPORT void AllocFreeHookSetFreeFnc(TypeAllocFreeHookFree a_free);


ALLOCFREEHOOK_EXPORT void* AllocFreeHookCLibMalloc(size_t a_size);
ALLOCFREEHOOK_EXPORT void* AllocFreeHookCLibCalloc(size_t a_nmemb, size_t a_size);
ALLOCFREEHOOK_EXPORT void* AllocFreeHookCLibRealloc(void* a_ptr, size_t a_size);
ALLOCFREEHOOK_EXPORT void  AllocFreeHookCLibFree(void* a_ptr);


CPPUTILS_END_C


#endif  // #ifndef ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_ALLOC_FREE_HOOK_H
