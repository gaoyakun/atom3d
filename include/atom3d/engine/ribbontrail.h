#ifndef __ATOM3D_ENGINE_RIBBONTRAIL_H
#define __ATOM3D_ENGINE_RIBBONTRAIL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "linetrail.h"

class ATOM_ENGINE_API ATOM_RibbonTrail: public ATOM_LineTrail
{
public:
	ATOM_RibbonTrail (void);
	virtual ~ATOM_RibbonTrail (void);

public:
	class Controller
	{
	public:
		virtual void onHitTarget (ATOM_RibbonTrail *ribbon) = 0;
		virtual void onCollapsed (ATOM_RibbonTrail *ribbon) = 0;
	};

public:
	virtual void update (ATOM_Camera *camera);

protected:
	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad(ATOM_RenderDevice *device);

	//! \copydoc ATOM_Node::assign
	virtual void assign(ATOM_Node *other) const;

public:
	void setLength (float length);
	float getLength (void) const;
	void setNumSegments (unsigned numSegments);
	unsigned getNumSegments (void) const;
	void setTarget (ATOM_Node *target, float timeInSecond);
	void setController (Controller *controller);
	Controller *getController (void) const;
	void setCollapseTime (float timeInSecond);
	float getCollapseTime (void) const;
	void reset (void);
	bool save (const char *filename);

private:
	void updateInflectionPoints (ATOM_Camera *camera, float timeElapsedInSecond);
	void computePointParams (ATOM_LineTrail::InflectionPoint *p, float pos) const;

public:
	void setStartColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getStartColor (void) const;
	void setEndColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getEndColor (void) const;
	void setStartWidth (float val);
	float getStartWidth (void) const;
	void setEndWidth (float val);
	float getEndWidth (void) const;

private:
	ATOM_Vector4f _colorStart, _colorEnd;
	float _widthStart, _widthEnd;
	float _trailLengthDesired;
	unsigned _numSegments;
	bool _needUpdate;
	float _targetHitTimeDesired;
	float _targetHitTime;
	float _collapseTimeDesired;
	float _collapseTime;
	float _minSegmentLength;
	unsigned _starttick;
	ATOM_WeakPtrT<ATOM_Node> _target;
	Controller *_controller;

	ATOM_CLASS(engine, ATOM_RibbonTrail, ATOM_RibbonTrail)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_RibbonTrail)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_RibbonTrail, ATOM_RibbonTrail, "rbt", "ATOM3D ribbon trail")
};

#endif // __ATOM3D_ENGINE_RIBBONTRAIL_H
