INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/image")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")
INCLUDE_DIRECTORIES(../zlib)
INCLUDE_DIRECTORIES(libjpeg)
INCLUDE_DIRECTORIES(libpng)
INCLUDE_DIRECTORIES(squish)

AUX_SOURCE_DIRECTORY(. ImageSrc)
FILE(GLOB ImagePrivateHeaders *.h)
FILE(GLOB ImagePublicHeaders ${ATOM3D_INCLUDE_DIR}/image/*.h)
SET(ImageSrcHeaders ${ImageSrc} ${ImagePrivateHeaders})
SOURCE_GROUP(headers FILES ${ImagePublicHeaders})
SOURCE_GROUP(source FILES ${ImageSrcHeaders})

AUX_SOURCE_DIRECTORY(libjpeg JpegSrc)
FILE(GLOB JpegHeaders libjpeg/*.h)
SET(JpegSrcHeaders ${JpegSrc} ${JpegHeaders})
SOURCE_GROUP(libjpeg FILES ${JpegSrcHeaders})

AUX_SOURCE_DIRECTORY(libpng PngSrc)
FILE(GLOB PngHeaders libpng/*.h)
SET(PngSrcHeaders ${PngSrc} ${PngHeaders})
SOURCE_GROUP(libpng FILES ${PngSrcHeaders})

AUX_SOURCE_DIRECTORY(squish SquishSrc)
FILE(GLOB SquishHeaders squish/*.h)
SET(SquishSrcHeaders ${SquishSrc} ${SquishHeaders})
SOURCE_GROUP(squish FILES ${SquishSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_IMAGE)
if (MSVC)
	ADD_DEFINITIONS(-DSQUISH_USE_SSE=2)
endif (MSVC)

FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

ADD_LIBRARY(ATOM_image ${ATOM3D_SHARED} ${ImageSrcHeaders} ${ImagePublicHeaders} ${JpegSrcHeaders} ${PngSrcHeaders} ${SquishSrcHeaders})
ADD_PRECOMPILED_HEADER(ATOM_image ${PCH_Header} ${PCH_Source} ImageSrcHeaders)
ADD_DEPENDENCIES(ATOM_image ATOM_kernel zlib_s)
TARGET_LINK_LIBRARIES(ATOM_image ATOM_kernel zlib_s)

INSTALL_TARGET(ATOM_image)
