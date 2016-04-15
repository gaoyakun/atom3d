#ifndef __ATOM3D_STUDIO_EDITORRENDERSCHEME_H
#define __ATOM3D_STUDIO_EDITORRENDERSCHEME_H

#include <ATOM_engine.h>

class EditorRenderScheme: public ATOM_RenderScheme
{
public:
	static const int RenderScheme_TerrainShadowMap = 0;
	static const int RenderScheme_EditorRenderSchemeCount = 1;

public:
	EditorRenderScheme (ATOM_RenderSchemeFactory *factory);
	virtual ~EditorRenderScheme (void);

public:
	virtual const char *getName (void) const;
	virtual bool init (ATOM_RenderDevice *device, unsigned width, unsigned height);
	virtual bool fini (ATOM_RenderDevice *device);
	virtual unsigned getWidth (void) const;
	virtual unsigned getHeight (void) const;
	virtual void setLightDirection (const ATOM_Vector3f &dir);
	virtual void setOutputImage (ATOM_BaseImage *image);
	virtual ATOM_BaseImage *getOutputImage (void) const;
	virtual ATOM_Texture *getUVNormalTexture (void) const;

protected:
	virtual bool onRender (ATOM_RenderDevice *device, ATOM_Scene *scene);
	virtual int onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer);
	virtual void onResize (ATOM_RenderDevice *device, unsigned width, unsigned height);

private:
	void drawShadowMap (ATOM_RenderDevice *device, ATOM_Scene *scene);

private:
	ATOM_RenderQueue *_renderQueue[RenderScheme_EditorRenderSchemeCount];
	unsigned _currentViewportWidth;
	unsigned _currentViewportHeight;
	bool _isOk;
	ATOM_AUTOREF(ATOM_Texture) _uvNormalTexture;
	ATOM_BaseImage *_outputImage;
	ATOM_Vector3f _currentLightDir;
};

class EditorRenderSchemeFactory: public ATOM_RenderSchemeFactory
{
public:
	EditorRenderSchemeFactory (void);
	virtual ~EditorRenderSchemeFactory (void);
	virtual ATOM_RenderScheme *create (void);
	virtual void destroy (ATOM_RenderScheme *renderScheme);
	virtual int getLayerNumber (const char *layerName) const;
	virtual const char *getLayerName (int layerNumber) const;
	//--- wangjian added ---//
	virtual unsigned getLayerCount() const;
	virtual unsigned getLayerMaskId( int layer ) const;
	//----------------------//
};

#endif // __ATOM3D_STUDIO_EDITORRENDERSCHEME_H
