#ifndef __ATOM3D_ENGINE_DRAWABLE_H
#define __ATOM3D_ENGINE_DRAWABLE_H

#include "../ATOM_render.h"
#include "basedefs.h"

#include "renderqueue.h"

class ATOM_Camera;
class ATOM_Material;
class ATOM_RenderQueue;
class ATOM_RenderScheme;

class ATOM_ENGINE_API ATOM_Drawable
{
public:
	ATOM_Drawable(void);
	virtual ~ATOM_Drawable (void);
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material) = 0;
	virtual float computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix) const;

	//--- wangjian added ---//
	virtual bool		drawBatch(	ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, 
									bool bInstancing = true, sInstancing * instancing = 0	);
	virtual void		batching(sInstancing * instanceWrapper, ATOM_Camera *camera);
	virtual void		prepareForSort(ATOM_RenderScheme*);
	virtual void		setupSortFlag(int queueid=-1);

	void				appendEffectId(ATOM_RenderScheme*,int id);
	int					getEffectIdEnd(ATOM_RenderScheme*);
	int					getEffectIdFront(ATOM_RenderScheme*);
	
	/*void				setSortFlag(unsigned sortFlag);
	unsigned			getSortFlag(void) const;*/
	void				setSortFlag(unsigned sf_ae, unsigned sf_mt, unsigned sf_matid);
	unsigned			getSortFlag(unsigned sf) const;

	void				setObjFlag(unsigned of);
	void				resetObjFlag(unsigned of);
	unsigned			getObjFlag(void) const;

	void				setRenderQueueInfo( ATOM_RenderQueue * renderqueue, ATOM_RenderQueue::Entry * index);
	ATOM_RenderQueue::Entry *			getRenderQueueInfo(ATOM_RenderQueue * renderqueue);
	void				removeSelfFromRenderQueue(ATOM_RenderQueue * renderqueue);

	enum eSORTFLAG
	{
		SF_ACTIVEEFFECT		= 1<<0,		// active effect
		SF_MESHORTEXTURE	= 1<<1,		// mesh or texture (or misc stuff)
		SF_MATERIALID		= 1<<2,		// material id( if not set use 0 )
		SF_TOTAL,
	};

	enum eOBJFLAG
	{
		OF_GEOINSTANCING	= 1<<0,		// support geometry instancing
		OF_TOTAL,
	};

protected:
	//ULONGLONG		_sort_flag;

	//unsigned				_sort_flag

	unsigned				_sort_flag_ae;		// active effect sort flag
	unsigned				_sort_flag_mt;		// mesh or texture sort flag
	unsigned				_sort_flag_matid;	// material id sort flag

	unsigned				_obj_flag;			// object flag

	/*struct sRQInfo
	{
		ATOM_RenderQueue *					_renderqueue;
		ATOM_RenderQueue::Entry * 			_rq_entry;
	};
	ATOM_VECTOR<sRQInfo>	_rq_info_vec;*/

	// 用来保存选择渲染队列时得到的EFFECT ID
	struct sCullResult
	{
		ATOM_DEQUE<int>		_effect_cullresult;
	};
	ATOM_MAP<ATOM_RenderScheme*,sCullResult> _cull_results;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_DRAWABLE_H
