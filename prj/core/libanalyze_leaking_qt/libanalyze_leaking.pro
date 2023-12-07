#
# file:		liballoc_free_handler.pro
# path:		prj/core/liballoc_free_handler_qt/liballoc_free_handler.pro
# created on:	2021 Mar 24
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = lib
CONFIG += dll
CONFIG -= static
TARGET = analyze_leaking

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/flags_common.pri" )


QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -L$${DESTDIR}/../lib
LIBS += -lprogs_invest_basic
LIBS += -ldwarf
LIBS += -pthread
LIBS += -ldl

DEFINES += PRINV_LEAKA_COMPILING_SHARED_LIB
DEFINES += ALLOCFREEHOOK_LOAD_FROM_DLL
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS
DEFINES += STACK_INVEST_ANY_ALLOC=AllocFreeHookCLibMalloc
DEFINES += STACK_INVEST_ANY_REALLOC=AllocFreeHookCLibRealloc
DEFINES += STACK_INVEST_ANY_FREE=AllocFreeHookCLibFree
DEFINES += STACK_INVEST_C_LIB_FREE_NO_CLBK=AllocFreeHookCLibFree


SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/leak_analyze/*.c*,true)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_hash_dllhash.c"
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_lw_mutex_recursive.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_unix.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_common.c"
SOURCES += "$${stackInvestigatorRepoRoot}/src/core/stack_investigator_backtrace_addr_to_details_unix_dwarf.c"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../libanalyze_leaking_mkfl/*.Makefile,false)
