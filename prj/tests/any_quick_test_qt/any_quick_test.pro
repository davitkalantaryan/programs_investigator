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

LIBS += -pthread
LIBS += -ldl

DEFINES += CPPUTILS_USING_STATIC_LIB_OR_OBJECTS


SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/alloc_free_hook/*.c,true)
SOURCES	+=		\
        "$${PWD}/../../../src/tests/main_any_quick_test.cpp"

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../any_quick_test_mkfl/*.Makefile,false)
