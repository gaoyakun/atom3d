#ifndef __ATOM3D_ENGINE_IMAGELIST_H
#define __ATOM3D_ENGINE_IMAGELIST_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "../ATOM_render.h"
#include "basedefs.h"
#include "gui_misc.h"

// predefined image id
#define ATOM_AUTOMATIC_IMAGEID					-2
#define ATOM_INVALID_IMAGEID					-1
#define ATOM_NULL_IMAGEID						-1
#define ATOM_IMAGEID_WINDOW_BKGROUND			0
#define ATOM_IMAGEID_CONTROL_BKGROUND			1
#define ATOM_IMAGEID_LISTITEM					2
#define ATOM_IMAGEID_LISTITEM_HILIGHT			3
#define ATOM_IMAGEID_MENUITEM					4
#define ATOM_IMAGEID_MENUITEM_HILIGHT			5
#define ATOM_IMAGEID_MULTICHOICE_UNCHECKED		6
#define ATOM_IMAGEID_MULTICHOICE_CHECKED		7
#define ATOM_IMAGEID_SINGLECHOICE_UNCHECKED		8
#define ATOM_IMAGEID_SINGLECHOICE_CHECKED		9
#define ATOM_IMAGEID_TREEITEM					10
#define ATOM_IMAGEID_TREEITEM_HILIGHT			11
#define ATOM_IMAGEID_TREEBUTTON_COLLAPSED		12
#define ATOM_IMAGEID_TREEBUTTON_EXPANDED		13
#define ATOM_IMAGEID_PROGRESSBAR				14
#define ATOM_IMAGEID_SCROLLBAR_HBUTTONUP		15
#define ATOM_IMAGEID_SCROLLBAR_HBUTTONDOWN		16
#define ATOM_IMAGEID_SCROLLBAR_HSLIDER			17
#define ATOM_IMAGEID_SCROLLBAR_VBUTTONUP		18
#define ATOM_IMAGEID_SCROLLBAR_VBUTTONDOWN		19
#define ATOM_IMAGEID_SCROLLBAR_VSLIDER			20
#define ATOM_IMAGEID_SLIDER_HANDLE				21
#define ATOM_IMAGEID_SLIDER_HRULER				22
#define ATOM_IMAGEID_SLIDER_VRULER				23
#define ATOM_IMAGEID_EDIT_CURSOR				24
#define ATOM_IMAGEID_CHECKBOX_CHECKED			25
#define ATOM_IMAGEID_CHECKBOX_UNCHECKED			26
#define ATOM_IMAGEID_CHECKED					27
#define ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE	28
#define ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE	29
#define ATOM_IMAGEID_BLACK						30
#define ATOM_IMAGEID_GREEN						32
#define ATOM_IMAGEID_RED						39
#define ATOM_IMAGEID_BLUE						42
#define ATOM_IMAGEID_WHITE						47


#define ATOM_IMAGEID_USER						200
#define ATOM_IMAGEID_APP						600



class ATOM_GUICanvas;
class ATOM_VectorGraph;
//class SwappableTexture;

class ATOM_ENGINE_API ATOM_GUIImage
{
public:
	enum Type
	{
		IT_NULL,
		IT_IMAGE,
		IT_COLOR,
		IT_IMAGE9,
		IT_MATERIAL,
		IT_VG
	};

	enum DrawStyle
	{
		IDS_REPEAT,
		IDS_STRETCH
	};

public:
	ATOM_GUIImage (void);
	ATOM_GUIImage (const ATOM_GUIImage &other);
	~ATOM_GUIImage (void);
	ATOM_GUIImage &operator = (const ATOM_GUIImage &other);
	void swap (ATOM_GUIImage &other);

public:
	void setType (ATOM_WidgetState state, ATOM_GUIImage::Type type);
	void setImage (ATOM_WidgetState state, const char *filename);
	void setImage (ATOM_WidgetState state, ATOM_Texture *texture);
	void setMaterial (ATOM_WidgetState state, ATOM_Material *material);
	void setMaterial (ATOM_WidgetState state, const char *filename);
	void setColor (ATOM_WidgetState state, ATOM_ColorARGB color);
	void setRegion (ATOM_WidgetState state, const ATOM_Rect2Di &region);
	ATOM_Texture *getTexture (ATOM_WidgetState state);
	ATOM_GUIImage::Type getType (ATOM_WidgetState state);
	unsigned getTextureWidth (ATOM_WidgetState state);
	unsigned getTextureHeight (ATOM_WidgetState state);
	ATOM_Material *getMaterial (ATOM_WidgetState state);
	ATOM_ColorARGB getColor (ATOM_WidgetState state);
	const ATOM_Rect2Di &getRegion (ATOM_WidgetState state);
	ATOM_VectorGraph *getVectorGraph (ATOM_WidgetState state);
	void draw (ATOM_WidgetState state, ATOM_GUICanvas *canvas, const ATOM_Rect2Di &rect, ATOM_ColorARGB* customColor = NULL, float rotation = 0.f);
	void drawRect (ATOM_WidgetState state, ATOM_GUICanvas *canvas, const ATOM_Rect2Di &rect, const ATOM_Rect2Di &rectTexture, ATOM_ColorARGB* customColor = NULL, float rotation = 0.f);
	//--- wangjian added ---//
	// 异步加载标记
	void	setAsyncLoad(int async);
	int		getAsyncLoad(void) const;
	bool	isAsyncLoad(void) const;
	const ATOM_STRING & getTexFileName(ATOM_WidgetState state) const;
	//----------------------//
private:
	void updateState (ATOM_WidgetState state);

private:
	struct ImageState
	{
		bool textureDirty;
		ATOM_GUIImage::Type type;
		ATOM_STRING textureFileName;
		ATOM_AUTOREF(ATOM_Texture) texture;
		ATOM_ColorARGB color;
		ATOM_Rect2Di region;
		unsigned width;
		unsigned height;
		ATOM_AUTOPTR(ATOM_Material) material;
		ATOM_VectorGraph *vg;
	};
	ImageState _states[WST_COUNT];

	//--- wangjian added ---//
	// 异步加载标记
	enum
	{
		GUIIMAGE_LOADPRI_BASE = 5000,		// GUI 图片 非常高优先级
	};
	int		_loadPriority;
	//----------------------//

	ATOM_DECLARE_WEAK_REF
};

class ATOM_ENGINE_API ATOM_GUIImageList: public ATOM_ReferenceObj
{
public:
	typedef ATOM_HASHMAP<int, ATOM_GUIImage*> ImageMap;

public:
	ATOM_GUIImageList (void);
	ATOM_GUIImageList (const ATOM_GUIImageList &other);
	~ATOM_GUIImageList (void);
	ATOM_GUIImageList &operator = (const ATOM_GUIImageList &other);
	void swap (ATOM_GUIImageList &other);

public:
	bool load(const char* filename);
	ATOM_GUIImage *getImage (int id) const;
	ATOM_GUIImage *newImage (int id);
	ATOM_GUIImage *newColorImage (int id, ATOM_ColorARGB clrNormal, ATOM_ColorARGB clrHover, ATOM_ColorARGB clrHold, ATOM_ColorARGB clrDisabled);
	ATOM_GUIImage *newTextureImage (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const ATOM_Rect2Di &region=ATOM_Rect2Di(0,0,0,0));
	ATOM_GUIImage *newTextureImage9 (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const ATOM_Rect2Di &region=ATOM_Rect2Di(0,0,0,0));
	ATOM_GUIImage *newMaterialImage (int id, const char *texNormal, const char *texHover, const char *texHold, const char *texDisabled, const char *matNormal, const char *matHover, const char *matHold, const char *matDisabled, const ATOM_Rect2Di &region=ATOM_Rect2Di(0,0,0,0));
	ATOM_GUIImage *newNullImage (int id);
	ATOM_GUIImage *newVGImage (int id);
	void removeImage (int id);
	void clear (void);
	void loadDefaults (void);
	ImageMap &getImages (void);
	const ImageMap &getImages (void) const;

public:
	static const ATOM_GUIImageList &getDefaultImageList (void);
	static ATOM_AUTOPTR(ATOM_Material) allocMaterial (const char *filename);

private:
	ATOM_HASHMAP<int, ATOM_GUIImage*> _images;
	static ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_Material)> _guiMaterialCache;
};


#endif // __ATOM3D_ENGINE_IMAGELIST_H
