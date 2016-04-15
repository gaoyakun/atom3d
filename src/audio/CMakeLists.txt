INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/audio")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(libvorbis/include)
INCLUDE_DIRECTORIES(libvorbis/lib)
AUX_SOURCE_DIRECTORY(. AudioSrc)

FILE(GLOB AudioPrivateHeaders *.h)
FILE(GLOB AudioPublicHeaders ${ATOM3D_INCLUDE_DIR}/audio/*.h)
SET(AudioSrcHeaders ${AudioSrc} ${AudioPrivateHeaders})
SOURCE_GROUP(headers FILES ${AudioPublicHeaders})
SOURCE_GROUP(source FILES ${AudioSrcHeaders})

AUX_SOURCE_DIRECTORY(ogg OggSrc)
FILE(GLOB OggHeaders ogg/*.h)
SET(OggSrcHeaders ${OggSrc} ${OggHeaders})
SOURCE_GROUP(ogg FILES ${OggSrcHeaders})

SET(VorbisSrc
	libvorbis/lib/analysis.c
	libvorbis/lib/bitrate.c
	libvorbis/lib/block.c
	libvorbis/lib/codebook.c
	libvorbis/lib/envelope.c
	libvorbis/lib/floor0.c
	libvorbis/lib/floor1.c
	libvorbis/lib/info.c
	libvorbis/lib/lookup.c
	libvorbis/lib/lpc.c
	libvorbis/lib/lsp.c
	libvorbis/lib/mapping0.c
	libvorbis/lib/mdct.c
	libvorbis/lib/psy.c
	libvorbis/lib/registry.c
	libvorbis/lib/res0.c
	libvorbis/lib/sharedbook.c
	libvorbis/lib/smallft.c
	libvorbis/lib/synthesis.c
	libvorbis/lib/vorbisenc.c
	libvorbis/lib/window.c
	)
SOURCE_GROUP(vorbis FILES ${VorbisSrc})

SET(VorbisFileSrc libvorbis/lib/vorbisfile.c)
SOURCE_GROUP(vorbisfile FILES ${VorbisFileSrc})

ADD_DEFINITIONS(-DBUILD_ATOM_AUDIO)
ADD_LIBRARY(ATOM_audio ${ATOM3D_SHARED} ${AudioSrcHeaders} ${AudioPublicHeaders} ${OggSrcHeaders} ${VorbisSrc} ${VorbisFileSrc})
ADD_DEPENDENCIES(ATOM_audio ATOM_kernel)
TARGET_LINK_LIBRARIES(ATOM_audio ATOM_kernel winmm)

INSTALL_TARGET(ATOM_audio)
