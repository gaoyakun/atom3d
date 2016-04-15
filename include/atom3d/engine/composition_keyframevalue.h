#ifndef __ATOM3D_ENGINE_COMPOSITION_KEYFRAMEVALUE_H
#define __ATOM3D_ENGINE_COMPOSITION_KEYFRAMEVALUE_H

#include "../ATOM_kernel.h"

class ATOM_CompositionKeyFrameValue: public ATOM_ReferenceObj
{
public:
	void setTranslation (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getTranslation  (void) const;
	void setScale (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getScale (void) const;
	void setRotation (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getRotation (void) const;
	void setVisible (int visible);
	int getVisible (void) const;

private:
	ATOM_Vector3f _translation;
	ATOM_Vector3f _scale;
	ATOM_Vector3f _rotation;
	int _visible;
};

#endif // __ATOM3D_ENGINE_COMPOSITION_KEYFRAMEVALUE_H
