INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/geometry")

AUX_SOURCE_DIRECTORY(. GeometrySrc)
FILE(GLOB GeometryPrivateHeaders *.h)
FILE(GLOB GeometryPublicHeaders ${ATOM3D_INCLUDE_DIR}/geometry/*.h)
SET(GeometrySrcHeaders ${GeometrySrc} ${GeometryPrivateHeaders})
SOURCE_GROUP(headers FILES ${GeometryPublicHeaders})
SOURCE_GROUP(source FILES ${GeometrySrcHeaders})

ADD_LIBRARY(ATOM_geometry STATIC ${GeometrySrcHeaders} ${GeometryPublicHeaders})

INSTALL_TARGET(ATOM_geometry)
