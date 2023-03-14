#
# file:				flags_common.pri
# path:				prj/common/common_qt/flags_common.pri  
# created on:		2023 Mar 14
# Created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message ("$${PWD}/flags_common.pri")

isEmpty( allocfreehookRepoRoot ) {
	allocfreehookRepoRoot = $${PWD}/../../..
}

isEmpty( cinternalRepoRoot ) {
	cinternalRepoRoot=$${allocfreehookRepoRoot}/contrib/cinternal
}

include ( "$${cinternalRepoRoot}/prj/common/common_qt/flags_common.pri" )

INCLUDEPATH += $${allocfreehookRepoRoot}/include
