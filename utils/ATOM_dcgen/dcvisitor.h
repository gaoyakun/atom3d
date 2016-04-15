#ifndef __ATOM_TOOLS_DCVISITOR_H
#define __ATOM_TOOLS_DCVISITOR_H

#include <ATOM.h>
#include "lmgen.h"
#include "dcgen.h"
#include "misc.h"

class DC_CullVisitor: public ATOM_CullVisitor
{
public:
	enum CullMask
	{
		CM_BILLBOARD		= (1<<0),
		CM_DYNAMICSKY		= (1<<1),
		CM_PARTICLESYSTEM	= (1<<2),
		CM_HUD				= (1<<3),
		CM_GUIHUD			= (1<<4),
		CM_TERRAIN			= (1<<5),
		CM_GEODE			= (1<<6),
		CM_GEOMETRY			= CM_BILLBOARD|CM_DYNAMICSKY|CM_PARTICLESYSTEM|CM_TERRAIN|CM_GEODE
	};

public:
	unsigned mask;

public:
	virtual void visit (ATOM_Billboard &node)
	{
		if (mask & CM_BILLBOARD)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_DSky &node)
	{
		if (mask & CM_DYNAMICSKY)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_ParticleSystem &node)
	{
		if (mask & CM_PARTICLESYSTEM)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_Hud &node)
	{
		if (mask & CM_HUD)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_GuiHud &node)
	{
		if (mask & CM_HUD)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_Terrain &node)
	{
		if (mask & CM_TERRAIN)
		{
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_Geode &node)
	{
		if (mask & CM_GEODE)
		{
			ATOM_CullVisitor::visit (node);
		}
	}
};

class GeodeSelectVisitor: public ATOM_CullVisitor
{
public:
	GeodeSelectVisitor (ATOM_Geode *p): geode(p) {}
	ATOM_Geode *geode;
public:
	virtual void visit (ATOM_Billboard &node) {}
	virtual void visit (ATOM_DSky &node) {}
	virtual void visit (ATOM_ParticleSystem &node) {}
	virtual void visit (ATOM_Hud &node) {}
	virtual void visit (ATOM_GuiHud &node) {}
	virtual void visit (ATOM_Terrain &node) {}
	virtual void visit (ATOM_Geode &node)
	{
		if (&node == geode)
		{
			ATOM_CullVisitor::visit (node);
			cancelTraverse ();
		}
	}
};

class FindTerrainVisitor: public ATOM_Visitor
{
public:
	ATOM_AUTOREF(ATOM_Terrain) terrain;

public:
	virtual void visit (ATOM_Terrain &node)
	{
		terrain = &node;
	}
};

#endif // __ATOM_TOOLS_DCVISITOR_H
