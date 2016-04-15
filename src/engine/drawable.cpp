#include "StdAfx.h"
#include "drawable.h"

//--- wangjian added ---//
ATOM_Drawable::ATOM_Drawable (void)
{
	//_sort_flag = 0;

	_sort_flag_ae = 0;
	_sort_flag_mt = 0;
	_sort_flag_matid = 0;
	_obj_flag = 0;
}
//----------------------//

ATOM_Drawable::~ATOM_Drawable (void)
{
	//--- wangjian added ---//
	// 将该可绘制对象从绘制队列中移除
	/*ATOM_VECTOR<sRQInfo>::iterator iter = _rq_info_vec.begin();
	for( ; iter != _rq_info_vec.end(); ++iter )
	{
		if( iter->_renderqueue )
			iter->_renderqueue->removeDrawable(this);
	}
	_rq_info_vec.clear();*/

	_cull_results.clear();
	//----------------------//
}

float ATOM_Drawable::computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix) const
{
	return (camera->getPosition()-worldMatrix.getRow3(3)).getSquaredLength();
}

//--- wangjian added ---//
bool ATOM_Drawable::drawBatch(	ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material,
								bool bInstancing/*=true*/, sInstancing * instancing/* = 0*/	)
{
	return draw(device,camera,material);
}
void ATOM_Drawable::batching(sInstancing * instanceWrapper, ATOM_Camera *camera)
{

}
void ATOM_Drawable::prepareForSort(ATOM_RenderScheme * renderScheme)
{
	ATOM_MAP<ATOM_RenderScheme*,sCullResult>::iterator iter = _cull_results.find(renderScheme);
	if( iter == _cull_results.end() )
	{
		sCullResult cr;
		_cull_results.insert( std::make_pair( renderScheme,cr ) );
	}
	else
	{
		sCullResult & cr = iter->second;
		cr._effect_cullresult.clear();
	}
}
void ATOM_Drawable::appendEffectId(ATOM_RenderScheme * renderScheme, int id)
{
	ATOM_MAP<ATOM_RenderScheme*,sCullResult>::iterator iter = _cull_results.find(renderScheme);
	if( iter != _cull_results.end() )
	{
		sCullResult & cr = iter->second;
		cr._effect_cullresult.push_back(id);
	}
}
int	ATOM_Drawable::getEffectIdEnd(ATOM_RenderScheme* renderScheme)
{
	ATOM_MAP<ATOM_RenderScheme*,sCullResult>::iterator iter = _cull_results.find(renderScheme);
	if( iter != _cull_results.end() )
	{
		sCullResult & cr = iter->second;
		if( cr._effect_cullresult.empty() )
			return -1;
		return cr._effect_cullresult.back();
	}
	return -1;
}
int ATOM_Drawable::getEffectIdFront(ATOM_RenderScheme * renderScheme)
{
	ATOM_MAP<ATOM_RenderScheme*,sCullResult>::iterator iter = _cull_results.find(renderScheme);
	if( iter != _cull_results.end() )
	{
		sCullResult & cr = iter->second;
		if( cr._effect_cullresult.empty() )
			return -1;

		int id = cr._effect_cullresult.front();
		cr._effect_cullresult.pop_front();
		cr._effect_cullresult.push_back(id);

		return id;
	}
	return -1;
}
// 设置绘制排序标记
void ATOM_Drawable::setupSortFlag(int queueid/*=-1*/)
{

}


//void ATOM_Drawable::setSortFlag(unsigned sortFlag)
//{
//	_sort_flag = sortFlag;
//}
//unsigned ATOM_Drawable::getSortFlag(void) const
//{
//	return _sort_flag;
//}
void ATOM_Drawable::setSortFlag(unsigned sf_ae, unsigned sf_mt, unsigned sf_matid)
{
	_sort_flag_ae		= sf_ae;
	_sort_flag_mt		= sf_mt;
	_sort_flag_matid	= sf_matid;
}
unsigned ATOM_Drawable::getSortFlag(unsigned sf) const
{
	switch( sf )
	{
	case SF_ACTIVEEFFECT:
		return _sort_flag_ae;
	case SF_MESHORTEXTURE:
		return _sort_flag_mt;
	case SF_MATERIALID:
		return _sort_flag_matid;
	}
	return -1;
}

void ATOM_Drawable::setObjFlag(unsigned of)
{
	if( _obj_flag & of )
		return;
	_obj_flag |= of;
}
void ATOM_Drawable::resetObjFlag(unsigned of)
{
	if( _obj_flag & of )
		_obj_flag &= ~of;
}
unsigned ATOM_Drawable::getObjFlag(void) const
{
	return _obj_flag;
}

void	ATOM_Drawable::setRenderQueueInfo(ATOM_RenderQueue * renderqueue,ATOM_RenderQueue::Entry * entry)
{
	/*if( !renderqueue )
		return;

	ATOM_VECTOR<sRQInfo>::iterator iter = _rq_info_vec.begin();
	for( ; iter != _rq_info_vec.end(); ++iter )
	{
		if( iter->_renderqueue == renderqueue )
		{
			iter->_rq_entry = entry;
			return;
		}
	}
	sRQInfo info;
	info._renderqueue = renderqueue;
	info._rq_entry = entry;
	_rq_info_vec.push_back(info);*/
}
ATOM_RenderQueue::Entry * ATOM_Drawable::getRenderQueueInfo(ATOM_RenderQueue * renderqueue)
{
	/*ATOM_VECTOR<sRQInfo>::iterator iter = _rq_info_vec.begin();
	for( ; iter != _rq_info_vec.end(); ++iter )
	{
		if( iter->_renderqueue == renderqueue )
		{
			return iter->_rq_entry;
		}
	}*/
	return 0;
}
void ATOM_Drawable::removeSelfFromRenderQueue(ATOM_RenderQueue * renderqueue)
{
	/*ATOM_VECTOR<sRQInfo>::iterator iter = _rq_info_vec.begin();
	for( ; iter != _rq_info_vec.end(); ++iter )
	{
	if( iter->_renderqueue == renderqueue )
	{
	_rq_info_vec.erase(iter);
	return;
	}
	}*/
}
//----------------------//

