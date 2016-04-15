#ifndef __ATOM3D_ENGINE_TRANSFORM_KEYFRAME_H
#define __ATOM3D_ENGINE_TRANSFORM_KEYFRAME_H

class ATOM_ENGINE_API ATOM_TranslateKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_Vector3f>
{
public:
	ATOM_TranslateKeyFrame (void);
	ATOM_TranslateKeyFrame (const ATOM_Vector3f &t);

	virtual void visit (ATOM_Node &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_TranslateKeyFrame, ATOM_Vector3f)

class ATOM_ENGINE_API ATOM_ScaleKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_Vector3f>
{
public:
	ATOM_ScaleKeyFrame (void);
	ATOM_ScaleKeyFrame (const ATOM_Vector3f &t);

	virtual void visit (ATOM_Node &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_ScaleKeyFrame, ATOM_Vector3f)

class ATOM_ENGINE_API ATOM_RotateKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_Vector3f>
{
public:
	ATOM_RotateKeyFrame (void);
	ATOM_RotateKeyFrame (const ATOM_Vector3f &r);

	virtual void visit (ATOM_Node &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_RotateKeyFrame, ATOM_Vector3f)

class ATOM_ENGINE_API ATOM_AlphaKeyFrame: public ATOM_CompositionKeyFrameT<float>
{
public:
	ATOM_AlphaKeyFrame (void);
	ATOM_AlphaKeyFrame (float alpha);

public:
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_AlphaKeyFrame, float)

#endif // __ATOM3D_ENGINE_TRANSFORM_KEYFRAME_H
