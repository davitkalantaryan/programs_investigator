#
# file:				alloc_free_hook_all.pro
# path:				workspaces/alloc_free_hook_all_qt/alloc_free_hook_all.pro
# created on:		2023 Mar 14
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = subdirs
#CONFIG += ordered

repositoryRoot = $${PWD}/../..

SUBDIRS		+=	"$${repositoryRoot}/prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro"

OTHER_FILES += $$files($${repositoryRoot}/docs/*.md,true)
OTHER_FILES += $$files($${repositoryRoot}/docs/*.txt,true)

OTHER_FILES	+=	\
        "$${repositoryRoot}/.gitattributes"					\
	"$${repositoryRoot}/.gitignore"						\
	"$${repositoryRoot}/.gitmodules"						\
	"$${repositoryRoot}/LICENSE"						\
	"$${repositoryRoot}/README.md"	
