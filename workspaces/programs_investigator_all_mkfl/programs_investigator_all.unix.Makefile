

targetName=programs_investigator_all

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

include $(mkfile_dir)/../../prj/common/common_mkfl/sys_common.unix.Makefile

all:
	make -f $(cinternalRepoRoot)/workspaces/cinternal_all_mkfl/cinternal_all.unix.Makefile artifactRoot=$(artifactRoot)
	
	make -f $(programsInvestigatorRepoRoot)/prj/core/libprogs_invest_basic_mkfl/libprogs_invest_basic.unix.Makefile
	
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test01_mkfl/alloc_free_hook_test01.unix.Makefile
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test02_lib_mkfl/alloc_free_hook_test02_lib.unix.Makefile
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test02_exe_mkfl/alloc_free_hook_test02_exe.unix.Makefile
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test03_mkfl/alloc_free_hook_test03.unix.Makefile
	make -f $(programsInvestigatorRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile
	make -f $(programsInvestigatorRepoRoot)/prj/tests/test_lib_to_change_alloc_free_mkfl/test_lib_to_change_alloc_free.unix.Makefile
	
	

.PHONY: clean
clean:
	make -f $(cinternalRepoRoot)/workspaces/cinternal_all_mkfl/cinternal_all.unix.Makefile artifactRoot=$(artifactRoot) clean
	
	make -f $(programsInvestigatorRepoRoot)/prj/core/libprogs_invest_basic_mkfl/libprogs_invest_basic.unix.Makefile clean
	
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test01_mkfl/alloc_free_hook_test01.unix.Makefile clean
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test02_lib_mkfl/alloc_free_hook_test02_lib.unix.Makefile clean
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test02_exe_mkfl/alloc_free_hook_test02_exe.unix.Makefile clean
	make -f $(programsInvestigatorRepoRoot)/prj/tests/alloc_free_hook_test03_mkfl/alloc_free_hook_test03.unix.Makefile clean
	make -f $(programsInvestigatorRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile clean
	make -f $(programsInvestigatorRepoRoot)/prj/tests/test_lib_to_change_alloc_free_mkfl/test_lib_to_change_alloc_free.unix.Makefile clean
