#
# file:		liballoc_free_handler.pro
# path:		prj/core/liballoc_free_handler_qt/liballoc_free_handler.pro
# created on:	2021 Mar 24
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = lib
CONFIG += dll
CONFIG -= static
TARGET = progs_invest_basic

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/flags_common.pri" )


QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -L$${DESTDIR}/../lib
LIBS += -lprogs_invest_basic
LIBS += -pthread
LIBS += -ldl

DEFINES += PRINV_LEAKA_COMPILING_SHARED_LIB
DEFINES += STACK_INVEST_USING_STATIC_LIB_OR_OBJECTS
DEFINES += STACK_INVEST_ANY_ALLOC=MemoryHandlerCLibMalloc
DEFINES += STACK_INVEST_ANY_FREE=MemoryHandlerCLibFree
DEFINES += STACK_INVEST_C_LIB_FREE_NO_CLBK=MemoryHandlerCLibFree


SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/leak_analyze/*.c*,true)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../libanalyze_leaking_mkfl/*.Makefile,false)
