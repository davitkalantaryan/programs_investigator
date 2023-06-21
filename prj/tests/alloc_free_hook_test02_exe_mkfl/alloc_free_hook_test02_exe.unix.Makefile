
targetName=alloc_free_hook_test02

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/unix.common.Makefile

SOURCES	= $(shell find $(programsInvestigatorRepoRoot)/src/core/alloc_free_hook -name "*.c")
SOURCES += $(programsInvestigatorRepoRoot)/src/tests/main_alloc_free_hook_test02_exe.cpp

COMMON_FLAGS += -DALLOCFREEHOOK_LOAD_FROM_DLL

LIBS += -lalloc_free_hook_test02_lib -L$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/lib -ldl

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName): \
        $(SOURCES:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@$(LINK) $^ $(LFLAGS) -o $@ $(LIBS)

.PHONY: clean
clean:
        @rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
        @rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of $(targetName) complete !!!
