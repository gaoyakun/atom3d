#ifndef __ATOM3D_GFXDRIVER_DRIVERCAPS_H
#define __ATOM3D_GFXDRIVER_DRIVERCAPS_H

class ATOM_GfxDriverCaps
{
public:
	virtual bool supportSRGBTexture (void) const = 0;
	virtual bool supportS3TCCompression (void) const = 0;
	virtual bool supportFloatTexture (void) const = 0;
	virtual unsigned getMaxRenderTargets (void) const = 0;
	virtual unsigned getMaxTextureWidth (void) const = 0;
	virtual unsigned getMaxTextureHeight (void) const = 0;
	virtual unsigned getMaxCubeTextureSize (void) const = 0;
	virtual unsigned getMaxVolumeTextureWidth (void) const = 0;
	virtual unsigned getMaxVolumeTextureHeight (void) const = 0;
	virtual unsigned getMaxVolumeTextureDepth (void) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_DRIVERCAPS_H
