#ifndef __ATOM3D_ENGINE_SCRIPTBIND_TRAITS_H
#define __ATOM3D_ENGINE_SCRIPTBIND_TRAITS_H

#include "../ATOM_math.h"
#include "../ATOM_utils.h"
#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "../ATOM_script.h"

#include "widget.h"
#include "topwindow.h"
#include "scrollbar.h"
#include "richedit.h"
#include "realtimectrl.h"
#include "progressbar.h"
#include "multiedit.h"
#include "menubar.h"
#include "menu.h"
#include "marquee.h"
#include "hyperlink.h"
#include "edit.h"
#include "dialog.h"
#include "desktop.h"
#include "combobox.h"
#include "checkbox.h"
#include "button.h"
#include "cell.h"
#include "celldata.h"
#include "treectrl.h"
#include "gui_renderer.h"
#include "gui_font.h"
#include "gui_misc.h"
#include "gui_events.h"
#include "imagelist.h"
#include "listbox.h"
#include "listview.h"
#include "slider.h"
#include "propertylist.h"
#include "scrollmap.h"
#include "components.h"
#include "geode2.h"
#include "actionmixer2.h"
#include "instancemesh.h"
#include "instanceskeleton.h"
#include "instancetrack.h"
#include "skeleton.h"
#include "modelanimationtrack.h"
#include "deferredscene.h"
//#include "tree.h"
#include "water.h"
#include "particlesystem.h"
#include "lightnode.h"
#include "atmosphere.h"
#include "terrain.h"
#include "actor.h"
#include "ribbontrail.h"
#include "camera.h"
#include "hud.h"
#include "guihud.h"
#include "engineapi.h"
#include "scripteventtrigger.h"

ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Vector2f, ATOM_Vector2f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Vector3f, ATOM_Vector3f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Vector4f, ATOM_Vector4f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Matrix3x3f, ATOM_Matrix3x3f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Matrix3x4f, ATOM_Matrix3x4f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Matrix4x4f, ATOM_Matrix4x4f, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_BBox, ATOM_BBox, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Quaternion, ATOM_Quaternion, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_ColorARGB, ATOM_ColorARGB, false)

ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Object, ATOM_Object, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_KernelConfig, ATOM_KernelConfig, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_RenderDevice, ATOM_RenderDevice, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Widget, ATOM_Widget, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Label, ATOM_Label, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_TopWindow, ATOM_TopWindow, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ScrollBar, ATOM_ScrollBar, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_RichEdit, ATOM_RichEdit, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_RealtimeCtrl, ATOM_RealtimeCtrl, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ProgressBar, ATOM_ProgressBar, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_MultiEdit, ATOM_MultiEdit, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_MenuBar, ATOM_MenuBar, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_PopupMenu, ATOM_PopupMenu, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Marquee, ATOM_Marquee, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_HyperLink, ATOM_HyperLink, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Edit, ATOM_Edit, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Dialog, ATOM_Dialog, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Desktop, ATOM_Desktop, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ComboBox, ATOM_ComboBox, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Checkbox, ATOM_Checkbox, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Button, ATOM_Button, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Cell, ATOM_Cell, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_CellData, ATOM_CellData, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_TreeCtrl, ATOM_TreeCtrl, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_TreeItem, ATOM_TreeItem, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_GUIRenderer, ATOM_GUIRenderer, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_GUIFont, ATOM_GUIFont, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_GUIImageList, ATOM_GUIImageList, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ListBox, ATOM_ListBox, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ListView, ATOM_ListView, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Slider, ATOM_Slider, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_FlashCtrl, ATOM_FlashCtrl, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_PropertyList, ATOM_PropertyList, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ScrollMap, ATOM_ScrollMap, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Components, ATOM_Components, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Node, ATOM_Node, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Geode, ATOM_Geode, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ActionMixer2, ATOM_ActionMixer2, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_JointAnimationTrack, ATOM_JointAnimationTrack, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Skeleton, ATOM_Skeleton, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Scene, ATOM_Scene, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_SDLScene, ATOM_SDLScene, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_DeferredScene, ATOM_DeferredScene, false)
//ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Tree, ATOM_Tree, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Water, ATOM_Water, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ParticleSystem, ATOM_ParticleSystem, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_LightNode, ATOM_LightNode, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Terrain, ATOM_Terrain, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Atmosphere, ATOM_Atmosphere, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Actor, ATOM_Actor, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_RibbonTrail, ATOM_RibbonTrail, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_LineTrail, ATOM_LineTrail, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Camera, ATOM_Camera, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Hud, ATOM_Hud, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_GuiHud, ATOM_GuiHud, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Rect2Di, ATOM_Rect2Di, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Point2Di, ATOM_Point2Di, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_Size2Di, ATOM_Size2Di, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_DisplayMode, ATOM_DisplayMode, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_RenderWindowDesc, ATOM_RenderWindowDesc, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Application, ATOM_Application, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_FrameStamp, ATOM_FrameStamp, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_EventTrigger, ATOM_EventTrigger, false)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_ScriptEventTrigger, ATOM_ScriptEventTrigger, true)
ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_Event, ATOM_Event, true)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppWMEvent, ATOM_AppWMEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_MouseMoveEvent, ATOM_MouseMoveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_MouseWheelEvent, ATOM_MouseWheelEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_MouseButtonDownEvent, ATOM_MouseButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_MouseButtonUpEvent, ATOM_MouseButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_KeyDownEvent, ATOM_KeyDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_KeyUpEvent, ATOM_KeyUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WindowResizeEvent, ATOM_WindowResizeEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WindowActiveEvent, ATOM_WindowActiveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CharEvent, ATOM_CharEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WindowMoveEvent, ATOM_WindowMoveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WindowExposeEvent, ATOM_WindowExposeEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WindowCloseEvent, ATOM_WindowCloseEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_DeviceLostEvent, ATOM_DeviceLostEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_DeviceResetEvent, ATOM_DeviceResetEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppIdleEvent, ATOM_AppIdleEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppInitEvent, ATOM_AppInitEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppExitEvent, ATOM_AppExitEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppQuitEvent, ATOM_AppQuitEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_AppActiveEvent, ATOM_AppActiveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetEvent, ATOM_WidgetEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetIdleEvent, ATOM_WidgetIdleEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetCommandEvent, ATOM_WidgetCommandEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_MenuPopupEvent, ATOM_MenuPopupEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetResizeEvent, ATOM_WidgetResizeEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetSetFocusEvent, ATOM_WidgetSetFocusEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetLostFocusEvent, ATOM_WidgetLostFocusEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMouseButtonEvent, ATOM_WidgetMouseButtonEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetLButtonDownEvent, ATOM_WidgetLButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetLButtonUpEvent, ATOM_WidgetLButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMButtonDownEvent, ATOM_WidgetMButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMButtonUpEvent, ATOM_WidgetMButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetRButtonDownEvent, ATOM_WidgetRButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetRButtonUpEvent, ATOM_WidgetRButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMouseMoveEvent, ATOM_WidgetMouseMoveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMouseWheelEvent, ATOM_WidgetMouseWheelEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMouseEnterEvent, ATOM_WidgetMouseEnterEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetMouseLeaveEvent, ATOM_WidgetMouseLeaveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetKeyEvent, ATOM_WidgetKeyEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetKeyDownEvent, ATOM_WidgetKeyDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetKeyUpEvent, ATOM_WidgetKeyUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_WidgetCharEvent, ATOM_WidgetCharEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_TopWindowCloseEvent, ATOM_TopWindowCloseEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellLButtonDownEvent, ATOM_CellLButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellLButtonUpEvent, ATOM_CellLButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellLeftClickEvent, ATOM_CellLeftClickEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellRButtonDownEvent, ATOM_CellRButtonDownEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellRButtonUpEvent, ATOM_CellRButtonUpEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellRightClickEvent, ATOM_CellRightClickEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellMouseEnterEvent, ATOM_CellMouseEnterEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_CellMouseLeaveEvent, ATOM_CellMouseLeaveEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_ListBoxClickEvent, ATOM_ListBoxClickEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_ScrollEvent, ATOM_ScrollEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_EditSysKeyEvent, ATOM_EditSysKeyEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_EditTextChangedEvent, ATOM_EditTextChangedEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_DialogInitEvent, ATOM_DialogInitEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(AOTM_FlashCallEvent, ATOM_FlashCallEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_FlashFSCommandEvent, ATOM_FlashFSCommandEvent, false)
ATOM_SCRIPT_DECLARE_TYPE_TRAITS(ATOM_RenderSettings, ATOM_RenderSettings, true)

ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_LOGGER, ATOM_LOGGER, true)

#endif // __ATOM3D_ENGINE_SCRIPTBIND_TRAITS_H
