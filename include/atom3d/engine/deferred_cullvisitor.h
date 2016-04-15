#ifndef __ATOM3D_ENGINE_DEFERRED_CULLVISITOR_H
#define __ATOM3D_ENGINE_DEFERRED_CULLVISITOR_H

#include "cullvisitor.h"

class ATOM_DeferredRenderScheme;

class ATOM_ENGINE_API ATOM_DeferredCullVisitor: public ATOM_CullVisitor
{
public:
	ATOM_DeferredCullVisitor (ATOM_DeferredRenderScheme *deferredRenderScheme);
	virtual ~ATOM_DeferredCullVisitor (void);

public:
	virtual void visit (ATOM_LightNode &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);

private:
	ATOM_DeferredRenderScheme *_deferredRenderScheme;
};

#endif // __ATOM3D_ENGINE_DEFERRED_CULLVISITOR_H
