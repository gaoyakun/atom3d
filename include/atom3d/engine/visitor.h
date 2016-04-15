#ifndef __ATOM3D_ENGINE_VISITOR_H
#define __ATOM3D_ENGINE_VISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

class ATOM_Node;
class ATOM_NodeOctree;
class ATOM_Terrain;
class ATOM_VisualNode;
class ATOM_BaseGeode;
class ATOM_Geode;
class ATOM_ParticleSystem;
class ATOM_Hud;
class ATOM_GuiHud;
class ATOM_Water;
class ATOM_LightNode;
class ATOM_Atmosphere;
class ATOM_Atmosphere2;
class ATOM_Actor;
class ATOM_Decal;
class ATOM_Sky;
class ATOM_BkImage;
class ATOM_WeaponTrail;
class ATOM_RibbonTrail;
//class SwordEffect;
class ClientSimpleCharacter;
class ATOM_CompositionNode;
class ATOM_ShapeNode;

class ATOM_ENGINE_API ATOM_Visitor
{
public:
	ATOM_Visitor (void);
	virtual ~ATOM_Visitor (void);

public:
	void cancelTraverse (void);
	bool traverseCanceled (void) const;
	void skipChildren (void);
	bool childrenSkipped (void) const;
	void reset (void);

public:
	virtual void traverse (ATOM_Node &node);
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_NodeOctree &node);
	virtual void visit (ATOM_Terrain &node);
	virtual void visit (ATOM_VisualNode &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_Hud &node);
	virtual void visit (ATOM_GuiHud &node);
	virtual void visit (ATOM_Water &node);
	virtual void visit (ATOM_LightNode &node);
	virtual void visit (ATOM_Atmosphere &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);
	virtual void visit (ATOM_BkImage &node);
	virtual void visit (ATOM_Actor &node);
	virtual void visit (ATOM_Decal &node);
	virtual void visit (ATOM_CompositionNode &node);
	virtual void visit (ATOM_ShapeNode &node);
	virtual void visit (ATOM_RibbonTrail &node);
	virtual void visit (ATOM_WeaponTrail &node);
	virtual void visit (ClientSimpleCharacter &node);
	//virtual void visit (SwordEffect &node);

public:
	void traverse_R (ATOM_Node &node);

protected:
	virtual void onBeginVisitNodeTree (ATOM_Node &node);
	virtual void onEndVisitNodeTree (ATOM_Node &node);
	virtual void onResetVisitor (void);

private:
	bool _cancelTraverse;
	bool _skipChildren;
	//typedef ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> NodePath;
	//NodePath _nodePath;
	//ATOM_AUTOREF(ATOM_Node) _rootNode;
};

#endif // __ATOM3D_ENGINE_VISITOR_H
