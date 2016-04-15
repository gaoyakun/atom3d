#ifndef __ATOM3D_ENGINE_COMMON_KEYFRAME_H
#define __ATOM3D_ENGINE_COMMON_KEYFRAME_H

class ATOM_ENGINE_API ATOM_TransparencyKeyFrame: public ATOM_CompositionKeyFrameT<float>
{
public:
	ATOM_TransparencyKeyFrame (void);
	ATOM_TransparencyKeyFrame (float transparency);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_ShapeNode &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_TransparencyKeyFrame, float)

#endif // __ATOM3D_ENGINE_COMMON_KEYFRAME_H

