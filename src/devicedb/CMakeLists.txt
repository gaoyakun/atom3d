INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/devicedb")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")

AUX_SOURCE_DIRECTORY(. DeviceDBSrc)
FILE(GLOB DeviceDBPrivateHeaders *.h)
FILE(GLOB DeviceDBPublicHeaders ${ATOM3D_INCLUDE_DIR}/devicedb/*.h)
SET(DeviceDBSrcHeaders ${DeviceDBSrc} ${DeviceDBPrivateHeaders})
SOURCE_GROUP(headers FILES ${DeviceDBPublicHeaders})
SOURCE_GROUP(source FILES ${DeviceDBSrcHeaders})

ADD_LIBRARY(ATOM_devicedb STATIC ${DeviceDBSrcHeaders} ${DeviceDBPublicHeaders})

INSTALL_TARGET(ATOM_devicedb)
