#
# repo:			programs_investigator
# file:				flags_common.pri
# path:				prj/common/common_qt/flags_common.pri  
# created on:		2023 Mar 14
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/flags_common.pri")
programsInvestigatorFlagsCommonIncluded = 1

isEmpty( programsInvestigatorResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
	programsInvestigatorResolveCommonIncluded = 1
}

isEmpty( stackInvestigatorFlagsCommonIncluded ) {
        include ( "$${stackInvestigatorRepoRoot}/prj/common/common_qt/flags_common.pri" )
	stackInvestigatorFlagsCommonIncluded = 1
}

INCLUDEPATH += $${programsInvestigatorRepoRoot}/include
