#
# file:				alloc_free_hook_all.pro
# path:				workspaces/alloc_free_hook_all_qt/alloc_free_hook_all.pro
# created on:		2023 Mar 14
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = subdirs
#CONFIG += ordered

repositoryRoot = $${PWD}/../..

SUBDIRS		+=	"$${repositoryRoot}/prj/core/liballoc_free_handler_qt/liballoc_free_handler.pro"

SUBDIRS		+=	"$${repositoryRoot}/prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/alloc_free_hook_test02_lib_qt/alloc_free_hook_test02_lib.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/alloc_free_hook_test02_exe_qt/alloc_free_hook_test02_exe.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/alloc_free_hook_test03_qt/alloc_free_hook_test03.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${repositoryRoot}/prj/tests/test_lib_to_change_alloc_free_qt/test_lib_to_change_alloc_free.pro"



OTHER_FILES += $$files($${repositoryRoot}/docs/*.md,true)
OTHER_FILES += $$files($${repositoryRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${repositoryRoot}/scripts/*.bat,true)

OTHER_FILES	+=	\
        "$${repositoryRoot}/.gitattributes"							    \
	"$${repositoryRoot}/.gitignore"								    \
	"$${repositoryRoot}/.gitmodules"							    \
	"$${repositoryRoot}/LICENSE"								    \
	"$${repositoryRoot}/README.md"								    \
	"$${repositoryRoot}/workspaces/alloc_free_hook_all_mkfl/alloc_free_hook_all.unix.Makefile"  \
	"$${PWD}/../../prj/common/common_mkfl/unix.common.Makefile"				    \
	"$${PWD}/../../prj/common/common_mkfl/windows.common.Makefile"
