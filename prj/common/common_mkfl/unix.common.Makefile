

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))
allocFreeRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})
ifndef repoRootPath
        repoRootPath	= $(allocFreeRepoRoot)
endif

COMMON_FLAGS	+= -I$(allocFreeRepoRoot)/include

include $(allocFreeRepoRoot)/contrib/cinternal/prj/common/common_mkfl/unix.common.Makefile
