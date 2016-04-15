#include "StdAfx.h"
#include "events.h"

AS_AssetCreateEvent::AS_AssetCreateEvent (void)
: name(0)
, filename(0)
, desc(0)
, randomRotateX(0.f)
, randomRotateY(0.f)
, randomRotateZ(0.f)
, offsetY(0.f)
{
}

AS_AssetCreateEvent::AS_AssetCreateEvent (const char *name_, const char *filename_, const char *desc_, float rotateX_, float rotateY_, float rotateZ_, float offsetY_)
: name(name_)
, filename(filename_)
, desc(desc_)
, randomRotateX(rotateX_)
, randomRotateY(rotateY_)
, randomRotateZ(rotateZ_)
, offsetY(offsetY_)
{
}

AS_AssetReplaceEvent::AS_AssetReplaceEvent (void)
: name(0)
, filename(0)
, desc(0)
{
}

AS_AssetReplaceEvent::AS_AssetReplaceEvent (const char *name_, const char *filename_, const char *desc_)
: name(name_)
, filename(filename_)
, desc(desc_)
{
}

