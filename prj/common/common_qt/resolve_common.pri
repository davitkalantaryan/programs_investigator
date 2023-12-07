#
# file:			resolve_common.pri
# path:			prj/common/common_qt/resolve_common.pri
# created on:		2023 Jun 21
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/resolve_common.pri")
programsInvestigatorResolveCommonIncluded = 1

isEmpty( programsInvestigatorRepoRoot ) {
        programsInvestigatorRepoRoot = $$(programsInvestigatorRepoRoot)
        isEmpty(programsInvestigatorRepoRoot) {
            programsInvestigatorRepoRoot = $${PWD}/../../..
        }
}

isEmpty( repositoryRoot ) {
        repositoryRoot = $$(repositoryRoot)
        isEmpty(repositoryRoot) {
            repositoryRoot = $${programsInvestigatorRepoRoot}
        }
}

isEmpty(artifactRoot) {
    artifactRoot = $$(artifactRoot)
    isEmpty(artifactRoot) {
        artifactRoot = $${repositoryRoot}
    }
}

isEmpty(stackInvestigatorRepoRoot) {
    stackInvestigatorRepoRoot = $$(stackInvestigatorRepoRoot)
    isEmpty(stackInvestigatorRepoRoot) {
        stackInvestigatorRepoRoot=$${programsInvestigatorRepoRoot}/contrib/stack_investigator
    }
}
