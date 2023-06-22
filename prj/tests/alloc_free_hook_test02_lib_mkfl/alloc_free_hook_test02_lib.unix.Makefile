
targetName=alloc_free_hook_test02_lib

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB

include $(mkfile_dir)/../../common/common_mkfl/sys_common.unix.Makefile

SOURCES	= $(shell find $(programsInvestigatorRepoRoot)/src/core/alloc_free_hook -name "*.c")
SOURCES += $(programsInvestigatorRepoRoot)/src/tests/entry_alloc_free_hook_test02_lib.cpp

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so.1

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so.1: \
		$(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@mkdir -p $(@D)/../tlib
	@$(LINK) $^ -Wl,-E,-soname,lib$(targetName).so.1 -pie -shared $(LFLAGS) -o $@ $(LIBS) -ldl -pthread
	@rm -f $(@D)/../tlib/lib$(targetName).so
	@cd $(@D)/../tlib && ln -s ../tdll/lib$(targetName).so.1 lib$(targetName).so

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tdll/lib$(targetName).so*
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/tlib/lib$(targetName).so*
	@echo "  " cleaning of $(targetName) complete !!!
