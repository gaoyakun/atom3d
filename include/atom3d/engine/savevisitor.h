#ifndef __ATOM3D_ENGINE_SAVEVISITOR_H
#define __ATOM3D_ENGINE_SAVEVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "visitor.h"

class ATOM_ENGINE_API ATOM_XmlSaveVisitor: public ATOM_Visitor
{
public:
	typedef bool (__cdecl *FilterFunction) (ATOM_Node *, void *userData);

public:
	ATOM_XmlSaveVisitor (void);
	ATOM_XmlSaveVisitor (ATOM_TiXmlElement *xmlElement);
	virtual ~ATOM_XmlSaveVisitor (void);

public:
	bool savedOK (void) const;
	void setFilter (FilterFunction function, void *userData);

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
	virtual void visit (ATOM_Actor &node);
	virtual void visit (ClientSimpleCharacter &node);

protected:
	virtual void onBeginVisitNodeTree (ATOM_Node &node);
	virtual void onEndVisitNodeTree (ATOM_Node &node);
	virtual void onResetVisitor (void);

protected:
	struct Entry
	{
		ATOM_TiXmlElement *xml;
		bool skipped;
	};
	ATOM_TiXmlElement* _rootXmlElement;
	ATOM_VECTOR<Entry> _xmlElement;
	bool _savedOK;
	FilterFunction _filterFunction;
	void *_filterUserData;
};

#endif // __ATOM3D_ENGINE_SAVEVISITOR_H
