#ifndef __ATOM3D_ENGINE_BKIMAGE_H
#define __ATOM3D_ENGINE_BKIMAGE_H

#include "basedefs.h"
#include "node.h"
#include "drawable.h"

class ATOM_ENGINE_API ATOM_BkImage: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS (engine, ATOM_BkImage, ATOM_BkImage)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_BkImage)

public:
	ATOM_BkImage (void);
	virtual ~ATOM_BkImage (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual const ATOM_Matrix4x4f &getWorldMatrix (void) const;

protected:
	virtual void buildBoundingbox(void) const;
	virtual void buildWorldMatrix (void) const;
	virtual bool onLoad(ATOM_RenderDevice *pDevice);

public:
	void setImageFile (const ATOM_STRING &filename);
	const ATOM_STRING &getImageFile (void) const;
	void setTexture (ATOM_Texture *texture);
	ATOM_Texture *getTexture (void) const;
	ATOM_Material *getMaterial(void) const;

private:
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOREF(ATOM_Texture) _imageTexture;
	ATOM_STRING _imageFileName;
	bool _imageChanged;
};

#endif // __ATOM3D_ENGINE_BKIMAGE_H
