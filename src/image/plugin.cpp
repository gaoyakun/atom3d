#include "StdAfx.h"
#include "basedefs.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

ATOM_PLUGIN_BEGIN(Image, "atom.image", 0, 0, 1)
	ATOM_PLUGIN_DEF(Image, ATOM_Image)
	ATOM_PLUGIN_DEF(Image, ATOM_JPEGCodec)
	ATOM_PLUGIN_DEF(Image, ATOM_PNGCodec)
	ATOM_PLUGIN_DEF(Image, ATOM_DDSCodec)
	ATOM_PLUGIN_DEF(Image, ATOM_AnyCodec)
ATOM_PLUGIN_END

