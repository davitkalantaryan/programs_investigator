#
# file:			alloc_free_hook_test01.pro
# path:			prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro
# created on:	2021 Mar 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
#include ( "$${PWD}/../../common/common_qt/flags_common.pri" )

DESTDIR     = "$${artifactRoot}/$${SYSTEM_PATH}/$$CONFIGURATION/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -pthread
LIBS += -ldl

repoRootPath=$${PWD}/../../..

INCLUDEPATH += "$${PWD}/../../../include"
INCLUDEPATH += "$${cinternalRepoRoot}/include"
DEFINES += CPPUTILS_USING_STATIC_LIB_OR_OBJECTS


SOURCES += $$files($${repoRootPath}/src/core/alloc_free_hook/*.c,true)
SOURCES	+=		\
        "$${PWD}/../../../src/tests/main_alloc_free_hook_test01.cpp"

COMMON_HDRS	= $$files($${repoRootPath}/include/*.h,true)
COMMON_HDRSPP	= $$files($${repoRootPath}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP
