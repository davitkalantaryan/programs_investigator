
targetName=libprogs_invest_basic

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB

include $(mkfile_dir)/../../common/common_mkfl/sys_common.unix.Makefile

SOURCES	+= $(shell find $(programsInvestigatorRepoRoot)/src/core/alloc_free_hook -name "*.c")

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/$(targetName).so.$(PROGS_INVEST_VERSION)

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/$(targetName).so.$(PROGS_INVEST_VERSION): \
		$(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@mkdir -p $(@D)/../lib
	@rm -f $(@D)/$(targetName).so.*
	@$(LINK) $^ -Wl,-E,-soname,$(targetName).so.$(PROGS_INVEST_VERSION) -pie -shared -ldl $(LIBS) $(LFLAGS) -o $@
	@rm -f $(@D)/../lib/$(targetName).so
	@#cd $(@D) && ln -s $(targetName).so.$(PROGS_INVEST_VERSION) $(targetName).so.$(PROGS_INVEST_VERSION_MM)
	@#cd $(@D) && ln -s $(targetName).so.$(PROGS_INVEST_VERSION_MM) $(targetName).so.$(PROGS_INVEST_VERSION_MAJ)
	@cd $(@D)/../lib && ln -s ../dll/$(targetName).so.$(PROGS_INVEST_VERSION) $(targetName).so

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/dll/$(targetName).so*
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib/$(targetName).so*
	@echo "  " cleaning of $(targetName) complete !!!
