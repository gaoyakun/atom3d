#ifndef __ATOM3D_ENGINE_VISIBLE_KEYFRAME_H
#define __ATOM3D_ENGINE_VISIBLE_KEYFRAME_H

class ATOM_ENGINE_API ATOM_VisibleKeyFrame: public ATOM_CompositionKeyFrameT<int>
{
public:
	ATOM_VisibleKeyFrame (void);
	ATOM_VisibleKeyFrame (int visible);

	virtual void visit (ATOM_Node &node);

private:
	ATOM_AUTOREF(ATOM_Node) _parent;
	ATOM_AUTOREF(ATOM_Node) _node;
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_VisibleKeyFrame, int)

#endif // __ATOM3D_ENGINE_VISIBLE_KEYFRAME_H
