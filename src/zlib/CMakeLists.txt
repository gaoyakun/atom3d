INCLUDE_DIRECTORIES(".")

AUX_SOURCE_DIRECTORY(. ZlibSrc)
FILE(GLOB ZlibHeaders *.h)
SET(ZlibSrcHeaders ${ZlibSrc} ${ZlibHeaders})
SOURCE_GROUP(headers FILES ${ZlibHeaders})
SOURCE_GROUP(source FILES ${ZlibSrc})

ADD_LIBRARY(zlib_s STATIC ${ZlibSrcHeaders})
INSTALL_TARGET(zlib_s)
