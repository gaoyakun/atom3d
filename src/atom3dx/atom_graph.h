#ifndef __ATOMX_ATOM_GRAPH_H
#define __ATOMX_ATOM_GRAPH_H

#include "basedefs.h"
#include "tw_wrapper.h"

class CAtom3DGraph : public ITwGraph
{
public:
	CAtom3DGraph (ATOM_RenderDevice *device);

public:
    virtual int                 Init();
    virtual int                 Shut();
    virtual void                BeginDraw(int _WndWidth, int _WndHeight);
    virtual void                EndDraw();
    virtual bool                IsDrawing();
    virtual void                Restore();
    virtual void                DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color0, color32 _Color1, bool _AntiAliased=false);
    virtual void                DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color, bool _AntiAliased=false);
    virtual void                DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color00, color32 _Color10, color32 _Color01, color32 _Color11);
    virtual void                DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color);
    virtual void                DrawTriangles(int _NumTriangles, int *_Vertices, color32 *_Colors, Cull _CullMode);

    virtual void *              NewTextObj();
    virtual void                DeleteTextObj(void *_TextObj);
    virtual void                BuildText(void *_TextObj, const char **_TextLines, color32 *_LineColors, color32 *_LineBgColors, int _NbLines, const CTexFont *_Font, int _Sep, int _BgWidth);
    virtual void                DrawText(void *_TextObj, int _X, int _Y, color32 _Color, color32 _BgColor);

    virtual void                ChangeViewport(int _X0, int _Y0, int _Width, int _Height, int _OffsetX, int _OffsetY);
    virtual void                RestoreViewport();
	virtual void				DeleteThis();

private:
    struct CTextVtx
    {
        float		_pos[4];
        unsigned	_color;
        float		_uv[2];
    };
    struct CBgVtx
    {
        float		_pos[4];
        unsigned    _color;
    };

    struct CTextObj
    {
        ATOM_VECTOR<CTextVtx>   _textVerts;
        ATOM_VECTOR<CBgVtx>     _bgVerts;
        bool                    _lineColors;
        bool                    _lineBgColors;
    };

    struct CTriVtx
    {
        float m_Pos[4];
        unsigned m_Color;
    };
    ATOM_VECTOR<CTriVtx>        _triVertices;
    const CTexFont *            _fontTex;

	bool _isDrawing;
	int _offsetX;
	int _offsetY;

	ATOM_RenderDevice *_device;
	ATOM_Rect2Di _savedViewport;
	unsigned _wWidth;
	unsigned _wHeight;

	ATOM_AUTOPTR(ATOM_Material) _twMaterial;
	ATOM_AUTOPTR(ATOM_Material) _twMaterialText;
	ATOM_AUTOREF(ATOM_Texture) _fontTexture;

	ATOM_VECTOR<unsigned short> _indicesCache;
	void resizeIndicesCache (unsigned count);
};

inline void* TW_CALL ATOMGraphCreate (TwGraphAPI graphAPI, void *clientData)
{
	return (graphAPI == TW_DIRECT3D9) ? ATOM_NEW(CAtom3DGraph, (ATOM_RenderDevice*)clientData) : 0;
}

#endif // __ATOMX_ATOM_GRAPH_H
