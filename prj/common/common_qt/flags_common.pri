#
# file:				flags_common.pri
# path:				prj/common/common_qt/flags_common.pri  
# created on:		2023 Mar 14
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/flags_common.pri")

isEmpty( programsInvestigatorRepoRoot ) {
        programsInvestigatorRepoRoot = $${PWD}/../../..
}

isEmpty( repositoryRoot ) {
        repositoryRoot = $${programsInvestigatorRepoRoot}
}


isEmpty( stackInvestigatorRepoRoot ) {
        stackInvestigatorRepoRoot=$${programsInvestigatorRepoRoot}/contrib/stack_investigator
}

include ( "$${stackInvestigatorRepoRoot}/prj/common/common_qt/flags_common.pri" )

INCLUDEPATH += $${programsInvestigatorRepoRoot}/include
