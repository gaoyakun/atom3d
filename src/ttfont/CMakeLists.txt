INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/ttfont")
INCLUDE_DIRECTORIES("freetype/include")

AUX_SOURCE_DIRECTORY(. TTFontSrc)
AUX_SOURCE_DIRECTORY(cc/cp936 CP936Src)
AUX_SOURCE_DIRECTORY(cc/cp950 CP950Src)
AUX_SOURCE_DIRECTORY(cc/iso8859_1 ISO8859_1Src)

FILE(GLOB TTFontPrivateHeaders *.h)
FILE(GLOB TTFontPublicHeaders ${ATOM3D_INCLUDE_DIR}/ttfont/*.h)
SET(TTFontSrcHeaders ${TTFontSrc} ${TTFontPrivateHeaders})
SET(CCSrcHeaders ${CP936Src} ${CP950Src} ${ISO8859_1Src}) 

SET(FreeTypeSrc
		freetype/src/base/ftbbox.c
		freetype/src/base/ftgxval.c
		freetype/src/base/ftlcdfil.c
		freetype/src/base/ftmm.c
		freetype/src/base/ftotval.c
		freetype/src/base/ftpatent.c
		freetype/src/base/ftpfr.c
		freetype/src/base/ftsynth.c
		freetype/src/base/fttype1.c
		freetype/src/base/ftwinfnt.c
		freetype/src/base/ftxf86.c
		freetype/src/pcf/pcf.c
		freetype/src/pfr/pfr.c
		freetype/src/psaux/psaux.c
		freetype/src/pshinter/pshinter.c
		freetype/src/psnames/psmodule.c
		freetype/src/raster/ftraster.c
		freetype/src/raster/ftrend1.c
		freetype/src/sfnt/sfnt.c
		freetype/src/truetype/truetype.c
		freetype/src/type1/type1.c
		freetype/src/cid/type1cid.c
		freetype/src/type42/type42.c
		freetype/src/winfonts/winfnt.c
		freetype/src/autofit/autofit.c
		freetype/src/bdf/bdf.c
		freetype/src/cff/cff.c
		freetype/src/base/ftbase.c
		freetype/src/base/ftbitmap.c
		freetype/src/cache/ftcache.c
		freetype/src/base/ftdebug.c
		freetype/src/base/ftfstype.c
		freetype/src/base/ftgasp.c
		freetype/src/base/ftglyph.c
		freetype/src/gzip/ftgzip.c
		freetype/src/base/ftinit.c
		freetype/src/lzw/ftlzw.c
		freetype/src/base/ftstroke.c
		freetype/src/base/ftsystem.c
		freetype/src/smooth/smooth.c
	)		
	
SOURCE_GROUP(headers FILES ${TTFontPublicHeaders})
SOURCE_GROUP(source FILES ${TTFontSrcHeaders})
SOURCE_GROUP(cc FILES ${CCSrcHeaders})
SOURCE_GROUP(freetype FILES ${FreeTypeSrc})

IF(DEFINED PC_LINT_ENABLE)
	set(PC_LINT_OPT_DIR ${TOPLEVEL_DIR}/)
	set(PC_LINT_USER_FLAGS "-b")
	if (COMMAND add_pc_lint)
	  add_pc_lint(ATOM_ttfont ${TTFontSrcHeaders} ${CCSrcHeaders} ${TTFontPublicHeaders})
	endif (COMMAND add_pc_lint)
ENDIF(DEFINED PC_LINT_ENABLE)

ADD_DEFINITIONS(-DBUILD_ATOM_FONT -DFT2_BUILD_LIBRARY)
IF(MSVC)
	ADD_DEFINITIONS(/wd4800)
ENDIF(MSVC)

ADD_LIBRARY(ATOM_ttfont ${ATOM3D_SHARED} ${TTFontSrcHeaders} ${CCSrcHeaders} ${TTFontPublicHeaders} ${FreeTypeSrc})
ADD_DEPENDENCIES(ATOM_ttfont ATOM_kernel)
TARGET_LINK_LIBRARIES(ATOM_ttfont ATOM_kernel)

INSTALL_TARGET(ATOM_ttfont)
