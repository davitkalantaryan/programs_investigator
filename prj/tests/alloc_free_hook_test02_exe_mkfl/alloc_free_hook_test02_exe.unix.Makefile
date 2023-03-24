
targetName=alloc_free_hook_test02

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/unix.common.Makefile

SOURCES	= $(shell find $(allocFreeRepoRoot)/src/core/alloc_free_hook -name "*.c")
SOURCES += $(allocFreeRepoRoot)/src/tests/main_alloc_free_hook_test02_exe.cpp

COMMON_FLAGS += -DALLOCFREEHOOK_LOAD_FROM_DLL

LIBS += -lalloc_free_hook_test02_lib -L$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/lib -ldl

all: $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)

$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName): \
	$(SOURCES:%=$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@$(LINK) $^ $(LFLAGS) -o $@ $(LIBS)

.PHONY: clean
clean:
	@rm -rf $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of $(targetName) complete !!!
