#
# file:			alloc_free_hook_test01.pro
# path:			prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro
# created on:	2021 Mar 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = lib

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/flags_common.pri" )

DESTDIR     = "$${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/tlib"


QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -L$${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/lib
LIBS += -lprogs_invest_basic
LIBS += -pthread

DEFINES += ALLOCFREEHOOK_LOAD_FROM_DLL


#SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/alloc_free_hook/*.c,true)
SOURCES	+=		\
        "$${PWD}/../../../src/tests/entry_test_lib_to_change_alloc_free.c"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../test_lib_to_change_alloc_free_mkfl/*.Makefile,false)
