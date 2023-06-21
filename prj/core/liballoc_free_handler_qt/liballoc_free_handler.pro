#
# file:		liballoc_free_handler.pro
# path:		prj/core/liballoc_free_handler_qt/liballoc_free_handler.pro
# created on:	2021 Mar 24
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = lib
CONFIG += dll
CONFIG -= static
TARGET = alloc_free_handler

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/flags_common.pri" )


QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -pthread
LIBS += -ldl

DEFINES += ALLOCFREEHOOK_COMPILING_SHARED_LIB


SOURCES += $$files($${programsInvestigatorRepoRoot}/src/core/alloc_free_hook/*.c,true)

COMMON_HDRS	= $$files($${programsInvestigatorRepoRoot}/include/*.h,true)
COMMON_HDRSPP	= $$files($${programsInvestigatorRepoRoot}/include/*.hpp,true)

HEADERS += $$COMMON_HDRS
HEADERS += $$COMMON_HDRSPP

OTHER_FILES += $$files($${PWD}/../liballoc_free_handler_mkfl/*.Makefile,false)
