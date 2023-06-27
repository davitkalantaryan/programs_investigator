//
// file:			export_symbols.h
// path:			include/allocfreehook/export_symbols.h
// created on:		2023 Mar 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_EXPORT_SYMBOLS_H
#define ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_EXPORT_SYMBOLS_H

#include <cinternal/internal_header.h>


#ifndef ALLOCFREEHOOK_EXPORT
#if defined(ALLOCFREEHOOK_COMPILING_SHARED_LIB)
#define ALLOCFREEHOOK_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(ALLOCFREEHOOK_USING_STATIC_LIB_OR_OBJECTS)
#define ALLOCFREEHOOK_EXPORT
#elif defined(ALLOCFREEHOOK_LOAD_FROM_DLL)
#define ALLOCFREEHOOK_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define ALLOCFREEHOOK_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#endif  // #ifndef ALLOCFREEHOOK_INCLUDE_ALLOCFREEHOOK_EXPORT_SYMBOLS_H
