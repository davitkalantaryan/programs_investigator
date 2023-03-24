
targetName=test_lib_to_change_alloc_free

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

COMMON_FLAGS += -fPIC -DALLOCFREEHOOK_COMPILING_SHARED_LIB
COMMON_FLAGS += -DALLOCFREEHOOK_LOAD_FROM_DLL

include $(mkfile_dir)/../../common/common_mkfl/unix.common.Makefile

SOURCES += $(allocFreeRepoRoot)/src/tests/entry_test_lib_to_change_alloc_free.c

LIBS += -lalloc_free_handler -L$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/lib -pthread

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
