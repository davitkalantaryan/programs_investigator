#
# repo:			programs_investigator
# file:				sys_common.pri
# path:				prj/common/common_qt/sys_common.pri    
# created on:		2023 Mar 14
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/sys_common.pri")
programsInvestigatorSysCommonIncluded = 1

isEmpty( programsInvestigatorResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
	programsInvestigatorResolveCommonIncluded = 1
}

isEmpty( stackInvestigatorSysCommonIncluded ) {
        include ( "$${stackInvestigatorRepoRoot}/prj/common/common_qt/sys_common.pri" )
	stackInvestigatorSysCommonIncluded = 1
}
