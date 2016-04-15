#ifndef __ATOM3D_ENGINE_EFFECTSTATEVISITOR_H
#define __ATOM3D_ENGINE_EFFECTSTATEVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "visitor.h"

class ATOM_ENGINE_API ATOM_EffectStateVisitor: public ATOM_Visitor
{
public:
	ATOM_EffectStateVisitor (void);

public:
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ClientSimpleCharacter &node);
	virtual void visit (ATOM_CompositionNode &node);

public:
	unsigned getEffectStartTick (void) const;

private:
	unsigned _starttick;
};

class ATOM_ENGINE_API ATOM_EffectResetVisitor: public ATOM_Visitor
{
public:
	ATOM_EffectResetVisitor (void);

public:
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ClientSimpleCharacter &node);
	//virtual void visit (SwordEffect &node);
};

#endif // __ATOM3D_ENGINE_EFFECTSTATEVISITOR_H
