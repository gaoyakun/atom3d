#ifndef ATOM_ENGINE_H_
#define ATOM_ENGINE_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "ATOM_basedefs.h"

//draw
#include "engine/camera.h"
#include "engine/rendersettings.h"
#include "engine/renderqueue.h"
#include "engine/grayscaleeffect.h"
#include "engine/colorgradingeffect.h"
#include "engine/posteffectDOF.h"
#include "engine/FXAA.h"
//--- wangjian added ---//
#include "engine/postEffectbloom.h"
//----------------------//
#include "engine/model.h"
#include "engine/model2.h"
#include "engine/sdlscene.h"
#include "engine/deferredscene.h"
#include "engine/geometry.h"
#include "engine/deferredrenderscheme.h"
#include "engine/deferred_cullvisitor.h"

//animations
#include "engine/joint_animation_track.h"
#include "engine/skinanim_sse.h"
#include "engine/components.h"

//nodes	
#include "engine/nodeoctree.h"
#include "engine/terrain.h"
#include "engine/heightfield.h"
#include "engine/cullvisitor.h"
#include "engine/updatevisitor.h"
#include "engine/savevisitor.h"
#include "engine/pickvisitor.h"
#include "engine/effectstatevisitor.h"
#include "engine/light.h"
#include "engine/decal.h"
#include "engine/geode2.h"
#include "engine/actionmixer2.h"
#include "engine/particlesystem.h"
#include "engine/hud.h"
#include "engine/guihud.h"
#include "engine/water.h"
#include "engine/lightnode.h"
#include "engine/atmosphere.h"
#include "engine/atmosphere2.h"
#include "engine/shape.h"
#include "engine/bkimage.h"
#include "engine/actor.h"
#include "engine/clientsimplecharacter.h"
#include "engine/ribbontrail.h"
#include "engine/sky.h"
#include "engine/weapontrail.h"
#include "engine/composition_track.h"
#include "engine/composition_node.h"

//gui
#include "engine/widget.h"
#include "engine/topwindow.h"
#include "engine/dialog.h"
#include "engine/button.h"
#include "engine/realtimectrl.h"
#include "engine/gui_renderer.h"
#include "engine/desktop.h"
#include "engine/gui_events.h"
#include "engine/menu.h"
#include "engine/menubar.h"
#include "engine/slider.h"
#include "engine/imagelist.h"
#include "engine/scrollbar.h"
#include "engine/checkbox.h"
#include "engine/label.h"
#include "engine/marquee.h"
#include "engine/edit.h"
#include "engine/progressbar.h"
#include "engine/circleprogress.h"
#include "engine/richedit.h"
#include "engine/cell.h"
#include "engine/listbox.h"
#include "engine/viewstack.h"
#include "engine/scrollmap.h"
#include "engine/listview.h"
#include "engine/propertylist.h"
#include "engine/treectrl.h"
#include "engine/multiedit.h"
#include "engine/hyperlink.h"
#include "engine/combobox.h"
#include "engine/curveeditor.h"
#include "engine/flashctrl.h"
#include "engine/panel.h"
#include "engine/widgetlayout.h"
#include "engine/basiclayout.h"
#include "engine/verticallayout.h"
#include "engine/horizontallayout.h"
#include "engine/gui_canvas.h"
#include "engine/widget_io.h"

//misc
#include "engine/async_loader.h"
#include "engine/engineapi.h"
#include "engine/gameentity.h"
#include "engine/timevalue.h"
#include "engine/timemanager.h"
#include "engine/path.h"

#include "engine/ParticleEmitter.h"
#include "engine/CohesiveEmitter.h"
#include "engine/CylinderEmitter.h"
#include "engine/RingEmitter.h"
#include "engine/SphereEmitter.h"
#include "engine/TaperEmitter.h"

#include "engine/scripteventtrigger.h"

//flash
//#include "engine/delegate.h"
#include "engine/flashcontrol.h"
#include "engine/flashvalue.h"
#include "engine/position.h"
#include "engine/renderbuffer.h"

#endif//ATOM_ENGINE_H_
