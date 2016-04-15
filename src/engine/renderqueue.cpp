#include "stdafx.h"
#include "renderscheme.h"

#include "kernel/profiler.h"

#include "drawable.h"
#include "renderqueue.h"


#define DEBUG_RENDERQUEUE_MATERIAL 0

//--- wangjian added ---//
sInstancing::sInstancing():
#if 0
_drawable(0),
#endif
	_batching(false),_material(0),_instanceCountThreshold(50)
{
	if( ATOM_RenderSettings::isUseInstancingStaticBuffer() )
		_instanceCountThreshold = ATOM_RenderSettings::getInstancingSBCountThreshold();
}
sInstancing::~sInstancing()
{ 
#if 1
	_drawables.clear();
#endif
	_instance_data.clear();
}

void sInstancing::beginBatching(ATOM_Drawable * drawable, ATOM_Material * material)
{
#if 0
	_drawable = drawable;
#endif

	_material = material;
	_batching = true;
	_instance_data.clear();
}
bool sInstancing::isBatching()
{
	return _batching;
}
void sInstancing::endBatching()
{
	_batching = false;
	_instance_data.clear();
#if 0
	_drawable = 0;
#else
	_drawables.clear();
#endif
	_material = 0;
}

void sInstancing::batching(ATOM_Drawable * drawable, ATOM_Camera *camera)
{
	if( drawable && _batching )
	{
		_drawables.push_back(drawable);

#if 0
		ATOM_StaticMesh * mesh = (ATOM_StaticMesh *)drawable;
		if( mesh )
		{
			const ATOM_Matrix4x4f & worldMatrix = mesh->getWorldTransform(camera);
			sInstanceAttribute data;
			data._world_matrix.setRow(0,worldMatrix.getRow(0));
			data._world_matrix.setRow(1,worldMatrix.getRow(1));
			data._world_matrix.setRow(2,worldMatrix.getRow(2));
			data._world_matrix.setCol(3,worldMatrix.getRow3(3));
			_instance_data.push_back(data);
		}
#else
		drawable->batching(this,camera);
#endif

	}
}

void sInstancing::drawing(ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//#if 0
	//	if( _drawable )
	//	{
	//		ATOM_StaticMesh * mesh = dynamic_cast<ATOM_StaticMesh*>(_drawable);
	//		if( mesh )
	//			ATOM_LOGGER::log("%s | %s\n",mesh->getModelFileName().c_str(), mesh->getSharedMesh()->getName() ? mesh->getSharedMesh()->getName() : "");
	//	}
	//#endif

#if 0
	if( _batching && _drawable )
		_drawable->drawBatch(device,camera,_material.get(),true,this);
#else
	if( _batching && !_drawables.empty() && _drawables[0] )
	{
		_drawables[0]->drawBatch(device,camera,_material.get(),true,this);

		// 因为不同renderqueue对于drwable对象的排序可能有差入，及同一个batch的drawable对象在不同的renderqueue中可能顺序并不一致，所以
		// 会在单个drwable对象的getEffectIdFront时出现active effect id选择错误的问题，导致渲染出错。
		// 所以此处将所有需要batch的drwable对象都保存到一个数组里，然后在drawBatch之后，将所有的drawable对象都进行一次effectid重排
		int id_front;
		for(int i = 1; i < _drawables.size(); ++i )
		{
			if( _drawables[i] )
				id_front = _drawables[i]->getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		}
	}
#endif


#if 0
	if( _material && _material->getActiveEffect() )
	{
		ATOM_LOGGER::log(	"batch : active effect is : %s, effect is : %s, sort flag is : %u | %u | %u \n",
			_material->getCoreMaterialFileName(),
			_material->getActiveEffect()->getName(), 
			_drawable->getSortFlag(ATOM_Drawable::SF_ACTIVEEFFECT),
			_drawable->getSortFlag(ATOM_Drawable::SF_MESHORTEXTURE),
			_drawable->getSortFlag(ATOM_Drawable::SF_MATERIALID)  );
	}
#endif

	endBatching();
}

unsigned sInstancing::getIntanceCount()
{
	return _instance_data.size();
}

void * sInstancing::getInstancedata()
{
	return (void*)(&_instance_data[0]._world_matrix);
}

void sInstancing::setInstanceCountThreshold(unsigned countThreshold)
{
	_instanceCountThreshold = countThreshold;
}

bool sInstancing::isExceedThreshold()
{
	// only use static instance buffer
	if( _batching && ATOM_RenderSettings::isUseInstancingStaticBuffer() )
		return _instance_data.size() == _instanceCountThreshold;

	return false;
}

void sInstancing::appendInstance( const ATOM_Matrix4x4f & worldMatrix )
{
	sInstanceAttribute data;
	data._world_matrix.setRow(0,worldMatrix.getRow(0));
	data._world_matrix.setRow(1,worldMatrix.getRow(1));
	data._world_matrix.setRow(2,worldMatrix.getRow(2));
	data._world_matrix.setCol(3,worldMatrix.getRow3(3));
	_instance_data.push_back(data);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_RenderQueue::ATOM_RenderQueue (void)
{
	_lastSortMode = SortDisable;

	//--- wangjian added ---//
#if 1
	_name = "";
#endif

	_queue_id = -1;

	_DPCount_approximate = 0;
	_batchCount_approximate = 0;
	//---------------------//
}

ATOM_RenderQueue::~ATOM_RenderQueue (void)
{
	//--- wangjian added ---//
	//removeAllDrawable();

	for (int i = 0; i < _entries.size(); ++i)
	{
		ATOM_DELETE(_entries[i]);
	}
	for (int i = 0; i < _freeEntryList.size(); ++i)
	{
		ATOM_DELETE(_freeEntryList[i]);
	}
	_entries.clear();
	_freeEntryList.clear ();
	//----------------------//
}

void ATOM_RenderQueue::addDrawable (ATOM_Drawable *drawable, ATOM_Camera *camera, const ATOM_Matrix4x4f &worldmatrix, ATOM_Material *material, bool projectable, float distanceOverrideValue)
{
	//--- wangjian added ---//
	if( !drawable )
		return;
	if( ATOM_RenderSettings::isStateSortingEnabled() )
		drawable->setupSortFlag(_queue_id);
	//----------------------//

	ATOM_VECTOR<Entry*> &entries = _entries;
	entries.resize (_entries.size() + 1);
	entries.back() = allocEntry ();
	entries.back()->drawable				= drawable;
	entries.back()->material				= material;
	entries.back()->worldMatrix			= worldmatrix;
	entries.back()->distanceToCamera		= distanceOverrideValue!= 0.f ? distanceOverrideValue : drawable->computeSquaredDistanceToCamera (camera, worldmatrix);
	entries.back()->projectable			= projectable;


	//--- wangjian added ---//
	//entries.back().sort_flag = drawable->getSortFlag();
	entries.back()->sort_flag_ae			= drawable->getSortFlag(ATOM_Drawable::SF_ACTIVEEFFECT);
	entries.back()->sort_flag_mt			= drawable->getSortFlag(ATOM_Drawable::SF_MESHORTEXTURE);
	entries.back()->sort_flag_matid			= drawable->getSortFlag(ATOM_Drawable::SF_MATERIALID);
	//entries.back()->draw_flag				= 1;
	//drawable->setRenderQueueInfo( this, entries.back() );
	//----------------------//

	_lastSortMode = SortDisable;
}

ATOM_RenderQueue::Entry *ATOM_RenderQueue::allocEntry (void)
{
	if (!_freeEntryList.empty ())
	{
		ATOM_RenderQueue::Entry *entry = _freeEntryList.back();
		_freeEntryList.pop_back ();
		return entry;
	}
	else
	{
		return ATOM_NEW(ATOM_RenderQueue::Entry);
	}
}

void ATOM_RenderQueue::freeEntry (ATOM_RenderQueue::Entry *entry)
{
	entry->material = nullptr;
	_freeEntryList.push_back (entry);
}

void ATOM_RenderQueue::drawEntry (ATOM_RenderDevice *device, ATOM_Camera *camera, const Entry &entry)
{
	//--- wangjian added ---//
	if( !entry.drawable/* || !entry.draw_flag*/ )
		return;
	//----------------------//

	// 如果使用实例化
	if( ATOM_RenderSettings::isUseHWInstancing() )
	{
		// 检查是否标记是否发生了变化
		bool bFlagChanged = _sf_0 != entry.sort_flag_ae || _sf_1 != entry.sort_flag_mt || _sf_2 != entry.sort_flag_matid;
		// 检查实例个数是否已经到阈值
		bool bInstanceCountExceed = _instancing_wrapper.isExceedThreshold();
		if( bFlagChanged || bInstanceCountExceed )
		{
			if( bFlagChanged )
			{
				_sf_0	= entry.sort_flag_ae;
				_sf_1	= entry.sort_flag_mt;
				_sf_2	= entry.sort_flag_matid;
			}

			// 如果有支持GEOMETRY INSTANCING的drawable ENTRY
			if( _instancing_wrapper.isBatching() )
			{
				_instancing_wrapper.drawing(device, camera);	// 实例化渲染
				_DPCount_approximate++;
			}
			// 如果该ENTRY的drawable支持GEOMETRY INSTANCING
			if( entry.drawable->getObjFlag() & ATOM_Drawable::OF_GEOINSTANCING )
			{
				_instancing_wrapper.beginBatching(entry.drawable,entry.material.get());
				_batchCount_approximate++;
			}
		}
	}
	
	// 进行实例数据打包
	if( ATOM_RenderSettings::isUseHWInstancing() && _instancing_wrapper.isBatching() )
	{
		_instancing_wrapper.batching(entry.drawable,camera);
	}
	// 否则直接渲染
	else
	{
		device->setTransform (ATOM_MATRIXMODE_WORLD, entry.worldMatrix);
		entry.drawable->draw (device, camera, entry.material.get());
		_DPCount_approximate++;

#if DEBUG_RENDERQUEUE_MATERIAL
		ATOM_Material * mat = entry.material.get();
		if( mat && mat->getActiveEffect() )
		{
			ATOM_LOGGER::log(	"not batch : the distance is : %f, the active material is : %s, effect is : %s, sort flag is : %u | %u | %u \n",
				entry.distanceToCamera,
				mat->getCoreMaterialFileName(),
				mat->getActiveEffect()->getName(), 
				entry.sort_flag_ae,entry.sort_flag_mt,entry.sort_flag_matid  );
		}
#endif
	}
}

//--- wangjian added ---//
bool RQ_SORT_FUNC(const ATOM_RenderQueue::Entry * x, const ATOM_RenderQueue::Entry * y) 
{
	ATOM_ASSERT(x && y);

	/*if( x->draw_flag > y->draw_flag )
	{
		return true;
	}
	else if( x->draw_flag == y->draw_flag )
	{*/
		if( x->sort_flag_ae < y->sort_flag_ae )
		{
			return true;
		}
		else if( x->sort_flag_ae == y->sort_flag_ae )
		{
			if( x->sort_flag_mt < y->sort_flag_mt )
			{
				return true;
			}
			else if( x->sort_flag_mt == y->sort_flag_mt )
			{
				if( x->sort_flag_matid < y->sort_flag_matid )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	//}
	
	return false;

	/*
	else if( x->sort_flag_ae < y->sort_flag_ae )
		return true;
	else if( x->sort_flag_mt < y->sort_flag_mt )
		return true;
	else
		return x->sort_flag_matid < y->sort_flag_matid;*/
}
//----------------------//

bool DistanceSortFunc (const ATOM_RenderQueue::Entry * x, const ATOM_RenderQueue::Entry * y) 
{
	return x->distanceToCamera < y->distanceToCamera;
}

void ATOM_RenderQueue::render (ATOM_RenderDevice *device, ATOM_Camera *camera, SortMode sortMode, bool decalPass)
{
	//--- wangjian added ------//
	_sf_0 = 0;
	_sf_1 = 0;
	_sf_2 = 0;
	_instancing_wrapper.endBatching();
	_DPCount_approximate = 0;
	_batchCount_approximate = 0;
	//-------------------------//

	if (_entries.empty ())
	{
		return;
	}

	//// 如果开启渲染状态排序
	//if( ATOM_RenderSettings::isStateSortingEnabled() )
	//{
	//	// 如果还未排序过 进行排序
	//	//_radixSort.sort (&_entries.front().sort_flag, _entries.size(), false, sizeof(Entry));

	//	_radixSort.sort (	&_entries.front().sort_flag_matid,	_entries.size(), false, sizeof(Entry)	);
	//	_radixSort.sort (	&_entries.front().sort_flag_mt,		_entries.size(), false, sizeof(Entry)	);
	//	_radixSort.sort (	&_entries.front().sort_flag_ae,		_entries.size(), false, sizeof(Entry)	);
	//}
	
	if (sortMode != _lastSortMode)
	{
		

		//--- wangjian added ---//
			/*char buffer[128] = {0};
			sprintf( buffer,"%s_%d",_name.c_str(),(int)_entries.size() );
			ATOM_Profiler profile(buffer);*/

		// 如果进行状态排序
		if( ATOM_RenderSettings::isStateSortingEnabled() && sortMode == SortNone )
		{
							
#if 0
			//_radixSort.sort (&_entries.front().sort_flag, _entries.size(), false, sizeof(Entry));

			_radixSort.sort (	&_entries.front()->sort_flag_matid,	_entries.size(), false, sizeof(Entry)	);
			_radixSort.sort (	&_entries.front()->sort_flag_mt,		_entries.size(), false, sizeof(Entry)	);
			_radixSort.sort (	&_entries.front()->sort_flag_ae,		_entries.size(), false, sizeof(Entry)	);
			
#else

			std::sort(_entries.begin(),_entries.end(),RQ_SORT_FUNC);

#endif
		}

		// 透明对象按从后往前的顺序
		if (sortMode != SortNone)
		{
			std::stable_sort (_entries.begin(), _entries.end(), DistanceSortFunc);
		}

		//----------------------//

		_lastSortMode = sortMode;
	}
	else
	{
		// 如果开启渲染状态排序
#if 0
		if( ATOM_RenderSettings::isStateSortingEnabled() )
		{
			// 如果还未排序过 进行排序
			/*if( 0 == _radixSort.getIndices())*/
			{
				//_radixSort.sort (&_entries.front().sort_flag, _entries.size(), false, sizeof(Entry));

				_radixSort.sort (	&_entries.front().sort_flag_matid,	_entries.size(), false, sizeof(Entry)	);
				_radixSort.sort (	&_entries.front().sort_flag_mt,		_entries.size(), false, sizeof(Entry)	);
				_radixSort.sort (	&_entries.front().sort_flag_ae,		_entries.size(), false, sizeof(Entry)	);
			}
		}
#endif
	}

	// 从后往前排序绘制
	if (sortMode == SortBackToFront)
	{
#if 1
		for (int i = _entries.size()-1; i >= 0; --i)
		{
			drawEntry (device, camera, *_entries[i]);
		}

		// 如果还有未处理的INSTANCING,处理完
		if( _instancing_wrapper.isBatching() )
		{
			_instancing_wrapper.drawing(device, camera);	// 实例化渲染
			_DPCount_approximate++;
		}
#else
		// 如果不按状态排序
		if( !ATOM_RenderSettings::isStateSortingEnabled() )
		{
			for (int i = _entries.size()-1; i>=0; --i)
			{
				drawEntry (device, camera, _entries[i]);
			}

			// 如果还有未处理的INSTANCING,处理完
			if( _instancing_wrapper.isBatching() )
			{
				_instancing_wrapper.drawing(device, camera);	// 实例化渲染
				_DPCount_approximate++;
			}
		}
		else
		{
			// 透明对象按从后往前的顺序 反序
			unsigned * indices = _radixSort.getIndices();
			unsigned int count = _entries.size();
			for (int j = count-1; j >= 0; --j)
			{
				unsigned i = indices[j];
				drawEntry (device, camera, _entries[i]);
			}

			// 如果还有未处理的INSTANCING,处理完
			if( _instancing_wrapper.isBatching() )
			{
				_instancing_wrapper.drawing(device, camera);	// 实例化渲染
				_DPCount_approximate++;
			}
		}
#endif
	}
	else
	{
		if (decalPass)
		{
#if 1
			if( !ATOM_RenderSettings::isStateSortingEnabled() )
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					if (!_entries[i]->projectable)
					{
						drawEntry (device, camera, *_entries[i]);
					}
				}

				for (int i = 0; i < _entries.size(); ++i)
				{
					if (_entries[i]->projectable)
					{
						drawEntry (device, camera, *_entries[i]);
					}

				}
			}
			// 使用STATE SORTING 对 RENDER ITEM 进行排序
			else
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					if (!_entries[i]->projectable)
					{
						drawEntry (device, camera, *_entries[i]);
					}
				}

				// 如果还有未处理的INSTANCING,处理完
				if( ATOM_RenderSettings::isUseHWInstancing() && _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////

				for (int i = 0; i < _entries.size(); ++i)
				{
					if (_entries[i]->projectable)
					{
						drawEntry (device, camera, *_entries[i]);
					}

				}

				// 如果还有未处理的INSTANCING,处理完
				if( ATOM_RenderSettings::isUseHWInstancing() && _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}
			}
#else
			if( !ATOM_RenderSettings::isStateSortingEnabled() )
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					if (!_entries[i].projectable)
					{
						drawEntry (device, camera, _entries[i]);
					}
				}
					
				for (int i = 0; i < _entries.size(); ++i)
				{
					if (_entries[i].projectable)
					{
						drawEntry (device, camera, _entries[i]);
					}
					
				}
			}
			// 使用STATE SORTING 对 RENDER ITEM 进行排序
			else
			{
				unsigned * indices = _radixSort.getIndices();
				unsigned int count = _entries.size();
				for (int j = 0; j < count; ++j)
				{
					unsigned i = indices[j];

					if (!_entries[i].projectable)
					{
						drawEntry (device, camera, _entries[i]);
					}
				}

				// 如果还有未处理的INSTANCING,处理完
				if( ATOM_RenderSettings::isUseHWInstancing() && _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////

				indices = _radixSort.getIndices();
				for (int j = 0; j < count; ++j)
				{
					unsigned i = indices[j];
					if (_entries[i].projectable)
					{
						drawEntry (device, camera, _entries[i]);
					}
				}

				// 如果还有未处理的INSTANCING,处理完
				if( ATOM_RenderSettings::isUseHWInstancing() && _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}
			}
#endif

		}
		else
		{
#if 1
			if( !ATOM_RenderSettings::isStateSortingEnabled() )
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					drawEntry (device, camera, *_entries[i]);
				}
			}
			else
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					drawEntry (device, camera, *_entries[i]);
				}

				// 如果还有未处理的INSTANCING,处理完
				if( _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}
			}
#else
			if( !ATOM_RenderSettings::isStateSortingEnabled() )
			{
				for (int i = 0; i < _entries.size(); ++i)
				{
					drawEntry (device, camera, _entries[i]);
				}
			}
			else
			{
				unsigned * indices = _radixSort.getIndices();
				unsigned int count = _entries.size();
				for (int j = 0; j < count; ++j)
				{
					unsigned i = indices[j];
					drawEntry (device, camera, _entries[i]);
				}

				// 如果还有未处理的INSTANCING,处理完
				if( _instancing_wrapper.isBatching() )
				{
					_instancing_wrapper.drawing(device, camera);	// 实例化渲染
					_DPCount_approximate++;
				}
			}
#endif
		}
	}
}

void ATOM_RenderQueue::clear (void)
{
	for( int i = 0;i < _entries.size();++i )
	{
		freeEntry(_entries[i]);
	}
	_entries.clear();

	_lastSortMode = SortDisable;
}

bool ATOM_RenderQueue::isEmpty (void) const
{
	return _entries.empty ();
}

//--- wangjian added ----//
// 移除可绘制物体 
// 并非真正移除，只是设置一个标记，使其无法被绘制
bool ATOM_RenderQueue::removeDrawable(ATOM_Drawable *drawable)
{
	//if( !drawable )
	//	return false;
	//
	//Entry * entry = drawable->getRenderQueueInfo(this);
	//if( entry && entry->drawable && entry->drawable == drawable )
	//{
	//	entry->draw_flag = 0;	// 设置渲染标记为0
	//}
	return true;
}
void ATOM_RenderQueue::removeAllDrawable(void)
{
	/*ATOM_VECTOR<Entry*>::iterator iter =  _entries.begin();
	for(; iter != _entries.end(); ++iter)
	{
		Entry * entry = *iter;
		if( entry && entry->drawable && entry->draw_flag != 0 )
			entry->drawable->removeSelfFromRenderQueue(this);
	}*/
}
void ATOM_RenderQueue::setQueueId(int id)
{
	_queue_id = id;
}

unsigned ATOM_RenderQueue::getDPCountApproximate(void)
{
	return _DPCount_approximate;
}
unsigned ATOM_RenderQueue::getBatchCountApproximate(void)
{
	return _batchCount_approximate;
}

