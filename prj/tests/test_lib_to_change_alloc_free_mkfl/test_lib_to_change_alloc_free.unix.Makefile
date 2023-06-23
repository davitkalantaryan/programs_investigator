
targetName=test_lib_to_change_alloc_free

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB
COMMON_FLAGS += -DALLOCFREEHOOK_LOAD_FROM_DLL

include $(mkfile_dir)/../../common/common_mkfl/sys_common.unix.Makefile

SOURCES += $(programsInvestigatorRepoRoot)/src/tests/entry_test_lib_to_change_alloc_free.c

LIBS += -lprogs_invest_basic -L$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib -pthread

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so.1

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so.1: \
		$(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@mkdir -p $(@D)/../tlib
	@$(LINK) $^ -Wl,-E,-soname,lib$(targetName).so.1 -pie -shared -ldl $(LIBS) $(LFLAGS) -o $@
	@rm -f $(@D)/../tlib/lib$(targetName).so
	@cd $(@D)/../tlib && ln -s ../tdll/lib$(targetName).so.1 lib$(targetName).so

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so*
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tlib/lib$(targetName).so*
	@echo "  " cleaning of $(targetName) complete !!!
