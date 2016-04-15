#include "stdafx.h"
#include "basedefs.h"
#include "tree.h"
#include "treeinstance.h"
#include "atmosphere2.h"
#include "sky.h"
#include "decal.h"
#include "linetrail.h"
#include "ribbontrail.h"
#include "material.h"
#include "model2.h"
#include "geode2.h"
#include "bkimage.h"
#include "weapontrail.h"
#include "composition_node.h"
#include "colorgradingeffect.h"
#include "FXAA.h"
//--- wangjian added ---//
#include "postEffectRain.h"
#include "postEffectRainLayer.h"
#include "postEffectSilhouet.h"
#include "postEffectbloom.h"
#include "postEffectGammaCorrect.h"
#include "postEffectHeatHaze.h"
#include "postEffectColorFilter.h"
#include "postEffectDOF.h"
#include "postEffectDepthMask.h"
#include "postEffectSunShaft.h"
#include "postEffectSSAO.h"
#include "ayncLoader.h"
//----------------------//

ATOM_PLUGIN_BEGIN(Engine, "atom3d.engine", 0, 0, 1)

	ATOM_PLUGIN_DEF(Engine, ATOM_Model)
	ATOM_PLUGIN_DEF(Engine, ATOM_SharedModel)

	//Material
	ATOM_PLUGIN_DEF(Engine, ATOM_CoreMaterial)
	//Node
	ATOM_PLUGIN_DEF(Engine, ATOM_Node)
	ATOM_PLUGIN_DEF(Engine, ATOM_NodeOctree)
	ATOM_PLUGIN_DEF(Engine, ATOM_Terrain)
	ATOM_PLUGIN_DEF(Engine, ATOM_Geode)
	ATOM_PLUGIN_DEF(Engine, ATOM_BkImage)
	ATOM_PLUGIN_DEF(Engine, ATOM_ParticleSystem)
	ATOM_PLUGIN_DEF(Engine, ATOM_Hud)
	ATOM_PLUGIN_DEF(Engine, ATOM_GuiHud)
	ATOM_PLUGIN_DEF(Engine, ATOM_Water)
	ATOM_PLUGIN_DEF(Engine, ATOM_LightNode)
	ATOM_PLUGIN_DEF(Engine, ATOM_Atmosphere)
	ATOM_PLUGIN_DEF(Engine, ATOM_Atmosphere2)
	ATOM_PLUGIN_DEF(Engine, ATOM_Sky)
	ATOM_PLUGIN_DEF(Engine, ATOM_Tree)
	ATOM_PLUGIN_DEF(Engine, ATOM_TreeInstance)
	ATOM_PLUGIN_DEF(Engine, ATOM_ShapeNode)
	ATOM_PLUGIN_DEF(Engine, ATOM_WeaponTrail)
	ATOM_PLUGIN_DEF(Engine, ATOM_Decal)
	ATOM_PLUGIN_DEF(Engine, ATOM_LineTrail)
	ATOM_PLUGIN_DEF(Engine, ATOM_RibbonTrail)
	ATOM_PLUGIN_DEF(Engine, ATOM_ActorComponentsPart)
	ATOM_PLUGIN_DEF(Engine, ATOM_ActorColorPart)
	ATOM_PLUGIN_DEF(Engine, ATOM_ActorTexturePart)
	ATOM_PLUGIN_DEF(Engine, ATOM_ActorTransformPart)
	ATOM_PLUGIN_DEF(Engine, ATOM_ActorBindingPart)
	ATOM_PLUGIN_DEF(Engine, CohesiveEmitter)
	ATOM_PLUGIN_DEF(Engine, CylinderEmitter)
	ATOM_PLUGIN_DEF(Engine, RingEmitter)
	ATOM_PLUGIN_DEF(Engine, SphereEmitter)
	ATOM_PLUGIN_DEF(Engine, TaperEmitter)
	ATOM_PLUGIN_DEF(Engine, ATOM_Actor)
	ATOM_PLUGIN_DEF(Engine, ClientSimpleCharacter)
	ATOM_PLUGIN_DEF(Engine, ATOM_ColorGradingEffect)
	ATOM_PLUGIN_DEF(Engine, ATOM_FXAAEffect)
	
	ATOM_PLUGIN_DEF(Engine, ATOM_RainEffect)				// wangjian added 
	ATOM_PLUGIN_DEF(Engine, ATOM_RainLayerEffect)			// wangjian added 
	ATOM_PLUGIN_DEF(Engine, ATOM_SilhouetEffect)			// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_BLOOMEffect)				// wangjian added 
	ATOM_PLUGIN_DEF(Engine, ATOM_GammaCorrectEffect)		// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_HeatHazeEffect)			// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_ColorFilterEffect)			// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_DOFEffect)					// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_SunShaftEffect)			// wangjian added
	ATOM_PLUGIN_DEF(Engine, ATOM_SSAOEffect)				// wangjian added

	ATOM_PLUGIN_DEF(Engine, ATOM_DepthMaskEffect)			// wangjian added 
	ATOM_PLUGIN_DEF(Engine, ATOM_CompositionNode)
	ATOM_PLUGIN_DEF(Engine, ATOM_NodeAttributeFile)			// wangjian added

ATOM_PLUGIN_END

