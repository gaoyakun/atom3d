INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${ATOM3DX_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${TOPLEVEL_INCLUDE_DIR}/atom3d_studio")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/src/AntTweakBar/include")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/src/otl/include")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/assimp")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/ffmpeg")
INCLUDE_DIRECTORIES(".")

IF (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x64")
ELSE (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x86")
ENDIF (BUILD_WIN64)

AUX_SOURCE_DIRECTORY(. StudioSrc)
FILE(GLOB StudioPrivateHeaders *.h)
set(StudioSrcHeaders ${StudioSrc} ${StudioPrivateHeaders})

AUX_SOURCE_DIRECTORY(scene_plugin ScenePluginSrc)
FILE(GLOB ScenePluginPrivateHeaders scene_plugin/*.h)
set(ScenePluginSrcHeaders ${ScenePluginSrc} ${ScenePluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(model_plugin ModelPluginSrc)
FILE(GLOB ModelPluginPrivateHeaders model_plugin/*.h)
set(ModelPluginSrcHeaders ${ModelPluginSrc} ${ModelPluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(tree_plugin TreePluginSrc)
FILE(GLOB TreePluginPrivateHeaders tree_plugin/*.h)
set(TreePluginSrcHeaders ${TreePluginSrc} ${TreePluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(actor_plugin ActorPluginSrc)
FILE(GLOB ActorPluginPrivateHeaders actor_plugin/*.h)
set(ActorPluginSrcHeaders ${ActorPluginSrc} ${ActorPluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(ribbontrail_plugin RibbonTrailPluginSrc)
FILE(GLOB RibbonTrailPluginPrivateHeaders ribbontrail_plugin/*.h)
set(RibbonTrailPluginSrcHeaders ${RibbonTrailPluginSrc} ${RibbonTrailPluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(gui_plugin GuiPluginSrc)
FILE(GLOB GuiPluginPrivateHeaders gui_plugin/*.h)
set(GuiPluginSrcHeaders ${GuiPluginSrc} ${GuiPluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(composition_plugin CompositionPluginSrc)
FILE(GLOB CompositionPluginPrivateHeaders composition_plugin/*.h)
set(CompositionPluginSrcHeaders ${CompositionPluginSrc} ${CompositionPluginPrivateHeaders})

AUX_SOURCE_DIRECTORY(misc MiscSrc)
FILE(GLOB MiscPrivateHeaders misc/*.h)
set(MiscSrcHeaders ${MiscSrc} ${MiscPrivateHeaders})

FILE(GLOB StudioPublicHeaders ${TOPLEVEL_INCLUDE_DIR}/atom3d_studio/*.h)

SOURCE_GROUP(headers FILES ${StudioPublicHeaders})
SOURCE_GROUP(source FILES ${StudioSrcHeaders})
SOURCE_GROUP(scene_plugin FILES ${ScenePluginSrcHeaders})
SOURCE_GROUP(model_plugin FILES ${ModelPluginSrcHeaders})
SOURCE_GROUP(tree_plugin FILES ${TreePluginSrcHeaders})
SOURCE_GROUP(actor_plugin FILES ${ActorPluginSrcHeaders})
SOURCE_GROUP(ribbontrail_plugin FILES ${RibbonTrailPluginSrcHeaders})
SOURCE_GROUP(gui_plugin FILES ${GuiPluginSrcHeaders})
SOURCE_GROUP(composition_plugin FILES ${CompositionPluginSrcHeaders})
SOURCE_GROUP(misc FILES ${MiscSrcHeaders})
set(AllSrc
	${StudioSrcHeaders}
	${ScenePluginSrcHeaders}
	${ModelPluginSrcHeaders}
	${TreePluginSrcHeaders}
	${ActorPluginSrcHeaders}
	${RibbonTrailPluginSrcHeaders}
	${GuiPluginSrcHeaders}
	${CompositionPluginSrcHeaders}
	${MiscSrcHeaders}
	)	

FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

IF(ATOM3D_BUILD_ANTTWEAKBAR_STATIC)
	ADD_DEFINITIONS(-DTW_STATIC)
ENDIF(ATOM3D_BUILD_ANTTWEAKBAR_STATIC)
ADD_DEFINITIONS(-DTW_NO_LIB_PRAGMA)

ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
ADD_EXECUTABLE(ATOM_studio WIN32 ${AllSrc} ${StudioPublicHeaders} ATOM_studio.rc)
ADD_PRECOMPILED_HEADER(ATOM_studio ${PCH_Header} ${PCH_Source} AllSrc)

ADD_DEPENDENCIES(ATOM_studio ATOM_kernel ATOM_engine ATOM_commondlg ATOM_render ATOM_archive ATOMX)
TARGET_LINK_LIBRARIES(ATOM_studio ATOM_kernel ATOM_engine ATOM_commondlg ATOM_archive ATOM_lzmapkg ATOMX psapi d3dx9 dxerr wininet Shlwapi)

INSTALL_TARGET(ATOM_studio)
