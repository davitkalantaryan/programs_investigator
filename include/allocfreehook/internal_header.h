//
// file:			internal_header.h
// path:			include/cinternal/internal_header.h
// created on:		2022 Jun 04
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef CINTERNAL_INCLUDE_CINTERNAL_INTERNAL_HEADER_H
#define CINTERNAL_INCLUDE_CINTERNAL_INTERNAL_HEADER_H

#include <stddef.h>

#define cpputils_alloca	alloca

#ifdef _MSC_VER

	#if defined(_WIN64) || defined(_M_ARM)
		#define CPPUTILS_FNAME_PREFIX ""
		#define CPPUTILS_DS_FNAME_POSTFIX
		#define CPPUTILS_SEC_CH_FNC_NAME	"__security_check_cookie"
	#else
		#define CPPUTILS_FNAME_PREFIX "_"
		#define CPPUTILS_DS_FNAME_POSTFIX	"@12"
		#define CPPUTILS_SEC_CH_FNC_NAME	"@__security_check_cookie@4"
	#endif

    #define CPPUTILS_C_CODE_INITIALIZER_RAW(_sect,f) \
        __pragma(section(_sect,read)) \
        static void f(void); \
        __declspec(allocate(_sect)) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" CPPUTILS_FNAME_PREFIX #f "_")) \
        static void f(void)

    #define CPPUTILS_C_CODE_INITIALIZER(f)  CPPUTILS_C_CODE_INITIALIZER_RAW(".CRT$XCU",f)

	#undef cpputils_alloca
	#define cpputils_alloca	_alloca
	#define CPPUTILS_UNREACHABLE_CODE(_code)
	//#if _MSC_FULL_VER
	#if (_MSC_VER>1900) // 1900 is VS2015
		#pragma warning (disable:5045)
		#pragma warning (disable:5247)
		#pragma warning (disable:5248)
	#endif
	// assignment within conditional expression (https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4706?view=msvc-160)
	#pragma warning (disable:4706) 
	// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4711?view=msvc-160
	#pragma warning (disable:4711)
	// Function not inlined althou defined to be inline. 
	// this can happen also with library functions, so this should be disabled
	// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4711?view=msvc-160
	#pragma warning (disable:4710)
	#define CPPUTILS_BEFORE_CPP_17_FALL_THR
    #define CPPUTILS_DLL_PUBLIC		__declspec(dllexport)
    #define CPPUTILS_DLL_PRIVATE
    #define CPPUTILS_IMPORT_FROM_DLL	__declspec(dllimport)
	#define CPPUTILS_THREAD_LOCAL		__declspec(thread)
	#if !defined(_WIN64) && !defined(_M_ARM64)
		#define CPPUTLS_32_BIT
	#endif
#elif defined(__GNUC__) || defined(__clang__) || defined(LINUX_GCC)

	#define CPPUTILS_GCC_FAMILY		1
	#define CPPUTILS_C_CODE_INITIALIZER(f)	static void __attribute__ ((__constructor__)) f(void)

    #define CPPUTILS_MAY_ALIAS  __attribute__ ((__may_alias__))
	#define CPPUTILS_UNREACHABLE_CODE(_code)	_code ;
	#if __GNUC__>=7
		#define CPPUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
	#elif defined(__has_attribute)
		#if __has_attribute (fallthrough)
			#define CPPUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
		#else
			#define CPPUTILS_BEFORE_CPP_17_FALL_THR		/* FALLTHRU */
		#endif
	#else
		#define CPPUTILS_BEFORE_CPP_17_FALL_THR		/* FALLTHRU */
	#endif  // #if __GNUC__>=7
    //#define CPPUTILS_DLL_PUBLIC		__attribute__((visibility("default")))
    #define CPPUTILS_DLL_PUBLIC
    #define CPPUTILS_DLL_PRIVATE		__attribute__((visibility("hidden")))
    #define CPPUTILS_IMPORT_FROM_DLL
	#define CPPUTILS_THREAD_LOCAL		__thread
#elif defined(__CYGWIN__)

	#define CPPUTILS_GCC_FAMILY		1
	#define CPPUTILS_C_CODE_INITIALIZER(f)	static void __attribute__ ((__constructor__)) f(void)

	#define CPPUTILS_UNREACHABLE_CODE(_code)	_code ;
	#define CPPUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
    #define CPPUTILS_DLL_PUBLIC		__attribute__((dllexport))
    #define CPPUTILS_DLL_PRIVATE
    #define CPPUTILS_IMPORT_FROM_DLL	__attribute__((dllimport))
#elif defined(__MINGW64__) || defined(__MINGW32__)

	#define CPPUTILS_GCC_FAMILY		1
	#define CPPUTILS_C_CODE_INITIALIZER(f)	static void __attribute__ ((__constructor__)) f(void)

	#define CPPUTILS_UNREACHABLE_CODE(_code)	_code ;
	#define CPPUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
    #define CPPUTILS_DLL_PUBLIC		_declspec(dllexport)
    #define CPPUTILS_DLL_PRIVATE
    #define CPPUTILS_IMPORT_FROM_DLL	_declspec(dllimport)
#elif defined(__SUNPRO_CC)

	#define CPPUTILS_C_CODE_INITIALIZER(f)	static void f(void); _Pragma(init  (f)) static void f(void)

	#define CPPUTILS_UNREACHABLE_CODE(_code)	_code ;
	// #define CPPUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ; // ???
	#define CPPUTILS_BEFORE_CPP_17_FALL_THR
    #define CPPUTILS_DLL_PUBLIC
    #define CPPUTILS_DLL_PRIVATE		__hidden
    #define CPPUTILS_IMPORT_FROM_DLL
#endif  // #ifdef _MSC_VER

#if defined(_MSC_VER) && defined(_MSVC_LANG)
	#if (_MSVC_LANG>=201100L)
		#define CPPUTILS_CPP_11_DEFINED		1
	#endif
	#if (_MSVC_LANG>=201400L)
		#define CPPUTILS_CPP_14_DEFINED		1
	#endif
	#if (_MSVC_LANG>=201700L)
		#define CPPUTILS_CPP_17_DEFINED		1
	#endif
	#if (_MSVC_LANG>=202000L)
		#define CPPUTILS_CPP_20_DEFINED		1
	#endif
	#if (_MSVC_LANG>=202300L)
		#define CPPUTILS_CPP_23_DEFINED		1
	#endif
#elif defined(__cplusplus)  // #if defined(_MSC_VER) && defined(_MSVC_LANG)
	#if (__cplusplus>=201100L)
		#define CPPUTILS_CPP_11_DEFINED		1
	#endif
	#if (__cplusplus>=201400L)
		#define CPPUTILS_CPP_14_DEFINED		1
	#endif
	#if (__cplusplus>=201700L)
		#define CPPUTILS_CPP_17_DEFINED		1
	#endif
	#if (__cplusplus>=202000L)
		#define CPPUTILS_CPP_20_DEFINED		1
	#endif
	#if (__cplusplus>=202300L)
		#define CPPUTILS_CPP_23_DEFINED		1
	#endif
#endif // #if defined(_MSC_VER) && defined(_MSVC_LANG)


//#undef CPPUTILS_CPP_11_DEFINED


//#define CPPUTILS_RESTRICT   restrict
#define CPPUTILS_RESTRICT

#ifndef CPPUTILS_MAY_ALIAS
#define CPPUTILS_MAY_ALIAS
#endif

#ifdef __cplusplus
#define CPPUTILS_STATIC_CAST(_type,_data)		static_cast<_type>(_data)
#define CPPUTILS_REINTERPRET_CAST(_type,_data)	reinterpret_cast<_type>(_data)
#define CPPUTILS_CONST_CAST(_type,_data)		const_cast<_type>(_data)
#define CPPUTILS_DYNAMIC_CAST(_type,_data)		dynamic_cast<_type>(_data)
#define CPPUTILS_GLOBAL	   ::
#define CPPUTILS_BEGIN_C   extern "C" {
#define CPPUTILS_END_C     }
#define CPPUTILS_EXTERN_C  extern "C"
#else
#define CPPUTILS_STATIC_CAST(_type,_data)		((_type)(_data))
#define CPPUTILS_REINTERPRET_CAST(_type,_data)	((_type)(_data))
#define CPPUTILS_CONST_CAST(_type,_data)		((_type)(_data))
#define CPPUTILS_DYNAMIC_CAST(_type,_data)		((_type)(_data))
#define CPPUTILS_GLOBAL
#define CPPUTILS_BEGIN_C
#define CPPUTILS_END_C
#define CPPUTILS_EXTERN_C
#endif


#ifdef CPPUTILS_CPP_17_DEFINED
#define CPPUTILS_FALLTHROUGH	[[fallthrough]] ;
#else
#define CPPUTILS_FALLTHROUGH	CPPUTILS_BEFORE_CPP_17_FALL_THR 
//#define CPPUTILS_FALLTHROUGH	
#endif

//#undef CPPUTILS_CPP_11_DEFINED
//#undef CPPUTILS_CPP_14_DEFINED

#ifdef CPPUTILS_CPP_11_DEFINED
#define CPPUTILS_STD_MOVE_DEFINED   1
#define CPPUTILS_NOEXCEPT           noexcept
#define CPPUTILS_NULL               nullptr
#define CPPUTILS_OVERRIDE           override
//#define CPPUTILS_CONSTEXPR          constexpr
//#define CPPUTILS_CONSTEXPR_CONS     constexpr
#define CPPUTILS_CONSTEXPR          constexpr
#define CPPUTILS_CONSTEXPR_FNC          constexpr
#define CPPUTILS_CONSTEXPR_EQ(_exp)	= (_exp)
#define CPPUTILS_DELETE             =delete;
#define CPPUTILS_CONSTEXPR_STACK_ARRAY(_type,_name,_count)	_type _name[_count]
#else
#define CPPUTILS_NOEXCEPT	throw()
#define CPPUTILS_NULL		NULL 
#define CPPUTILS_OVERRIDE
#define CPPUTILS_CONSTEXPR	const
#define CPPUTILS_CONSTEXPR_FNC
#define CPPUTILS_CONSTEXPR_EQ(_exp)
#define CPPUTILS_DELETE   {}
#define CPPUTILS_CONSTEXPR_STACK_ARRAY(_type,_name,_count)	_type * _name = CPPUTILS_STATIC_CAST(  _type *,cpputils_alloca((_count)*sizeof(_type))  )
#endif

#ifndef __cplusplus
#undef CPPUTILS_NOEXCEPT
#define CPPUTILS_NOEXCEPT
#endif

#ifdef CPPUTILS_CPP_14_DEFINED
#define CPPUTILS_CONSTEXPR_FNC_CPP14     constexpr
#else
#define CPPUTILS_CONSTEXPR_FNC_CPP14
#endif

#ifdef __cplusplus
#define CPPUTILS_REGISTER
#else
#define CPPUTILS_REGISTER	register
#endif


//#define CPPUTILS_IS_LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)

#if defined(_DEBUG) || defined(CPPUTILS_DEBUG)
// we have debug compilation
#else
// we have release
#ifndef NDEBUG
// let's define NDEBUG (No DEBUG)
#define NDEBUG
#endif
#endif

#ifdef NDEBUG
#define CPPUTILS_DO_DEBUG_EXP(_exp)
#define CPPUTILS_SAFE_CAST(_type,_val)	static_cast<_type>(_val)
#else
#define CPPUTILS_DO_DEBUG_EXP(_exp)              _exp ;
#define CPPUTILS_SAFE_CAST(_type,_val)	dynamic_cast<_type>(_val)
#endif


#define CPPUTILS_NO_NULL
#define CPPUTILS_ARG_NO_NULL

#define CPPUTILS_STRINGIFY(_x)                CPPUTILS_STRINGIFY_PRIV_RAW(_x)
#define CPPUTILS_STRINGIFY_PRIV_RAW(_x)		#_x


#if defined(__EMSCRIPTEN__) || defined(EMSCRIPTEN) || defined(CPPUTILS_WASM)
#define CPPUTILS_EMSCRIPTEN_IS_USED
#endif
//#define CPPUTILS_EMSCRIPTEN_IS_USED

#ifdef CPPUTILS_EMSCRIPTEN_IS_USED
#ifdef CPPUTILS_DLL_PUBLIC
#undef CPPUTILS_DLL_PUBLIC
#endif
#include <emscripten/emscripten.h>
#define CPPUTILS_DLL_PUBLIC     EMSCRIPTEN_KEEPALIVE
#endif

// todo: make better multithreading decision
#if !defined(CPPUTILS_EMSCRIPTEN_IS_USED) && !defined(CPPUTILS_FORCE_SINGLE_THREADED)
#define CPPUTILS_MULTITHREADED
#endif

// todo: make better possible no file dfecision
#ifdef CPPUTILS_EMSCRIPTEN_IS_USED
#define CPPUTILS_POSSIBLE_NO_PERS_FILE
#endif

#ifdef CPPUTILS_MULTITHREADED
#define CPPUTILS_TRY_CATCH(_expression,_result)	(_expression) ;
#else
#define CPPUTILS_TRY_CATCH(_expression,_result)   try{(_expression);}catch(...){return _result;}
#endif

#define CPPUTILS_IN_OUT

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56480
#ifdef __GNUC__
#define CPPUTILS_GCC_BUG_56480
#endif

#ifdef CPPUTILS_CPP_20_DEFINED
#define CPPUTILS_NODISCARD	[[nodiscard]]
#elif defined(CPPUTILS_CPP_11_DEFINED)
#define CPPUTILS_NODISCARD	noexcept
#else
#define CPPUTILS_NODISCARD	throw()
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(LINUX_GCC)
#define CPPUTILS_LIKELY(_x)             __builtin_expect(!!(_x), 1)
#define CPPUTILS_UNLIKELY(_x)           __builtin_expect(!!(_x), 0)
#define CPPUTILS_LIKELY_VALUE(_x,_val)  __builtin_expect((_x), (_val))
#else
#define CPPUTILS_LIKELY(_x)             (_x)
#define CPPUTILS_UNLIKELY(_x)           (_x)
#define CPPUTILS_LIKELY_VALUE(_x,_val)  ((_x)==(_val))
#endif


#define CPPUTILS_ARG_NONULL


#if defined(__APPLE__) && (defined(__GNUC__)  || defined(__xlC__)  || defined(__xlc__))
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_MAC) && TARGET_OS_MAC
        #define CPPUTILS_OS_DARWIN
        #define CPPUTILS_OS_BSD4
        #ifdef LP64
            #define CPPUTILS_OS_DARWIN64
        #else
            #define CPPUTILS_OS_DARWIN32
        #endif
        #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
            #define CPPUTILS_PLATFORM_UIKIT
            #if defined(TARGET_OS_WATCH) && TARGET_OS_WATCH
                #define CPPUTILS_OS_WATCHOS
            #elif defined(TARGET_OS_TV) && TARGET_OS_TV
                #define CPPUTILS_OS_TVOS
            #else
                // TARGET_OS_IOS is only available in newer SDKs,
                // so assume any other iOS-based platform is iOS for now
                #define CPPUTILS_OS_IOS
            #endif
        #else
            // TARGET_OS_OSX is only available in newer SDKs,
            // so assume any non iOS-based platform is macOS for now
            #define CPPUTILS_OS_MACOS
            #define CPPUTILS_OS_MAC 1
        #endif
    #else
        #error "CPPUTILS has not been ported to this Apple platform"
    #endif
#endif  //  #if defined(__APPLE__) && (defined(__GNUC__)  defined(__xlC__)  defined(__xlc__))


#if defined(_CPPUNWIND) || !defined(_MSC_VER)
#define CPPUTILS_TRY		try
#define CPPUTILS_CATCH()	catch(...)
#else
#define CPPUTILS_TRY		__try
#define CPPUTILS_CATCH()	__except (EXCEPTION_EXECUTE_HANDLER)
#endif

#define CPPUTILS_STRINGIZER(_arg)		#_arg
#define CPPUTILS_STRVAL(_var)			CPPUTILS_STRINGIZER(_var)


#define CPPUTILS_NAME_WITH_NUM_RAWEX(_var,_num)		_var ## _num
#define CPPUTILS_NAME_WITH_NUM_RAW(_var,_num)		CPPUTILS_NAME_WITH_NUM_RAWEX(_var,_num)
#define CPPUTILS_NAME_WITH_CNTR(_var)				CPPUTILS_NAME_WITH_NUM_RAW(_var,__COUNTER__)
#define CPPUTILS_NAME_WITH_LINE(_var)				CPPUTILS_NAME_WITH_NUM_RAW(_var,__LINE__)


#ifdef _MSC_VER

#define CPPUTILS_INSERT_COMMENT_TO_BIN_RAW_RAW(_sectionVar,_sectionName,_comment)				\
	__pragma(section(_sectionName,read))														\
	CPPUTILS_EXTERN_C __declspec(allocate(_sectionName)) const char _sectionVar[] = _comment;	\
	__pragma(comment(linker, "/include:" CPPUTILS_FNAME_PREFIX CPPUTILS_STRVAL(_sectionVar)))

#elif defined(__APPLE__)

#define CPPUTILS_INSERT_COMMENT_TO_BIN_RAW_RAW(_sectionVar,_sectionName,_comment)				\
    const char __attribute__((section("__DATA," _sectionName),used)) _sectionVar[] = _comment;

#elif defined(CPPUTILS_GCC_FAMILY)

#define CPPUTILS_INSERT_COMMENT_TO_BIN_RAW_RAW(_sectionVar,_sectionName,_comment)				\
    const char _sectionVar[] __attribute__((section(_sectionName))) = _comment;

#else

#define CPPUTILS_INSERT_COMMENT_TO_BIN_RAW_RAW(_sectionVar,_sectionName,_comment)				\
	_Pragma("GCC diagnostic push")                                                              \
    _Pragma("GCC diagnostic ignored \"-Wunused-const-variable\"")                               \
    const char _sectionVar[] = _comment;                                                        \
    _Pragma("GCC diagnostic pop")

#endif  // #ifdef _MSC_VER


#ifdef __cplusplus
#define CPPUTILS_CODE_INITIALIZER(_func)					\
	static void _func(void);								\
	class CPPUTILS_DLL_PRIVATE ___IniterClass_ ## _func{	\
		public:												\
			___IniterClass_ ## _func (){					\
				_func();									\
			}												\
	}static ___initerMember_ ## _func;						\
	void _func(void)
#else
#define CPPUTILS_CODE_INITIALIZER			CPPUTILS_C_CODE_INITIALIZER
#endif


#endif  // #ifndef CINTERNAL_INCLUDE_CINTERNAL_INTERNAL_HEADER_H
