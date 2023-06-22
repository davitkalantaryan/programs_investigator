
targetName=libprogs_invest_basic

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB

include $(mkfile_dir)/../../common/common_mkfl/sys_common.unix.Makefile
include $(programsInvestigatorRepoRoot)/ENVIRONMENT

SOURCES	= $(shell find $(programsInvestigatorRepoRoot)/src/core/alloc_free_hook -name "*.c")

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/lib$(targetName).so.1

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/lib$(targetName).so.1: \
		$(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@mkdir -p $(@D)/../lib
	@$(LINK) $^ -Wl,-E,-soname,$(targetName).so.$(PROGS_INVEST_VERSION) -pie -shared -ldl $(LIBS) $(LFLAGS) -o $@
	@rm -f $(@D)/../lib/$(targetName).so
	@cd $(@D)/../lib && ln -s ../dll/$(targetName).so.$(PROGS_INVEST_VERSION) $(targetName).so

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/$(targetName).so*
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib/$(targetName).so
	@echo "  " cleaning of $(targetName) complete !!!
