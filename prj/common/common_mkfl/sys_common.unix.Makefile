

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

include $(programsInvestigatorRepoRoot)/ENVIRONMENT

COMMON_FLAGS	+= -I$(programsInvestigatorRepoRoot)/include
PROGS_INVEST_VERSION	= $(PROGS_INVEST_VERSION_MAJ).$(PROGS_INVEST_VERSION_MIN).$(PROGS_INVEST_VERSION_PAT)
PROGS_INVEST_VERSION_MM = $(PROGS_INVEST_VERSION_MAJ).$(PROGS_INVEST_VERSION_MIN)

include $(stackInvestigatorRepoRoot)/prj/common/common_mkfl/sys_common.unix.Makefile
