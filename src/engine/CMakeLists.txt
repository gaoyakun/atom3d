INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${ATOM3D_INCLUDE_DIR}/engine"
	"${THIRDPART_DIR}/include"
	"${THIRDPART_DIR}/include/dxsdk"
	"${THIRDPART_DIR}/include/ffmpeg"
)

AUX_SOURCE_DIRECTORY(. EngineSrc)
FILE(GLOB EnginePrivateHeaders *.h)
FILE(GLOB EnginePublicHeaders ${ATOM3D_INCLUDE_DIR}/engine/*.h)
SET(EngineSrcHeaders ${EngineSrc} ${EnginePrivateHeaders})
SOURCE_GROUP(headers FILES ${EnginePublicHeaders})
SOURCE_GROUP(source FILES ${EngineSrcHeaders})
SOURCE_GROUP(flash FILES
	${ATOM3D_INCLUDE_DIR}/engine/delegate.h 
	${ATOM3D_INCLUDE_DIR}/engine/flashcontrol.h flashcontrol.cpp
	${ATOM3D_INCLUDE_DIR}/engine/flashhandler.h 
	${ATOM3D_INCLUDE_DIR}/engine/flashmanager.h flashmanager.cpp
	${ATOM3D_INCLUDE_DIR}/engine/flashsite.h
	${ATOM3D_INCLUDE_DIR}/engine/flashvalue.h flashvalue.cpp
	${ATOM3D_INCLUDE_DIR}/engine/position.h
	${ATOM3D_INCLUDE_DIR}/engine/renderbuffer.h
	keyboardhook.h keyboardhook.cpp
)
SOURCE_GROUP(gui FILES 
	${ATOM3D_INCLUDE_DIR}/engine/label.h label.cpp 
	${ATOM3D_INCLUDE_DIR}/engine/button.h button.cpp  
	${ATOM3D_INCLUDE_DIR}/engine/edit.h edit.cpp 
	${ATOM3D_INCLUDE_DIR}/engine/progressbar.h progressbar.cpp
	${ATOM3D_INCLUDE_DIR}/engine/checkbox.h checkbox.cpp
	${ATOM3D_INCLUDE_DIR}/engine/richedit.h richedit.cpp
	${ATOM3D_INCLUDE_DIR}/engine/cell.h cell.cpp
	${ATOM3D_INCLUDE_DIR}/engine/celldata.h celldata.cpp
	${ATOM3D_INCLUDE_DIR}/engine/widget.h widget.cpp
	${ATOM3D_INCLUDE_DIR}/engine/listbox.h listbox.cpp
	${ATOM3D_INCLUDE_DIR}/engine/menu.h menu.cpp
	${ATOM3D_INCLUDE_DIR}/engine/menubar.h menubar.cpp
	${ATOM3D_INCLUDE_DIR}/engine/desktop.h desktop.cpp
	${ATOM3D_INCLUDE_DIR}/engine/gui_canvas.h gui_canvas.cpp
	${ATOM3D_INCLUDE_DIR}/engine/gui_font.h gui_font.cpp
	${ATOM3D_INCLUDE_DIR}/engine/gui_renderer.h gui_renderer.cpp
	${ATOM3D_INCLUDE_DIR}/engine/gui_events.h
	${ATOM3D_INCLUDE_DIR}/engine/gui_events_fwd.h
	${ATOM3D_INCLUDE_DIR}/engine/gui_misc.h
	${ATOM3D_INCLUDE_DIR}/engine/realtimectrl.h realtimectrl.cpp
	${ATOM3D_INCLUDE_DIR}/engine/imagelist.h imagelist.cpp
	${ATOM3D_INCLUDE_DIR}/engine/framepainter.h framepainter.cpp
	${ATOM3D_INCLUDE_DIR}/engine/dialog.h dialog.cpp
	${ATOM3D_INCLUDE_DIR}/engine/scrollbar.h scrollbar.cpp
	${ATOM3D_INCLUDE_DIR}/engine/treectrl.h treectrl.cpp
	${ATOM3D_INCLUDE_DIR}/engine/circleprogress.h circleprogress.cpp
	${ATOM3D_INCLUDE_DIR}/engine/scrollmap.h scrollmap.cpp
	${ATOM3D_INCLUDE_DIR}/engine/topwindow.h topwindow.cpp
	${ATOM3D_INCLUDE_DIR}/engine/slider.h slider.cpp
	${ATOM3D_INCLUDE_DIR}/engine/vectorgraph.h vectorgraph.cpp
	${ATOM3D_INCLUDE_DIR}/engine/ATOM_WidgetAnimator.h ATOM_WidgetAnimator.cpp
	${ATOM3D_INCLUDE_DIR}/engine/polygondrawer.h polygondrawer.cpp
	${ATOM3D_INCLUDE_DIR}/engine/listview.h listview.cpp
	${ATOM3D_INCLUDE_DIR}/engine/propertylist.h propertylist.cpp 
	${ATOM3D_INCLUDE_DIR}/engine/marquee.h marquee.cpp
	${ATOM3D_INCLUDE_DIR}/engine/hyperlink.h hyperlink.cpp
	${ATOM3D_INCLUDE_DIR}/engine/multiedit.h multiedit.cpp
	${ATOM3D_INCLUDE_DIR}/engine/combobox.h combobox.cpp
	${ATOM3D_INCLUDE_DIR}/engine/flashctrl.h flashctrl.cpp
	${ATOM3D_INCLUDE_DIR}/engine/panel.h panel.cpp
	${ATOM3D_INCLUDE_DIR}/engine/widgetlayout.h widgetlayout.cpp
	${ATOM3D_INCLUDE_DIR}/engine/basiclayout.h basiclayout.cpp
	${ATOM3D_INCLUDE_DIR}/engine/horizontallayout.h horizontallayout.cpp
	${ATOM3D_INCLUDE_DIR}/engine/verticallayout.h verticallayout.cpp
	${ATOM3D_INCLUDE_DIR}/engine/widget_io.h widget_io.cpp
	${ATOM3D_INCLUDE_DIR}/engine/viewstack.h viewstack.cpp
)

ADD_DEFINITIONS(-DBUILD_ATOM_ENGINE)
FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

ADD_LIBRARY(ATOM_engine ${ATOM3D_SHARED} ${EngineSrcHeaders} ${EnginePublicHeaders})
ADD_PRECOMPILED_HEADER(ATOM_engine ${PCH_Header} ${PCH_Source} EngineSrcHeaders)
ADD_DEPENDENCIES(ATOM_engine ATOM_kernel ATOM_geometry ATOM_image ATOM_ttfont ATOM_audio ATOM_thread ATOM_render)
TARGET_LINK_LIBRARIES(ATOM_engine ATOM_kernel ATOM_geometry ATOM_image ATOM_ttfont ATOM_audio ATOM_render imm32)

INSTALL_TARGET(ATOM_engine)
