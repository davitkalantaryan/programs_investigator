#
# file:				alloc_free_hook_all.pro
# path:				workspaces/alloc_free_hook_all_qt/alloc_free_hook_all.pro
# created on:		2023 Mar 14
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

TEMPLATE = subdirs
CONFIG += ordered

include ( "$${PWD}/../../prj/common/common_qt/flags_common.pri" )

SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/core/libprogs_invest_basic_qt/libprogs_invest_basic.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/core/libanalyze_leaking_qt/libanalyze_leaking.pro"

SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/alloc_free_hook_test01_qt/alloc_free_hook_test01.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/alloc_free_hook_test02_lib_qt/alloc_free_hook_test02_lib.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/alloc_free_hook_test02_exe_qt/alloc_free_hook_test02_exe.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/alloc_free_hook_test03_qt/alloc_free_hook_test03.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/test_lib_to_change_alloc_free_qt/test_lib_to_change_alloc_free.pro"
SUBDIRS		+=	"$${programsInvestigatorRepoRoot}/prj/tests/stdout_invest_test_qt/stdout_invest_test.pro"

SUBDIRS += "$${cinternalRepoRoot}/workspaces/cinternal_all_qt/cinternal_all.pro"



OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/docs/*.md,true)
OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/prj/common/common_mkfl/*.Makefile,true)
OTHER_FILES += $$files($${programsInvestigatorRepoRoot}/workspaces/programs_investigator_all_mkfl/*.Makefile,true)

OTHER_FILES	+=	\
        "$${repositoryRoot}/.gitattributes"							\
	"$${repositoryRoot}/.gitignore"								\
	"$${repositoryRoot}/.gitmodules"							\
	"$${repositoryRoot}/ENVIRONMENT"							\
	"$${repositoryRoot}/LICENSE"								\
        "$${repositoryRoot}/README.md"
