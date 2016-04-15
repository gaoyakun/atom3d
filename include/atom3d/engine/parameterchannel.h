#ifndef __ATOM3D_ENGINE_PARAMETERCHANNEL_H
#define __ATOM3D_ENGINE_PARAMETERCHANNEL_H

#include "parameter.h"

class ATOM_ENGINE_API ATOM_ParameterChannel
{
public:
	struct Channel
	{
		ATOM_MaterialParam::ValueCallback callback;
		void *context;
		void *context2;
		ATOM_MaterialParam::Type type;
	};

public:
	static bool registerChannel (const char *name, ATOM_MaterialParam::ValueCallback callback, void *context, void *context2, ATOM_MaterialParam::Type type);
	static void unregisterChannel (const char *name);
	static const Channel *getChannel (const char *name);
};

#endif // __ATOM3D_ENGINE_PARAMETERCHANNEL_H
