

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

ifndef programsInvestigatorRepoRoot
        programsInvestigatorRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})
endif

ifndef repositoryRoot
        repositoryRoot	= $(programsInvestigatorRepoRoot)
endif

ifndef artifactRoot
        artifactRoot	= $(repositoryRoot)
endif

ifndef stackInvestigatorRepoRoot
        stackInvestigatorRepoRoot	= $(programsInvestigatorRepoRoot)/contrib/stack_investigator
endif


COMMON_FLAGS	+= -I$(programsInvestigatorRepoRoot)/include

include $(stackInvestigatorRepoRoot)/prj/common/common_mkfl/sys_common.unix.Makefile
