INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/archive")
INCLUDE_DIRECTORIES("../zlib")

AUX_SOURCE_DIRECTORY(. ArchiveSrc)
FILE(GLOB ArchivePrivateHeaders *.h)
FILE(GLOB ArchivePublicHeaders ${ATOM3D_INCLUDE_DIR}/archive/*.h)
SET(ArchiveSrcHeaders ${ArchiveSrc} ${ArchivePrivateHeaders})
SOURCE_GROUP(headers FILES ${ArchivePublicHeaders})
SOURCE_GROUP(source FILES ${ArchiveSrcHeaders})

ADD_LIBRARY(ATOM_archive STATIC ${ArchiveSrcHeaders} ${ArchivePublicHeaders})
ADD_DEPENDENCIES(ATOM_archive zlib_s)
TARGET_LINK_LIBRARIES(ATOM_archive zlib_s)
INSTALL_TARGET(ATOM_archive)
