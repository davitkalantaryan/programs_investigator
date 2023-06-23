#
# file:		alloc_free_hook_test02_lib.pro
# path:		prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test02_lib.pro
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

LIBS += -pthread
LIBS += -ldl

DEFINES += ALLOCFREEHOOK_COMPILING_SHARED_LIB


SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/alloc_free_hook/*.c,true)
SOURCES	+=		\
        "$${PWD}/../../../src/tests/entry_alloc_free_hook_test02_lib.cpp"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../alloc_free_hook_test02_lib_mkfl/*.Makefile,false)
