
targetName=alloc_free_hook_test02_lib

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB

include $(mkfile_dir)/../../common/common_mkfl/unix.common.Makefile

SOURCES	= $(shell find $(allocFreeRepoRoot)/src/core/alloc_free_hook -name "*.c")
SOURCES += $(allocFreeRepoRoot)/src/tests/entry_alloc_free_hook_test02_lib.cpp

all: $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/dll/lib$(targetName).so.1

$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/dll/lib$(targetName).so.1: \
	$(SOURCES:%=$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@mkdir -p $(@D)/../lib
	@$(LINK) $^ -Wl,-E,-soname,lib$(targetName).so.1 -pie -shared -ldl $(LIBS) $(LFLAGS) -o $@
	@rm -f $(@D)/../lib/lib$(targetName).so
	@cd $(@D)/../lib && ln -s ../dll/lib$(targetName).so.1 lib$(targetName).so

.PHONY: clean
clean:
	@rm -rf $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/dll/lib$(targetName).so.1
	@rm -f  $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/lib/lib$(targetName).so
	@echo "  " cleaning of $(targetName) complete !!!
