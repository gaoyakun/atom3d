#include "StdAfx.h"
#include "parameterchannel.h"
#include "paramcallback.h"

class ChannelMap
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_ParameterChannel::Channel> channelMap;

	ChannelMap (void)
	{
		registerChannel ("MVP_Matrix", &mvpMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Inv_MVP_Matrix", &invMVPMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Transpose_MVP_Matrix", &transposeMVPMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("World_Matrix", &worldMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Inv_World_Matrix", &invWorldMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Transpose_World_Matrix", &transposeWorldMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("View_Matrix", &viewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Inv_View_Matrix", &invViewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Transpose_View_Matrix", &transposeViewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Projection_Matrix", &projMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Inv_Projection_Matrix", &invProjMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Transpose_Projection_Matrix", &transposeProjMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("WorldView_Matrix", &modelViewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Inv_WorldView_Matrix", &invModelViewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("Transpose_WorldView_Matrix", &transposeModelViewMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("ViewProjection_Matrix", &viewProjMatrixCallback, 0, 0, ATOM_MaterialParam::ParamType_Matrix44);
		registerChannel ("WorldSpaceEye", &worldSpaceEyePositionCallback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
		registerChannel ("ObjectSpaceEye", &objectSpaceEyePositionCallback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
		registerChannel ("TimeInSecond", &timeCallback, 0, 0, ATOM_MaterialParam::ParamType_Float);
	}

public:
	bool registerChannel (const char *name, ATOM_MaterialParam::ValueCallback callback, void *context, void *context2, ATOM_MaterialParam::Type type)
	{
		if (name && callback)
		{
			ATOM_HASHMAP<ATOM_STRING, ATOM_ParameterChannel::Channel>::const_iterator it = channelMap.find (name);
			if (it != channelMap.end ())
			{
				return false;
			}
			ATOM_ParameterChannel::Channel channel;
			channel.callback = callback;
			channel.context = context;
			channel.context2 = context2;
			channel.type = type;
			channelMap[name] = channel;
			return true;
		}
		return false;
	}

	void unregisterChannel (const char *name)
	{
		if (name)
		{
			ATOM_HASHMAP<ATOM_STRING, ATOM_ParameterChannel::Channel>::iterator it = channelMap.find (name);
			if (it != channelMap.end ())
			{
				channelMap.erase (it);
			}
		}
	}

	const ATOM_ParameterChannel::Channel *getChannel (const char *name)
	{
		if (name)
		{
			ATOM_HASHMAP<ATOM_STRING, ATOM_ParameterChannel::Channel>::const_iterator it = channelMap.find (name);
			if (it != channelMap.end ())
			{
				return &it->second;
			}
		}
		return 0;
	}

	static ChannelMap *getInstance (void)
	{
		static ChannelMap channelMap;
		return &channelMap;
	}
};

bool ATOM_ParameterChannel::registerChannel (const char *name, ATOM_MaterialParam::ValueCallback callback, void *context, void *context2, ATOM_MaterialParam::Type type)
{
	return ChannelMap::getInstance()->registerChannel (name, callback, context, context2, type);
}

void ATOM_ParameterChannel::unregisterChannel (const char *name)
{
	ChannelMap::getInstance()->unregisterChannel (name);
}

const ATOM_ParameterChannel::Channel *ATOM_ParameterChannel::getChannel (const char *name)
{
	return ChannelMap::getInstance()->getChannel (name);
}

