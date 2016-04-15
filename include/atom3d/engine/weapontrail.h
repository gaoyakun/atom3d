#ifndef __ATOM3D_ENGINE_WEAPONTRAIL_H
#define __ATOM3D_ENGINE_WEAPONTRAIL_H

class ATOM_ENGINE_API ATOM_WeaponTrail: public ATOM_VisualNode, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_WeaponTrail, ATOM_WeaponTrail)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_WeaponTrail)

public:
	ATOM_WeaponTrail (void);
	virtual ~ATOM_WeaponTrail (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual void skipClipTest (bool b);
	virtual bool isSkipClipTest (void) const;
	virtual void update (ATOM_Camera *camera);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

	//--- wangjian added ---//
	// 设置绘制排序标记
	virtual void setupSortFlag(int queueid);
protected:
	virtual void resetMaterialDirtyFlag_impl();
	//----------------------//

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);
	virtual void buildBoundingbox (void) const;

public:
	void setStartPointName (const ATOM_STRING &str);
	const ATOM_STRING &getStartPointName (void) const;
	void setTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getTextureFileName (void) const;
	void setEndPointName (const ATOM_STRING &str);
	const ATOM_STRING &getEndPointName (void) const;
	void setResolution (int resolution);
	int getReslolution (void) const;
	void setPersistantOfVision (unsigned timeInMs);
	unsigned getPersistantOfVision (void) const;
	void setColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getColor (void) const;
	void setBlendMode (int mode);
	int getBlendMode (void) const;
	void setPower (float power);
	float getPower (void) const;
	void enable (int b);
	int isEnabled (void) const;
	bool isPlaying (void) const;
	void setTarget (ATOM_Node *node);
	ATOM_Node *getTarget (void) const;
	void reset (void);

private:
	ATOM_STRING _startPointName;
	ATOM_STRING _endPointName;
	ATOM_STRING _textureFileName;
	bool _isValid;
	bool _enabled;
	int _resolution;
	int _blendMode;
	float _power;
	unsigned _persistant;
	ATOM_Node *_weaponNode;
	ATOM_WeakPtrT<ATOM_Node> _targetNode;
	ATOM_Vector4f _color;
	ATOM_Vector3f _trailStart;
	ATOM_Vector3f _trailEnd;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOREF(ATOM_Texture) _texture;

	struct TrailVertex
	{
		ATOM_Vector3f xyz;
		ATOM_ColorARGB color;
		ATOM_Vector2f uv;
	};
	ATOM_VECTOR<TrailVertex> _vertices;
	ATOM_VECTOR<unsigned short> _indices;

	struct Frame
	{
		ATOM_Vector3f translation;
		ATOM_Quaternion rotation;
		ATOM_Vector3f scaling;
		unsigned tick;
	};
	ATOM_VECTOR<Frame> _frames;
};

#endif __ATOM3D_ENGINE_WEAPONTRAIL_H
