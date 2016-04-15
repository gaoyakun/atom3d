#ifndef __ATOM3D_ENGINE_SDLSCENE_H
#define __ATOM3D_ENGINE_SDLSCENE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "../ATOM_math.h"
#include "scene.h"
#include "light.h"
#include "timevalue.h"
#include "savevisitor.h"

class ATOM_Camera;

// Single Directional Light Scene
class ATOM_ENGINE_API ATOM_SDLScene: public ATOM_Scene
{
public:
	ATOM_SDLScene (void);
	virtual ~ATOM_SDLScene (void);

public:
	ATOM_Light *getLight (void) const;
	void setLightDirKey (int time, const ATOM_Vector3f &dir);
	const ATOM_Vector3f &getLightDirKey (int time) const;
	void setLightColorKey (int time, const ATOM_Vector4f &color);
	const ATOM_Vector4f &getLightColorKey (int time) const;
	void setGlobalLightColorKey (int time, const ATOM_Vector4f &color);
	const ATOM_Vector4f &getGlobalLightColorKey (int time) const;
	void setBloomThreshold (float value);
	float getBloomThreshold (void) const;
	void setBloomOffset (float value);
	float getBloomOffset (void) const;
	//--- wangjian added ---//
	void setBloomMultiplier(float value);
	float getBloomMultiplier(void) const;
	void setHDRFilmCurveParams(float filmCurveShoulder,float filmCurveMiddleTone,float filmCurveToe, float filmCurveWhitePoint);
	void getHDRFilmCurveParams(float & filmCurveShoulder,float & filmCurveMiddleTone,float & filmCurveToe, float & filmCurveWhitePoint) const;
	void setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance);
	void getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const;
	//----------------------//
	void setSplitLambda (float value);
	float getSplitLambda (void) const;
	void setShadowDistance (float value);
	float getShadowDistance (void) const;
	void setLightDistance (float value);
	float getLightDistance (void) const;
	void setSceneScale (float value);
	float getSceneScale (void) const;
	void setLightSize (float value);
	float getLightSize (void) const;
	void setDepthBias (float value);
	float getDepthBias (void) const;
	void setShadowFadeMin (float value);
	float getShadowFadeMin (void) const;
	void setShadowFadeMax (float value);
	float getShadowFadeMax (void) const;
	//--- wangjian added ---//
	// 阴影相关
	void setShadowBlur(float value);
	float getShadowBlur(void) const;
	void setHDRToggle(bool toggle);
	bool getHDRToggle(void) const;
	void setShadowSlopeBias(float value);
	float getShadowSlopeBias(void) const;
	void setShadowDensity(float value);
	float getShadowDensity(void) const;
	void setComplememtLightParams(	const ATOM_Vector3f	&	lightDir,
									const ATOM_Vector4f	&	lightColor,
									const float				lightIntensity	);
	void getComplementLightParams(	ATOM_Vector3f	&	lightDir,
									ATOM_Vector4f	&	lightColor,
									float			&	lightIntensity	) const;
	//----------------------//
	ATOM_BBox getPrecalculatedBoundingBox (void) const;
	ATOM_BBox calculateBoundingBox (void) const;
	void setFixedCameraVector (const ATOM_Vector3f &v);
	void setFixedCameraPosition (const ATOM_Vector3f &p);
	void setPerspectiveParams (float fov, float nearPlane, float farPlane);
	const ATOM_Vector3f &getFixedCameraVector (void) const;
	const ATOM_Vector3f &getFixedCameraPosition (void) const;
	void setCameraMatrix (const ATOM_Matrix4x4f &m);
	const ATOM_Matrix4x4f *getCameraMatrix (void) const;
	float getFOVY (void) const;
	float getNearPlane (void) const;
	float getFarPlane (void) const;

public:
	virtual bool save (const char *filename, ATOM_Node *parent = 0, ATOM_XmlSaveVisitor::FilterFunction filterFunction = 0, void *filterUserData = 0);
	virtual bool load (const char *filename, ATOM_Node *parent = 0, ATOM_Scene::LoadCallback callback = 0, void *userdata = 0);
	virtual void render (ATOM_RenderDevice *device, bool clear);
	

protected:
	ATOM_AUTOPTR(ATOM_Light) _directionalLight;
	ATOM_TimeValueT<ATOM_Vector3f> _lightDir;
	ATOM_TimeValueT<ATOM_Vector4f> _lightColor;
	ATOM_TimeValueT<ATOM_Vector4f> _lightFactors;
	float _bloomThreshold;
	float _bloomOffset;
	//--- wangjian added ---//
	float _bloomMultiplier;
	// HDR相关
	bool			_hdr_enable;
	float			_filmCurveShoulder;
	float			_filmCurveMiddleTone;
	float			_filmCurveToe;
	float			_filmCurveWhitePoint;
	float			_HDRSaturate;
	float			_HDRContrast;
	ATOM_Vector3f	_HDRColorBalance;

	// 补光参数
	ATOM_Vector3f							_complement_lightDir;
	ATOM_Vector4f							_complement_lightColor;
	float									_complement_lightIntensity;
	//----------------------//
	float _splitLambda;
	float _shadowDistance;
	float _lightDistance;
	float _sceneScale;
	float _lightSize;
	float _depthBias;
	bool _manageColorGrading;

	mutable ATOM_BBox _sceneBoundingBox;
	mutable bool _sceneBBoxCalculated;
	ATOM_Vector3f _fixedCameraDirection;
	ATOM_Vector3f _fixedCameraPosition;
	ATOM_Matrix4x4f *_cameraMatrix;
	float _FOV;
	float _nearPlane;
	float _farPlane;
	float _fadeMin;
	float _fadeMax;
	//--- wangjian added ---//
	// 阴影相关
	float _shadowBlur;
	float _shadowSlopeBias;
	float _shadowDensity;
	//----------------------//

	//--- wangjian added ---//
public:
	typedef void (*SceneAsyncLoadCallback) (void *);
	struct SceneAsyncLoadCallbackInfo
	{
		SceneAsyncLoadCallback callback;
		void *userdata;
	}_sceneAsyncLoadCallBackInfo;
	//----------------------//
	void SetLoadAsyncCallBack(SceneAsyncLoadCallback,void* userdata);
	bool IsAsyncLoad();
	//ATOM_TiXmlDocument			doc;
	//ATOM_TiXmlElement *			blocks_element;
	//ATOM_DEQUE<unsigned int>	blocks_r;
	//ATOM_HARDREF(ATOM_Node)		block_node_root;
	bool						bAsyncLoad;

	//--- use for test node count ---//
	static void appendNode(ATOM_Node* node);
	static void collectSceneNodeStatistics(ATOM_Scene * scene);
	static void dumpSceneNodeStatistics();
	static void drawSceneNode(ATOM_Scene * scene, ATOM_STRING nodeFileName = "", bool draw = true);
	struct sCategory
	{
		struct sGroup
		{
			ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _nodes;
		};
		ATOM_MAP<ATOM_STRING,sGroup> _groups;
		void append(ATOM_Node* node)
		{
			if( node->getNodeFileName().empty() )
				return;

			ATOM_MAP<ATOM_STRING,sGroup>::iterator iter = _groups.find(node->getNodeFileName());
			if( iter != _groups.end() )
			{
				sGroup & group = iter->second;
				group._nodes.push_back(node);
			}
			else
			{
				sGroup group;
				group._nodes.push_back(node);
				_groups.insert( std::make_pair( node->getNodeFileName(),group ) );
			}
		}
		void dump()
		{
			ATOM_MAP<ATOM_STRING,sGroup>::iterator iter_group = _groups.begin();
			for(;iter_group!=_groups.end();++iter_group)
			{
				sGroup & group = iter_group->second;
				ATOM_LOGGER::log("%s : %u \n",iter_group->first.c_str(),(unsigned)group._nodes.size() );
			}
		}
	};
	static ATOM_MAP<ATOM_STRING,sCategory>	g_SceneNodesCategory;
	
	//----------------------//
};

#endif // __ATOM3D_ENGINE_SDLSCENE_H
