#include "StdAfx.h"
#include "effectstatevisitor.h"
#include "actionmixer2.h"

ATOM_EffectStateVisitor::ATOM_EffectStateVisitor (void)
{
	_starttick = 0;
}

void ATOM_EffectStateVisitor::visit (ATOM_Node &node)
{
	_starttick = 0;
}

void ATOM_EffectStateVisitor::visit (ATOM_Geode &node)
{
	unsigned tick1 = node.getActionMixer()->getUpsideStartTick();
	unsigned tick2 = node.getActionMixer()->getDownsideStartTick();
	if (tick1 && tick2)
	{
		_starttick = ATOM_max2(tick1, tick2);
	}
	else
	{
		_starttick = 0;
	}
}

void ATOM_EffectStateVisitor::visit (ATOM_CompositionNode &node)
{
	_starttick = node.getStartTick ();
}

void ATOM_EffectStateVisitor::visit (ATOM_ParticleSystem &node)
{
	_starttick = node.getStartTick ();
}

void ATOM_EffectStateVisitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}
/*
void ATOM_EffectStateVisitor::visit (SwordEffect &node)
{
	_starttick = node.getStartTick ();
}
*/
unsigned ATOM_EffectStateVisitor::getEffectStartTick (void) const
{
	return _starttick;
}

////////////////////////////////////////////////////////////////////////////////////////////

ATOM_EffectResetVisitor::ATOM_EffectResetVisitor (void)
{
}

void ATOM_EffectResetVisitor::visit (ATOM_Node &node)
{
}

void ATOM_EffectResetVisitor::visit (ATOM_Geode &node)
{
	node.resetActions (ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE);
}

void ATOM_EffectResetVisitor::visit (ATOM_ParticleSystem &node)
{
	node.reset (false);
}

void ATOM_EffectResetVisitor::visit (ClientSimpleCharacter &node)
{
	// reset children
	int nCount = node.getEquipCount();
	for(int i=0; i<nCount; ++i)
	{
		ATOM_EffectResetVisitor v;
		ATOM_Node *p = node.getEquipNode(i);
		if (p)
		{
			p->accept (v);
		}
	}

	visit ((ATOM_Geode&)node);
}
/*
void ATOM_EffectResetVisitor::visit (SwordEffect &node)
{
	node.reset ();
}
*/
