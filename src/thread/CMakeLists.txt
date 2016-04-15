INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/thread")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")

AUX_SOURCE_DIRECTORY(. ThreadSrc)
FILE(GLOB ThreadPrivateHeaders *.h)
FILE(GLOB ThreadPublicHeaders ${ATOM3D_INCLUDE_DIR}/thread/*.h)
SET(ThreadSrcHeaders ${ThreadSrc} ${ThreadPrivateHeaders})
SOURCE_GROUP(headers FILES ${ThreadPublicHeaders})
SOURCE_GROUP(source FILES ${ThreadSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_THREAD)
ADD_LIBRARY(ATOM_thread ${ATOM3D_SHARED} ${ThreadSrcHeaders} ${ThreadPublicHeaders})

INSTALL_TARGET(ATOM_thread)
