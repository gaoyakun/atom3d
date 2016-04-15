/**	\file renderqueue.h
 *	渲染队列类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_RENDERQUEUE_H
#define __ATOM3D_ENGINE_RENDERQUEUE_H

#include "basedefs.h"
//#include "drawable.h"
#include "parametertable.h"

class ATOM_Camera;
class ATOM_RenderScheme;
class ATOM_Drawable;

class sInstancing
{
	friend class ATOM_Drawable;

public:
	struct sInstanceAttribute
	{
		ATOM_Matrix3x4f _world_matrix;				// 世界变换矩阵
	};

	sInstancing();
	~sInstancing();
	void beginBatching(ATOM_Drawable * drawable, ATOM_Material * material);
	bool isBatching();
	void endBatching();
	void batching(ATOM_Drawable* drawable,ATOM_Camera *camera);
	void drawing(ATOM_RenderDevice *device, ATOM_Camera *camera);
	unsigned getIntanceCount();
	void * getInstancedata();
	void setInstanceCountThreshold(unsigned countThreshold);
	bool isExceedThreshold();
	void appendInstance( const ATOM_Matrix4x4f & worldMatrix );

#if 0
	ATOM_Drawable *					_drawable;		//
#else
	ATOM_VECTOR<ATOM_Drawable *>	_drawables;		//
#endif
	ATOM_AUTOPTR(ATOM_Material)		_material;
	ATOM_VECTOR<sInstanceAttribute> _instance_data;
	bool							_batching;
	unsigned						_instanceCountThreshold;
};

//! \class ATOM_RenderQueue
//!	渲染队列类.
//! 可以将可渲染对象(ATOM_Drawable)加入到此队列，此队列可以执行分组，排序和渲染
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_RenderQueue
{
public:
	struct Entry
	{
		ATOM_Drawable				*drawable;
		ATOM_AUTOPTR(ATOM_Material) material;
		ATOM_Matrix4x4f				worldMatrix;
		float						distanceToCamera;
		bool						projectable;
		
		//--- wangjian added ---//
		/*unsigned					sort_flag;*/

		unsigned					sort_flag_ae;
		unsigned					sort_flag_mt;
		unsigned					sort_flag_matid;

		//unsigned					draw_flag;


		bool operator < ( const Entry & other ) const
		{
			//if( draw_flag > other.draw_flag )
			//	return true;
			if( sort_flag_ae < other.sort_flag_ae )
				return true;
			if( sort_flag_mt < other.sort_flag_mt )
				return true;
			return sort_flag_matid < other.sort_flag_matid;

			//return sort_flag < other.sort_flag;
		}
		//----------------------//
	};

	enum SortMode
	{
		SortDisable = 0,
		SortFrontToBack,
		SortBackToFront,
		SortNone
	};

public:
	//! 构造函数.
     ATOM_RenderQueue (void);

	//! 析构函数.
    ~ATOM_RenderQueue (void);

public:
	//! 添加一个渲染对象到队列中.
	//! \param drawable 渲染对象
	//! \param worldmatrix 渲染对象的世界变换矩阵
	//! \param effect 渲染对象的材质
	//! \param params 材质参数
	void addDrawable (ATOM_Drawable *drawable, ATOM_Camera *camera, const ATOM_Matrix4x4f &worldmatrix, ATOM_Material *material, bool projectable, float distanceOverrideValue = 0.f);
	void render (ATOM_RenderDevice *device, ATOM_Camera *camera, SortMode sortMode, bool decalPass);
    void clear (void);
	bool isEmpty (void) const;

	//--- wangjian added ---//
	bool removeDrawable(ATOM_Drawable *drawable);
	void removeAllDrawable(void);
	void setQueueId(int id);
	unsigned getDPCountApproximate(void);
	unsigned getBatchCountApproximate(void);
#if 1 
	void SetName(const char* name){_name=name;}
	ATOM_STRING _name;
#endif
	//----------------------//

private:
	Entry *allocEntry (void);
	void freeEntry (ATOM_RenderQueue::Entry *entry);
	void drawEntry (ATOM_RenderDevice *device, ATOM_Camera *camera, const Entry &entry);

private:

	//wangjian added for test//
	int _queue_id;
	
	unsigned				_sf_0;
	unsigned				_sf_1;
	unsigned				_sf_2;
	sInstancing				_instancing_wrapper;

	unsigned				_DPCount_approximate;
	unsigned				_batchCount_approximate;
	//-----------------------//


	ATOM_VECTOR<Entry*> _entries;
	ATOM_VECTOR<Entry*> _freeEntryList;
	ATOM_RadixSort _radixSort;
	SortMode _lastSortMode;
};

#endif // __ATOM3D_ENGINE_RENDERQUEUE_H
/*! @} */


