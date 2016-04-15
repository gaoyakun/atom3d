#ifndef __ATOM3D_ENGINE_BLOCKSAVEVISITOR_H
#define __ATOM3D_ENGINE_BLOCKSAVEVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "visitor.h"

class ATOM_ENGINE_API ATOM_XmlBlockSaveVisitor: public ATOM_Visitor
{
public:
	typedef bool (__cdecl *FilterFunction) (ATOM_Node *, void *userData);

public:
	ATOM_XmlBlockSaveVisitor (void);
	ATOM_XmlBlockSaveVisitor (ATOM_TiXmlElement *xmlElement, ATOM_SDLScene* scene);
	virtual ~ATOM_XmlBlockSaveVisitor (void);

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

	// added 
	void InsertNodeToBlock( ATOM_Node &node );

protected:
	struct Entry
	{
		ATOM_TiXmlElement *xml;
		bool skipped;
		//-- added --//
		bool bBLocked;
		//-----------//
	};
	ATOM_TiXmlElement* _rootXmlElement;
	ATOM_VECTOR<Entry> _xmlElement;
	bool _savedOK;
	FilterFunction _filterFunction;
	void *_filterUserData;

	struct block
	{
		ATOM_Vector2f min_corner;
		ATOM_Vector2f max_corner;
		block() : min_corner(ATOM_Vector2f(0)),max_corner(ATOM_Vector2f(0)){}
	};
	static block _worldBlock;
	static float _blockMinSize;
	//static block* 		 _blocks;
	static unsigned int _block_num_x;
	static unsigned int _block_num_z;
	static unsigned int _block_total_count;

	/*static ATOM_SET<ATOM_Node*> _blocked_nodes;
	static bool CheckAncesterInBlock(ATOM_Node*);*/
};

#endif // __ATOM3D_ENGINE_BLOCKSAVEVISITOR_H
