#
# file:			alloc_free_hook_test01.pro
# path:			prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro
# created on:	2021 Mar 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/flags_common.pri" )

DESTDIR     = "$${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -L$${DESTDIR}/../lib
LIBS += -lprogs_invest_basic
LIBS += -pthread
LIBS += -ldl

DEFINES += PRINV_LEAKA_COMPILING_SHARED_LIB
DEFINES += ALLOCFREEHOOK_LOAD_FROM_DLL
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS
DEFINES += STACK_INVEST_ANY_ALLOC=AllocFreeHookCLibMalloc
DEFINES += STACK_INVEST_ANY_FREE=AllocFreeHookCLibFree
DEFINES += STACK_INVEST_C_LIB_FREE_NO_CLBK=AllocFreeHookCLibFree

SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/leak_analyze/*.c*,true)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_hash_dllhash.c"
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_lw_mutex_recursive.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_unix.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_common.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_addr_to_details_unix_dwarf.c"
SOURCES	+=		\
        "$${PWD}/../../../src/tests/main_leak_analyze_test.cpp"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../any_quick_test_mkfl/*.Makefile,false)
