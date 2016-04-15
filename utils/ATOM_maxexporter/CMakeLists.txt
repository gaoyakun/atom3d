INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/geometry")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/3dsmax2010sdk")
LINK_DIRECTORIES("${THIRDPART_DIR}/lib/3dsmax2010sdk")
LINK_DIRECTORIES("${THIRDPART_DIR}/lib/tinyxml")

AUX_SOURCE_DIRECTORY(. ExporterSrc)
FILE(GLOB ExporterHeaders *.h)

ADD_DEFINITIONS(/DISOLATION_AWARE_ENABLED=1)

ADD_LIBRARY(ATOM_maxexporter SHARED ${ExporterSrc} ${ExporterHeaders} ATOM_maxexporter.def ATOM_maxexporter.rc)
SET_TARGET_PROPERTIES(ATOM_maxexporter PROPERTIES 
	SUFFIX ".dle" 
)
ADD_DEPENDENCIES(ATOM_maxexporter ATOM_geometry)
TARGET_LINK_LIBRARIES(ATOM_maxexporter ATOM_geometry comctl32 core geom gfx mesh maxutil maxscrpt paramblk2 bmm)

INSTALL_TARGET(ATOM_maxexporter)
