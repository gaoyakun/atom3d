#include "StdAfx.h"
#include "imagelist.h"
#include "gui_canvas.h"
#include "gui_renderer.h"
#include "vectorgraph.h"

//class SwappableTexture: public ATOM_Swappable
//{
//	ATOM_AUTOREF(ATOM_Texture) _texture;
//	ATOM_STRING _filename;
//	unsigned _size;
//
//public:
//	SwappableTexture (const char *textureFileName): _filename(textureFileName)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::SwappableTexture);
//
//		_size = 0;
//
//		if (!_filename.empty ())
//		{
//			ATOM_SwappableManager::registerSwappable (this, false);
//		}
//	}
//
//	SwappableTexture (ATOM_Texture *texture): _texture(texture)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::SwappableTexture);
//
//		_size = 0;
//	}
//
//	virtual ~SwappableTexture (void)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::~SwappableTexture);
//
//		if (!_filename.empty())
//		{
//			ATOM_SwappableManager::unregisterSwappable (this);
//		}
//	}
//
//public:
//	virtual unsigned getSize (void) const
//	{
//		ATOM_STACK_TRACE(SwappableTexture::getSize);
//
//		return _size;
//	}
//
//	virtual bool isDataValid (void) const
//	{
//		ATOM_STACK_TRACE(SwappableTexture::isDataValid);
//
//		return _texture != 0;
//	}
//
//protected:
//	virtual bool write (void *buffer)
//	{
//		return true;
//	}
//
//	virtual bool read (const void *buffer)
//	{
//		return true;
//	}
//
//	virtual void invalidate (void)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::invalidate);
//
//		_texture = 0;
//	}
//
//	virtual bool swapToDisk (void)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::swapToDisk);
//
//		if (!_filename.empty ())
//		{
//			invalidate ();
//		}
//		return true;
//	}
//
//	virtual void loadFromDisk (void)
//	{
//		ATOM_STACK_TRACE(SwappableTexture::loadFromDisk);
//
//		if (!_filename.empty ())
//		{
//			_texture = ATOM_CreateTextureResource (_filename.c_str());
//		}
//	}
//
//public:
//	ATOM_Texture *getTexture (void) const
//	{
//		ATOM_STACK_TRACE(SwappableTexture::getTexture);
//
//		return _texture.get();
//	}
//
//	const ATOM_STRING &getFileName (void) const
//	{
//		ATOM_STACK_TRACE(SwappableTexture::getFileName);
//
//		return _filename;
//	}
//};

ATOM_GUIImage::ATOM_GUIImage (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::ATOM_GUIImage);

	for (unsigned i = 0; i < WST_COUNT; ++i)
	{
		_states[i].type = IT_NULL;
		_states[i].color = 0xFFFFFFFF;
		_states[i].region.point.x = 0;
		_states[i].region.point.y = 0;
		_states[i].region.size.w = 0;
		_states[i].region.size.h = 0;
		_states[i].width = 0;
		_states[i].height = 0;
		_states[i].texture = 0;
		_states[i].textureDirty = false;
	}

	//--- wangjian added ---//
	// 默认开启异步加载
	_loadPriority = GUIIMAGE_LOADPRI_BASE;		// UI图片异步加载优先级很高 默认为1000
	//----------------------//
}

ATOM_GUIImage::ATOM_GUIImage (const ATOM_GUIImage &other)
{
	for (unsigned i = 0; i < WST_COUNT; ++i)
	{
		_states[i] = other._states[i];
		if (_states[i].type == IT_VG)
		{
			_states[i].vg = ATOM_NEW(ATOM_VectorGraph, *_states[i].vg);
		}
		if (_states[i].material)
		{
			_states[i].material = _states[i].material->clone ();
		}
	}
	_loadPriority = other._loadPriority;
}

ATOM_GUIImage &ATOM_GUIImage::operator = (const ATOM_GUIImage &other)
{
	ATOM_GUIImage tmp(other);
	swap (tmp);
	return *this;
}

void ATOM_GUIImage::swap (ATOM_GUIImage &other)
{
	for (unsigned i = 0; i < WST_COUNT; ++i)
	{
		std::swap(_states[i], other._states[i]);
	}
	std::swap (_loadPriority, other._loadPriority);
}

ATOM_GUIImage::~ATOM_GUIImage (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::~ATOM_GUIImage);

	for (unsigned i = 0; i < WST_COUNT; ++i)
	{
		if (_states[i].type == IT_VG)
		{
			ATOM_DELETE(_states[i].vg);
		}
	}
}

ATOM_Texture *ATOM_GUIImage::getTexture (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::lockTexture);

	updateState (state);
	ATOM_Texture *tex = _states[state].texture.get();
	if (!tex && state != WST_NORMAL)
	{
		tex = getTexture (WST_NORMAL);
	}
	return tex;
}

ATOM_GUIImage::Type ATOM_GUIImage::getType (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getType);

	return _states[state].type;
}

void ATOM_GUIImage::setType (ATOM_WidgetState state, ATOM_GUIImage::Type type)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::setType);

	if (type != _states[state].type)
	{
		if (type == IT_VG)
		{
			_states[state].vg = ATOM_NEW(ATOM_VectorGraph);
		}
		else if (_states[state].type == IT_VG)
		{
			ATOM_DELETE(_states[state].vg);
			_states[state].vg = 0;
		}

		_states[state].type = type;
		_states[state].textureDirty = true;
	}
}

unsigned ATOM_GUIImage::getTextureWidth (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getTextureWidth);

	//if (_states[state].request)
	//{
	//	ATOM_ContentStream::waitForRequestDone (_states[state].request);
	//	_states[state].request = 0;
	//}

	updateState (state);
	unsigned w = _states[state].width;

	// wangjian added : 检查纹理是否加载完成 获取纹理宽度
	if( w == 0 )
	{
		if( _states[state].texture && _states[state].texture->getAsyncLoader()->IsLoadAllFinished() && !_states[state].texture->getAsyncLoader()->IsLoadFailed() )
		{
			_states[state].width	= _states[state].texture->getWidth();
			_states[state].height	= _states[state].texture->getHeight();
			w = _states[state].width;
		}
	}

	if (w == 0 && state != WST_NORMAL)
	{
		w = getTextureWidth (WST_NORMAL);
	}
	return w;
}

bool checkFileExistence (const char *filename)
{
	char native[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName(filename, native);
	return ATOM_PhysicVFS().doesFileExists (native);
}


void ATOM_GUIImage::updateState (ATOM_WidgetState state)
{
	if (_states[state].textureDirty)
	{
		_states[state].textureDirty = false;

		if (_states[state].type == ATOM_GUIImage::IT_IMAGE || _states[state].type == ATOM_GUIImage::IT_IMAGE9 || _states[state].type == ATOM_GUIImage::IT_MATERIAL)
		{
			//--- wangjian MODIFIED : UI 图片是否使用异步加载 ---//
#if 0
			bool bExistInDisk = checkFileExistence(_states[state].textureFileName.c_str());
			bool bAsync = ATOM_GUIRenderer::isMultiThreadingEnabled() && (_loadPriority != ATOM_LoadPriority_IMMEDIATE)/* && !bExistInDisk*/;
			int loadPri = _loadPriority;
			if( !bAsync )
				loadPri = ATOM_LoadPriority_IMMEDIATE;
			_states[state].texture = ATOM_CreateTextureResource(_states[state].textureFileName.c_str(), ATOM_PIXEL_FORMAT_UNKNOWN, loadPri);
			if( _states[state].texture )
				_states[state].texture->getAsyncLoader()->setExistInDisk(bExistInDisk);
#else
			bool bAsync = ATOM_GUIRenderer::isMultiThreadingEnabled() && (_loadPriority != ATOM_LoadPriority_IMMEDIATE);
			int loadPri = _loadPriority;
			if( !bAsync )
				loadPri = ATOM_LoadPriority_IMMEDIATE;
			_states[state].texture = ATOM_CreateTextureResource(_states[state].textureFileName.c_str(), ATOM_PIXEL_FORMAT_UNKNOWN, loadPri);
#endif
			//-----------------------------------------------------//
			_states[state].width = _states[state].texture ? _states[state].texture->getWidth() : 0;
			_states[state].height = _states[state].texture ? _states[state].texture->getHeight() : 0;
		}
		else
		{
			_states[state].texture = NULL;
			_states[state].width = 0;
			_states[state].height = 0;
		}
	}
}

unsigned ATOM_GUIImage::getTextureHeight (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getTextureHeight);

	//if (_states[state].request)
	//{
	//	ATOM_ContentStream::waitForRequestDone (_states[state].request);
	//	_states[state].request = 0;
	//}

	updateState (state);
	unsigned h = _states[state].height;

	// wangjian added : 检查纹理是否加载完成 获取纹理高度
	if( h == 0 )
	{
		if( _states[state].texture && _states[state].texture->getAsyncLoader()->IsLoadAllFinished() && !_states[state].texture->getAsyncLoader()->IsLoadFailed() )
		{
			_states[state].width	= _states[state].texture->getWidth();
			_states[state].height	= _states[state].texture->getHeight();
			h = _states[state].height;
		}
	}

	if (h == 0 && state != WST_NORMAL)
	{
		h = getTextureHeight (WST_NORMAL);
	}
	return h;
}

const ATOM_Rect2Di &ATOM_GUIImage::getRegion (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getRegion);

	return _states[state].region;
}

ATOM_VectorGraph *ATOM_GUIImage::getVectorGraph (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getVectorGraph);

	return _states[state].vg;
}

static void __cdecl ImageCallback (ATOM_LoadingRequest *request)
{
	ATOM_STACK_TRACE(ImageCallback);

	ATOM_DELETE(request);
}

void ATOM_GUIImage::setImage (ATOM_WidgetState state, const char *filename)
{
	ATOM_STACK_TRACE(setImage);

	_states[state].textureFileName = filename ? filename : "";
	_states[state].textureDirty = true;

	/*
	if (filename)
	{

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		if (device)
		{
			_states[state].texture = ATOM_CreateTextureResource(filename);
			_states[state].width = _states[state].texture ? _states[state].texture->getWidth() : 0;
			_states[state].height = _states[state].texture ? _states[state].texture->getHeight() : 0;
		}
	}
	*/
}

void ATOM_GUIImage::setMaterial (ATOM_WidgetState state, const char *filename)
{
	if (filename && filename[0])
	{
#if 1
		_states[state].material = ATOM_GUIImageList::allocMaterial (filename);
#else
		_states[state].material = ATOM_MaterialManager::createMaterialFromFile(ATOM_GetRenderDevice(), filename);
#endif
		if (_states[state].material)
		{
			_states[state].material->setActiveEffect ("default");
		}
	}
}

void ATOM_GUIImage::setMaterial (ATOM_WidgetState state, ATOM_Material *material)
{
	_states[state].material = material;
}

ATOM_Material *ATOM_GUIImage::getMaterial (ATOM_WidgetState state)
{
	ATOM_Material *material = _states[state].material.get();
	if (!material && state != WST_NORMAL)
	{
		material = _states[WST_NORMAL].material.get();
	}
	return material;
}

void ATOM_GUIImage::setImage (ATOM_WidgetState state, ATOM_Texture *texture)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::setImage);
	_states[state].texture = texture;
	_states[state].width = texture ? texture->getWidth() : 0;
	_states[state].height = texture ? texture->getHeight() : 0;
	_states[state].textureDirty = false;
}

ATOM_ColorARGB ATOM_GUIImage::getColor (ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::getColor);

	return _states[state].color;
}

void ATOM_GUIImage::setColor (ATOM_WidgetState state, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::setColor);

	_states[state].color = color;
}

void ATOM_GUIImage::setRegion (ATOM_WidgetState state, const ATOM_Rect2Di &region)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::setRegion);

	_states[state].region = region;
}

void ATOM_GUIImage::draw (ATOM_WidgetState state, ATOM_GUICanvas *canvas, const ATOM_Rect2Di &rect, ATOM_ColorARGB* customColor, float rotation)
{
	ATOM_STACK_TRACE(ATOM_GUIImage::draw);

	ATOM_ASSERT(state != WST_UNKNOWN);
	updateState (state);

	if (canvas)
	{
		switch (_states[state].type)
		{
		case IT_IMAGE:
			{
				ATOM_Texture *texture = getTexture (state);
				const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
				canvas->drawTexturedRectEx (0, 
											rect, 
											color, 
											( texture && texture->getAsyncLoader()->IsLoadAllFinished() ) ? texture:ATOM_GetColorTexture(0x00FFFFFF),	// wangjian modified
											_states[state].region, 
											false, 
											rotation);
				break;
			}
		case IT_IMAGE9:
			{
				const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
				ATOM_Texture *texture = getTexture (state);
				//--- wangjian modified ---//
				if (!texture || !texture->getAsyncLoader()->IsLoadAllFinished() )
				{
					canvas->drawTexturedRect (0, rect, color, ATOM_GetColorTexture(0x00FFFFFF), false);
				}
				else
				{
					const ATOM_Rect2Di &region = _states[state].region;
					if (region.size.w == 0 || region.size.h == 0)
					{
						canvas->drawTexturedRect (0, rect, color, texture, false);
					}
					else
					{
						int width = texture->getWidth();
						int height = texture->getHeight();

						const ATOM_Rect2Di rects[9] = {
							ATOM_Rect2Di(rect.point.x, rect.point.y, region.point.x, region.point.y),
							ATOM_Rect2Di(rect.point.x+region.point.x, rect.point.y, rect.size.w-(width-region.size.w), region.point.y),
							ATOM_Rect2Di(rect.point.x+rect.size.w-(width-region.size.w-region.point.x), rect.point.y, width-region.size.w-region.point.x, region.point.y),
							ATOM_Rect2Di(rect.point.x, rect.point.y+region.point.y, region.point.x, rect.size.h-(height-region.size.h)),
							ATOM_Rect2Di(rect.point.x+region.point.x, rect.point.y+region.point.y, rect.size.w-(width-region.size.w), rect.size.h-(height-region.size.h)),
							ATOM_Rect2Di(rect.point.x+rect.size.w-(width-region.size.w-region.point.x), rect.point.y+region.point.y, width-region.size.w-region.point.x, rect.size.h-(height-region.size.h)),
							ATOM_Rect2Di(rect.point.x, rect.point.y+rect.size.h-(height-region.size.h-region.point.y), region.point.x, height-region.size.h-region.point.y),
							ATOM_Rect2Di(rect.point.x+region.point.x, rect.point.y+rect.size.h-(height-region.size.h-region.point.y), rect.size.w-(width-region.size.w), height-region.size.h-region.point.y),
							ATOM_Rect2Di(rect.point.x+rect.size.w-(width-region.size.w-region.point.x), rect.point.y+rect.size.h-(height-region.size.h-region.point.y), width-region.size.w-region.point.x, height-region.size.h-region.point.y)
						};

						const ATOM_ColorARGB colors[9] = {
							color, color, color, color, color, color, color, color, color
						};

						const ATOM_Rect2Di regions[9] = {
							ATOM_Rect2Di(0, 0, region.point.x, region.point.y),
							ATOM_Rect2Di(region.point.x, 0, region.size.w, region.point.y),
							ATOM_Rect2Di(region.point.x+region.size.w, 0, width-region.point.x-region.size.w, region.point.y),
							ATOM_Rect2Di(0, region.point.y, region.point.x, region.size.h),
							ATOM_Rect2Di(region.point.x, region.point.y, region.size.w, region.size.h),
							ATOM_Rect2Di(region.point.x+region.size.w, region.point.y, width-region.point.x-region.size.w, region.size.h),
							ATOM_Rect2Di(0, region.point.y+region.size.h, region.point.x, height-region.point.y-region.size.h),
							ATOM_Rect2Di(region.point.x, region.point.y+region.size.h, region.size.w, height-region.point.y-region.size.h),
							ATOM_Rect2Di(region.point.x+region.size.w, region.point.y+region.size.h, width-region.point.x-region.size.w, height-region.point.y-region.size.h)
						};

						canvas->drawTexturedRectListEx (0, 9, rects, colors, _states[state].texture.get(), regions, false);
					}
				}
				break;
			}
		case IT_COLOR:
			{
				const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
				canvas->fillRect (rect, color, rotation);
				break;
			}
		case IT_MATERIAL:
			{
				ATOM_Material *material = getMaterial (state);
				ATOM_Texture *texture = getTexture (state);
				const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
				canvas->drawTexturedRectEx (material, 
											rect, 
											color, 
											( texture && texture->getAsyncLoader()->IsLoadAllFinished() ) ? texture : ATOM_GetColorTexture(0x00FFFFFF),		// wangjian modified
											_states[state].region, 
											false, 
											rotation);
				break;
			}
		case IT_VG:
			{
				if (rect.size.w > 0 && rect.size.h > 0)
				{
					_states[state].vg->setTranslation (rect.point.x, rect.point.y);
					_states[state].vg->setScale (rect.size.w, rect.size.h);
					_states[state].vg->draw (canvas);
				}
				break;
			}
		case IT_NULL:
		default:
			break;
		}
	}
}

void ATOM_GUIImage::drawRect (ATOM_WidgetState state, ATOM_GUICanvas *canvas, const ATOM_Rect2Di &rect, const ATOM_Rect2Di &rectTexture, ATOM_ColorARGB* customColor, float rotation)
{
#if 1
	ATOM_Rect2Di rcSave = _states[state].region;
	_states[state].region = rectTexture;
	draw (state, canvas, rect, customColor, rotation);
	_states[state].region = rcSave;
#else
	ATOM_STACK_TRACE(ATOM_GUIImage::drawRect);
	updateState (state);

	if (canvas)
	{
		switch (_states[state].type)
		{
		case IT_IMAGE:
		case IT_IMAGE9:
			{
				ATOM_Texture *texture = getTexture (state);

				// wangjian modified 
				if ( !texture || !texture->getAsyncLoader()->IsLoadAllFinished() )
				{
					texture = ATOM_GetColorTexture (0x00FFFFFF);
					draw (state, canvas, rect, customColor, rotation);
				}
				else
				{
					const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
					if (rectTexture.size.w == 0 || rectTexture.size.h == 0)
					{
						canvas->drawTexturedRect (rect, color, texture, false, rotation);
					}
					else
					{
						canvas->drawTexturedRectEx (rect, color, texture, rectTexture, false, rotation);
					}
				}
				break;
			}
		case IT_COLOR:
			{
				const ATOM_ColorARGB color = customColor ? *customColor : _states[state].color;
				canvas->fillRect (rect, color, rotation);
				break;
			}
		case IT_NULL:
		default:
			break;
		}
	}
#endif
}

//--- wangjian added ---//
// 异步加载标记
void ATOM_GUIImage::setAsyncLoad(int loadpriority)
{
	_loadPriority = loadpriority;
}
int ATOM_GUIImage::getAsyncLoad() const
{
	return _loadPriority;
}
bool ATOM_GUIImage::isAsyncLoad(void) const
{
	return ATOM_GUIRenderer::isMultiThreadingEnabled() && (_loadPriority != ATOM_LoadPriority_IMMEDIATE);
}
const ATOM_STRING & ATOM_GUIImage::getTexFileName(ATOM_WidgetState state) const
{
	return _states[state].textureFileName;
}
//----------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_Material)> ATOM_GUIImageList::_guiMaterialCache;

ATOM_GUIImageList::ATOM_GUIImageList (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::ATOM_GUIImageList);

	clear ();
}

ATOM_GUIImageList::~ATOM_GUIImageList (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::~ATOM_GUIImageList);

	clear ();
}

ATOM_GUIImageList::ATOM_GUIImageList (const ATOM_GUIImageList &other)
{
	for (ATOM_HASHMAP<int, ATOM_GUIImage*>::const_iterator it = other._images.begin(); it != other._images.end(); ++it)
	{
		_images[it->first] = ATOM_NEW(ATOM_GUIImage, *(it->second));
	}
}

ATOM_GUIImageList &ATOM_GUIImageList::operator = (const ATOM_GUIImageList &other)
{
	ATOM_GUIImageList tmp(other);
	swap (tmp);
	return *this;
}

void ATOM_GUIImageList::swap (ATOM_GUIImageList &other)
{
	std::swap (_images, other._images);
}

bool ATOM_GUIImageList::load(const char* filename)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::load);

	ATOM_AutoFile file(filename, ATOM_VFS::text|ATOM_VFS::read);
	if(file == NULL)
	{
		return false;
	}

	ATOM_TiXmlDocument doc;
	int len = file->size();
	char* buffer = ATOM_NEW_ARRAY(char, len+1);
	assert(buffer);
	int nRead = file->read(buffer, len);
	buffer[nRead] = 0;
	doc.Parse(buffer);
	ATOM_DELETE_ARRAY(buffer);

	if (doc.Error ())
	{
		ATOM_LOGGER::error ("%s(%s) %s\n", __FUNCTION__, filename, doc.ErrorDesc());
		return false;
	}

	ATOM_TiXmlElement* root = doc.RootElement();
	if(NULL == root)
	{
		return false;
	}

	int id = 0;
	int type = 0;
	int color = 0;
	const char* region_str = NULL;
	ATOM_Rect2Di region;
	const char *texNormal;
	const char *texHover;
	const char *texHold;
	const char *texDisabled;
	const char *matNormal;
	const char *matHover;
	const char *matHold;
	const char *matDisabled;
	unsigned clrNormal;
	unsigned clrHover;
	unsigned clrHold;
	unsigned clrDisabled;
	ATOM_TiXmlElement* elem = root->FirstChildElement();
	for(; elem; elem = elem->NextSiblingElement())
	{
		if(NULL == elem->Attribute("id", &id) )
		{
			continue;
		}

		if(NULL == elem->Attribute("type", &type))
		{
			continue;
		}

		ATOM_GUIImage *image = 0;

		switch (type)
		{
		case ATOM_GUIImage::IT_NULL:
			image = newNullImage(id);
			break;

		case ATOM_GUIImage::IT_COLOR:
			clrNormal = elem->Attribute("normal", &color) ? (unsigned)color : 0;
			clrHover = elem->Attribute("hover", &color) ? (unsigned)color : 0;
			clrHold = elem->Attribute("hold", &color) ? (unsigned)color : 0;
			clrDisabled = elem->Attribute("disabled", &color) ? (unsigned)color : 0;
			image = newColorImage(id, clrNormal, clrHover, clrHold, clrDisabled);
			break;

		case ATOM_GUIImage::IT_IMAGE:
			texNormal = elem->Attribute("normal");
			texHover = elem->Attribute("hover");
			texHover = texHover ? texHover : texNormal;
			texHold = elem->Attribute("hold");
			texHold = texHold ? texHold : texNormal;
			texDisabled = elem->Attribute("disabled");
			texDisabled = texDisabled ? texDisabled : texNormal;

			region_str = elem->Attribute("region");
			region = ATOM_Rect2Di(0,0,0,0);
			if(region_str)
			{
				sscanf(region_str, "%d,%d,%d,%d", &region.point.x, &region.point.y, &region.size.w, &region.size.h);
			}
			image = newTextureImage(id, texNormal, texHover, texHold, texDisabled, region);
			break;

		case ATOM_GUIImage::IT_IMAGE9:
			texNormal = elem->Attribute("normal");
			texHover = elem->Attribute("hover");
			texHover = texHover ? texHover : texNormal;
			texHold = elem->Attribute("hold");
			texHold = texHold ? texHold : texNormal;
			texDisabled = elem->Attribute("disabled");
			texDisabled = texDisabled ? texDisabled : texNormal;

			region_str = elem->Attribute("region");
			if(region_str)
			{
				sscanf(region_str, "%d,%d,%d,%d", &region.point.x, &region.point.y, &region.size.w, &region.size.h);
				image = newTextureImage9(id, texNormal, texHover, texHold, texDisabled, region);
			}
			break;

		case ATOM_GUIImage::IT_MATERIAL:
			texNormal = elem->Attribute("normal");
			texHover = elem->Attribute("hover");
			texHover = texHover ? texHover : texNormal;
			texHold = elem->Attribute("hold");
			texHold = texHold ? texHold : texNormal;
			texDisabled = elem->Attribute("disabled");
			texDisabled = texDisabled ? texDisabled : texNormal;

			matNormal = elem->Attribute("mat_normal");
			matHover = elem->Attribute("mat_hover");
			matHover = matHover ? matHover : matNormal;
			matHold = elem->Attribute("mat_hold");
			matHold = matHold ? matHold : matNormal;
			matDisabled = elem->Attribute("mat_disabled");
			matDisabled = matDisabled ? matDisabled : matNormal;

			region_str = elem->Attribute("region");
			region = ATOM_Rect2Di(0,0,0,0);
			if(region_str)
			{
				sscanf(region_str, "%d,%d,%d,%d", &region.point.x, &region.point.y, &region.size.w, &region.size.h);
			}
			image = newMaterialImage(id, texNormal, texHover, texHold, texDisabled, matNormal, matHover, matHold, matDisabled);
			break;
		}

		//--- wangjian added ----//
		// 异步加载:添加一个是否异步加载的标记
		if( image )
		{
			int asyncload = 0;
			if( elem->Attribute("asyncload", &asyncload) )
			{
				image->setAsyncLoad(asyncload);					// 如果没有给定，则默认为异步加载，优先级为1000
			}
			bool bAllowMT = ATOM_AsyncLoader::IsRun() && ATOM_GUIRenderer::isMultiThreadingEnabled();
			if( image->getAsyncLoad()!=ATOM_LoadPriority_IMMEDIATE && !bAllowMT )
			{
				image->setAsyncLoad(ATOM_LoadPriority_IMMEDIATE);	// 如果不允许异步加载 设置直接加载
			}
		}
		//-----------------------//
	}

	return true;
}

ATOM_GUIImage *ATOM_GUIImageList::getImage (int id) const
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::getImage);

	ATOM_HASHMAP<int, ATOM_GUIImage*>::const_iterator it = _images.find (id);
	return (it == _images.end ()) ? 0 : it->second;
}

ATOM_GUIImage *ATOM_GUIImageList::newImage (int id)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newImage);

	ATOM_HASHMAP<int, ATOM_GUIImage*>::const_iterator it = _images.find (id);
	if (it == _images.end ())
	{
		ATOM_GUIImage *image = ATOM_NEW(ATOM_GUIImage);
		_images[id] = image;
		return image;
	}
	return 0;
}

ATOM_GUIImage *ATOM_GUIImageList::newMaterialImage (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const char *matNormal, const char *matHover, const char *matHold, const char *matDisabled, const ATOM_Rect2Di &region)
{
	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_MATERIAL);
		image->setImage (WST_NORMAL, texNormal);
		image->setMaterial (WST_NORMAL, matNormal);
		image->setRegion (WST_NORMAL, region);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_MATERIAL);
		image->setImage (WST_HOVER, texHover);
		image->setMaterial (WST_HOVER, matHover);
		image->setRegion (WST_HOVER, region);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_MATERIAL);
		image->setImage (WST_HOLD, texHold);
		image->setMaterial (WST_HOLD, matHold);
		image->setRegion (WST_HOLD, region);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_MATERIAL);
		image->setImage (WST_DISABLED, texDisabled);
		image->setMaterial (WST_DISABLED, matDisabled);
		image->setRegion (WST_DISABLED, region);
	}
	return image;
}

ATOM_GUIImage *ATOM_GUIImageList::newNullImage (int id)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newNullImage);

	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_NULL);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_NULL);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_NULL);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_NULL);
	}
	return image;
}

ATOM_GUIImage *ATOM_GUIImageList::newVGImage (int id)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newVGImage);

	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_VG);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_VG);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_VG);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_VG);
	}
	return image;
}

ATOM_GUIImage *ATOM_GUIImageList::newColorImage (int id, ATOM_ColorARGB clrNormal, ATOM_ColorARGB clrHover, ATOM_ColorARGB clrHold, ATOM_ColorARGB clrDisabled)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newColorImage);

	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_COLOR);
		image->setColor (WST_NORMAL, clrNormal);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_COLOR);
		image->setColor (WST_HOVER, clrHover);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_COLOR);
		image->setColor (WST_HOLD, clrHold);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_COLOR);
		image->setColor (WST_DISABLED, clrDisabled);
	}
	return image;
}

ATOM_GUIImage *ATOM_GUIImageList::newTextureImage (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const ATOM_Rect2Di &region)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newTextureImage);

	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_IMAGE);
		image->setImage (WST_NORMAL, texNormal);
		image->setRegion(WST_NORMAL, region);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_IMAGE);
		image->setImage (WST_HOVER, texHover);
		image->setRegion(WST_HOVER, region);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_IMAGE);
		image->setImage (WST_HOLD, texHold);
		image->setRegion(WST_HOLD, region);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_IMAGE);
		image->setImage (WST_DISABLED, texDisabled);
		image->setRegion(WST_DISABLED, region);
	}
	return image;
}

ATOM_GUIImage *ATOM_GUIImageList::newTextureImage9 (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const ATOM_Rect2Di &region)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::newTextureImage9);

	ATOM_GUIImage *image = newImage (id);
	if (image)
	{
		image->setType (WST_NORMAL, ATOM_GUIImage::IT_IMAGE9);
		image->setImage (WST_NORMAL, texNormal);
		image->setRegion(WST_NORMAL, region);
		image->setType (WST_HOVER, ATOM_GUIImage::IT_IMAGE9);
		image->setImage (WST_HOVER, texHover);
		image->setRegion(WST_HOVER, region);
		image->setType (WST_HOLD, ATOM_GUIImage::IT_IMAGE9);
		image->setImage (WST_HOLD, texHold);
		image->setRegion(WST_HOLD, region);
		image->setType (WST_DISABLED, ATOM_GUIImage::IT_IMAGE9);
		image->setImage (WST_DISABLED, texDisabled);
		image->setRegion(WST_DISABLED, region);
	}
	return image;
}

void ATOM_GUIImageList::removeImage (int id)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::removeImage);

	ATOM_HASHMAP<int, ATOM_GUIImage*>::iterator it = _images.find (id);
	if (it != _images.end ())
	{
		ATOM_DELETE(it->second);
		_images.erase (it);
	}
}

void ATOM_GUIImageList::clear (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::clear);

	ATOM_HASHMAP<int, ATOM_GUIImage*>::iterator it = _images.begin ();
	for (; it != _images.end (); ++it)
	{
		ATOM_DELETE(it->second);
	}
	_images.clear ();
}

void ATOM_GUIImageList::loadDefaults (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::loadDefaults);

	clear ();

	newColorImage (ATOM_IMAGEID_WINDOW_BKGROUND, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	newColorImage (ATOM_IMAGEID_CONTROL_BKGROUND, 0xFF808080, 0xFF808080, 0xFF808080, 0xFF808080);
	newNullImage (ATOM_IMAGEID_LISTITEM);
	newColorImage (ATOM_IMAGEID_LISTITEM_HILIGHT, 0xFF8080FF, 0xFF8080FF, 0xFF8080FF, 0xFF8080FF);
	newNullImage (ATOM_IMAGEID_MENUITEM);
	newColorImage (ATOM_IMAGEID_MENUITEM_HILIGHT, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	newColorImage (ATOM_IMAGEID_MULTICHOICE_UNCHECKED, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF202020);
	newColorImage (ATOM_IMAGEID_MULTICHOICE_CHECKED, 0xFF808080, 0xFF808080, 0xFF808080, 0xFF808080);
	newColorImage (ATOM_IMAGEID_SINGLECHOICE_UNCHECKED, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF202020);
	newColorImage (ATOM_IMAGEID_SINGLECHOICE_CHECKED, 0xFF808080, 0xFF808080, 0xFF808080, 0xFF808080);
	newNullImage (ATOM_IMAGEID_TREEITEM);
	newColorImage (ATOM_IMAGEID_TREEITEM_HILIGHT, 0xFF6060C0, 0xFF6060C0, 0xFF6060C0, 0xFF6060C0);
	newColorImage (ATOM_IMAGEID_TREEBUTTON_COLLAPSED, 0xFF202020, 0xFF202020, 0xFF202020, 0xFF202020);
	newColorImage (ATOM_IMAGEID_TREEBUTTON_EXPANDED, 0xFF808080, 0xFF808080, 0xFF808080, 0xFF808080);
	newColorImage (ATOM_IMAGEID_PROGRESSBAR, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF);
	newColorImage (ATOM_IMAGEID_EDIT_CURSOR, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000);
	newColorImage (ATOM_IMAGEID_BLACK, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000);
	newColorImage (ATOM_IMAGEID_WHITE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	newColorImage (ATOM_IMAGEID_RED, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000);
	newColorImage (ATOM_IMAGEID_GREEN, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00);
	newColorImage (ATOM_IMAGEID_BLUE, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF);

	ATOM_VectorGraph *vg = 0;

	ATOM_Vector2f vScrollButtonUpPoints[3] = {
		ATOM_Vector2f(0.5f, 0.1f),
		ATOM_Vector2f(0.1f, 0.9f),
		ATOM_Vector2f(0.9f, 0.9f)
	};
	ATOM_Vector2f vScrollButtonDownPoints[3] = {
		ATOM_Vector2f(0.1f, 0.1f),
		ATOM_Vector2f(0.9f, 0.1f),
		ATOM_Vector2f(0.5f, 0.9f)
	};
	ATOM_Vector2f hScrollButtonUpPoints[3] = {
		ATOM_Vector2f(0.1f, 0.5f),
		ATOM_Vector2f(0.9f, 0.9f),
		ATOM_Vector2f(0.9f, 0.1f)
	};
	ATOM_Vector2f hScrollButtonDownPoints[3] = {
		ATOM_Vector2f(0.1f, 0.1f),
		ATOM_Vector2f(0.1f, 0.9f),
		ATOM_Vector2f(0.9f, 0.5f)
	};

	ATOM_GUIImage *imgScrollbarHBtnUp = newVGImage (ATOM_IMAGEID_SCROLLBAR_HBUTTONUP);
	vg = imgScrollbarHBtnUp->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnUp->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnUp->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnUp->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonUpPoints, 0, 0xFF909090, 0);

	ATOM_GUIImage *imgScrollbarHBtnDown = newVGImage (ATOM_IMAGEID_SCROLLBAR_HBUTTONDOWN);
	vg = imgScrollbarHBtnDown->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnDown->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnDown->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarHBtnDown->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, hScrollButtonDownPoints, 0, 0xFF909090, 0);

	ATOM_GUIImage *imgScrollbarHSlider = newVGImage (ATOM_IMAGEID_SCROLLBAR_HSLIDER);
	vg = imgScrollbarHSlider->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarHSlider->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarHSlider->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarHSlider->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);

	ATOM_GUIImage *imgScrollbarVBtnUp = newVGImage (ATOM_IMAGEID_SCROLLBAR_VBUTTONUP);
	vg = imgScrollbarVBtnUp->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnUp->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnUp->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonUpPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnUp->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonUpPoints, 0, 0xFF909090, 0);

	ATOM_GUIImage *imgScrollbarVBtnDown = newVGImage (ATOM_IMAGEID_SCROLLBAR_VBUTTONDOWN);
	vg = imgScrollbarVBtnDown->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnDown->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnDown->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonDownPoints, 0, 0xFFFFFFFF, 0);
	vg = imgScrollbarVBtnDown->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (3, vScrollButtonDownPoints, 0, 0xFF909090, 0);

	ATOM_GUIImage *imgScrollbarVSlider = newVGImage (ATOM_IMAGEID_SCROLLBAR_VSLIDER);
	vg = imgScrollbarVSlider->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarVSlider->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarVSlider->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);
	vg = imgScrollbarVSlider->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addDrawRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF);

	ATOM_GUIImage *imgSliderHandle = newVGImage (ATOM_IMAGEID_SLIDER_HANDLE);
	vg = imgSliderHandle->getVectorGraph(WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF, 0);
	vg = imgSliderHandle->getVectorGraph(WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF, 0);
	vg = imgSliderHandle->getVectorGraph(WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFFFFFFFF, 0);
	vg = imgSliderHandle->getVectorGraph(WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);

	ATOM_GUIImage *imgSliderHRuler = newVGImage (ATOM_IMAGEID_SLIDER_HRULER);
	vg = imgSliderHRuler->getVectorGraph(WST_NORMAL);
	vg->addDrawLineOp (ATOM_Vector2f(0.f, 0.5f), ATOM_Vector2f(1.f, 0.5f), 0xFF202020);
	vg = imgSliderHRuler->getVectorGraph(WST_HOVER);
	vg->addDrawLineOp (ATOM_Vector2f(0.f, 0.5f), ATOM_Vector2f(1.f, 0.5f), 0xFF202020);
	vg = imgSliderHRuler->getVectorGraph(WST_HOLD);
	vg->addDrawLineOp (ATOM_Vector2f(0.f, 0.5f), ATOM_Vector2f(1.f, 0.5f), 0xFF202020);
	vg = imgSliderHRuler->getVectorGraph(WST_DISABLED);
	vg->addDrawLineOp (ATOM_Vector2f(0.f, 0.5f), ATOM_Vector2f(1.f, 0.5f), 0xFF808080);

	ATOM_GUIImage *imgSliderVRuler = newVGImage (ATOM_IMAGEID_SLIDER_VRULER);
	vg = imgSliderVRuler->getVectorGraph(WST_NORMAL);
	vg->addDrawLineOp (ATOM_Vector2f(0.5f, 0.f), ATOM_Vector2f(0.5f, 1.f), 0xFF202020);
	vg = imgSliderVRuler->getVectorGraph(WST_HOVER);
	vg->addDrawLineOp (ATOM_Vector2f(0.5f, 0.f), ATOM_Vector2f(0.5f, 1.f), 0xFF202020);
	vg = imgSliderVRuler->getVectorGraph(WST_HOLD);
	vg->addDrawLineOp (ATOM_Vector2f(0.5f, 0.f), ATOM_Vector2f(0.5f, 1.f), 0xFF202020);
	vg = imgSliderVRuler->getVectorGraph(WST_DISABLED);
	vg->addDrawLineOp (ATOM_Vector2f(0.5f, 0.f), ATOM_Vector2f(0.5f, 1.f), 0xFF808080);

	const ATOM_Vector2f checkPolygonPoints1[4] = {
		ATOM_Vector2f(0.1f, 0.5f),
		ATOM_Vector2f(0.4f, 0.9f),
		ATOM_Vector2f(0.4f, 0.7f),
		ATOM_Vector2f(0.2f, 0.4f)
	};
	const ATOM_Vector2f checkPolygonPoints2[4] = {
		ATOM_Vector2f(0.4f, 0.7f),
		ATOM_Vector2f(0.4f, 0.9f),
		ATOM_Vector2f(0.9f, 0.3f),
		ATOM_Vector2f(0.8f, 0.1f)
	};

	ATOM_GUIImage *imgCheckBox = newVGImage (ATOM_IMAGEID_CHECKBOX_CHECKED);
	vg = imgCheckBox->getVectorGraph (WST_NORMAL);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgCheckBox->getVectorGraph (WST_HOVER);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgCheckBox->getVectorGraph (WST_HOLD);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgCheckBox->getVectorGraph (WST_DISABLED);
	vg->addFillRectOp (ATOM_Vector4f(0.f, 0.f, 1.f, 1.f), 0xFF808080, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);

	newColorImage (ATOM_IMAGEID_CHECKBOX_UNCHECKED, 0xFF808080, 0xFF808080, 0xFF808080, 0xFF808080);

	ATOM_GUIImage *imgChecked = newVGImage (ATOM_IMAGEID_CHECKED);
	vg = imgChecked->getVectorGraph (WST_NORMAL);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgChecked->getVectorGraph (WST_HOVER);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgChecked->getVectorGraph (WST_HOLD);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);
	vg = imgChecked->getVectorGraph (WST_DISABLED);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints1, 0, 0xFF000000, 0);
	vg->addFillConvexPolygonOp (4, checkPolygonPoints2, 0, 0xFF000000, 0);

	ATOM_GUIImage *imgTreeCtrlCollapsed = newVGImage (ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE);
	const ATOM_Vector2f points[12] = {
		ATOM_Vector2f(0.f, 0.f),
		ATOM_Vector2f(1.f, 0.f),
		ATOM_Vector2f(1.f, 1.f),
		ATOM_Vector2f(0.f, 1.f),
		ATOM_Vector2f(0.5f, 0.3f),
		ATOM_Vector2f(0.3f, 0.5f),

		ATOM_Vector2f(1.f, 0.f),
		ATOM_Vector2f(1.f, 1.f),
		ATOM_Vector2f(0.f, 1.f),
		ATOM_Vector2f(0.f, 0.f),
		ATOM_Vector2f(0.5f, 0.9f),
		ATOM_Vector2f(0.9f, 0.5f)
	};
	vg = imgTreeCtrlCollapsed->getVectorGraph (WST_NORMAL);
	vg->addDrawLineListOp (points, 6, 0xFFFFFFFF);
	vg = imgTreeCtrlCollapsed->getVectorGraph (WST_HOVER);
	vg->addDrawLineListOp (points, 6, 0xFFFFFFFF);
	vg = imgTreeCtrlCollapsed->getVectorGraph (WST_HOLD);
	vg->addDrawLineListOp (points, 6, 0xFFFFFFFF);
	vg = imgTreeCtrlCollapsed->getVectorGraph (WST_DISABLED);
	vg->addDrawLineListOp (points, 6, 0xFFFFFFFF);

	ATOM_GUIImage *imgTreeCtrlExpanded = newVGImage (ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE);
	const ATOM_Vector2f pointsE[10] = {
		ATOM_Vector2f(0.f, 0.f),
		ATOM_Vector2f(1.f, 0.f),
		ATOM_Vector2f(1.f, 1.f),
		ATOM_Vector2f(0.f, 1.f),
		ATOM_Vector2f(0.3f, 0.5f),
		ATOM_Vector2f(1.f, 0.f),
		ATOM_Vector2f(1.f, 1.f),
		ATOM_Vector2f(0.f, 1.f),
		ATOM_Vector2f(0.f, 0.f),
		ATOM_Vector2f(0.9f, 0.5f)
	};
	vg = imgTreeCtrlExpanded->getVectorGraph (WST_NORMAL);
	vg->addDrawLineListOp (pointsE, 5, 0xFFFFFFFF);
	vg = imgTreeCtrlExpanded->getVectorGraph (WST_HOVER);
	vg->addDrawLineListOp (pointsE, 5, 0xFFFFFFFF);
	vg = imgTreeCtrlExpanded->getVectorGraph (WST_HOLD);
	vg->addDrawLineListOp (pointsE, 5, 0xFFFFFFFF);
	vg = imgTreeCtrlExpanded->getVectorGraph (WST_DISABLED);
	vg->addDrawLineListOp (pointsE, 5, 0xFFFFFFFF);
}

struct DefaultImageList
{
	ATOM_GUIImageList imagelist;

	DefaultImageList (void)
	{
		ATOM_STACK_TRACE(DefaultImageList::DefaultImageList);

		imagelist.loadDefaults ();
	}
};

const ATOM_GUIImageList& ATOM_GUIImageList::getDefaultImageList (void)
{
	ATOM_STACK_TRACE(ATOM_GUIImageList::getDefaultImageList);

	static const DefaultImageList imagelist;
	return imagelist.imagelist;
}

ATOM_GUIImageList::ImageMap &ATOM_GUIImageList::getImages (void)
{
	return _images;
}

const ATOM_GUIImageList::ImageMap &ATOM_GUIImageList::getImages (void) const
{
	return _images;
}

ATOM_AUTOPTR(ATOM_Material) ATOM_GUIImageList::allocMaterial (const char *filename)
{
	ATOM_AUTOPTR(ATOM_Material) proto = nullptr;
	ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_Material)>::const_iterator it = _guiMaterialCache.find (filename);
	if (it == _guiMaterialCache.end ())
	{
		proto = ATOM_MaterialManager::createMaterialFromFile(ATOM_GetRenderDevice(), filename);
		if (!proto)
		{
			return nullptr;
		}
		proto->setActiveEffect ("default");
		_guiMaterialCache[filename] = proto;
	}
	else
	{
		proto = it->second;
	}

	return proto->clone ();
}

