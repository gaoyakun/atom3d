#include "StdAfx.h"
#include "atom_graph.h"
#include "atom_twfont.h"

#define USE_ATOM3D_TEXT 1

static const char *twMaterial = 
	"<?xml version=\"1.0\"?>											\n"
	"<material auto=\"0\">												\n"
	"	<param name=\"cullmode\" type=\"int\" />						\n"
	"	<code><![CDATA[													\n"
	"		struct v2p													\n"
	"		{															\n"
	"			float4 position : POSITION0;							\n"
	"			float4 diffuse : COLOR0;								\n"
	"		};															\n"
	"		float4 ps (in v2p IN) : COLOR0								\n"
	"		{															\n"
	"			return IN.diffuse;										\n"
	"		}															\n"
	"	]]></code>														\n"
	"	<effect name=\"default\">										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" type=\"float3rhw\"/>	\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<alphablend_state>										\n"
	"				<enable value=\"true\" />							\n"
	"				<srcblend value=\"srcalpha\" />						\n"
	"				<destblend value=\"invsrcalpha\" />					\n"
	"			</alphablend_state>										\n"
	"			<depth_state>											\n"
	"				<zfunc value=\"always\" />							\n"
	"				<zwrite value=\"false\" />							\n"
	"			</depth_state>											\n"
	"			<rasterizer_state>										\n"
	"				<cullmode value=\"$cullmode\" />					\n"
	"			</rasterizer_state>										\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"		
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

static const char *twTextMaterial = 
"<?xml version=\"1.0\"?>												\n"
	"<material auto=\"0\">												\n"
	"	<param name=\"cullmode\" type=\"int\" />						\n"
	"	<param name=\"texture\" type=\"texture\" />						\n"
	"	<param name=\"s\" type=\"sampler2D\">							\n"
	"		<filter value=\"ppn\"/>										\n"
	"		<addressu value=\"clamp\"/>									\n"
	"		<addressv value=\"clamp\"/>									\n"
	"		<texture value=\"$texture\"/>								\n"
	"	</param>														\n"
	"	<code><![CDATA[													\n"
	"		struct v2p													\n"
	"		{															\n"
	"			float4 position : POSITION0;							\n"
	"			float4 diffuse : COLOR0;								\n"
	"			float2 texcoord : TEXCOORD0;							\n"
	"		};															\n"
	"		float4 ps (in v2p IN) : COLOR0								\n"
	"		{															\n"
	"			float4 c = tex2D(s, IN.texcoord);						\n"
	"			c.rgb *= IN.diffuse.rgb;								\n"
	"			return c;												\n"
	"		}															\n"
	"	]]></code>														\n"
	"	<effect name=\"default\">										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" type=\"float3rhw\" />	\n"
	"				<stream semantec=\"texcoord0\" type=\"float2\" />	\n"
	"			</streamdefine>											\n"
	"			<alphablend_state>										\n"
	"				<enable value=\"true\" />							\n"
	"				<srcblend value=\"srcalpha\" />						\n"
	"				<destblend value=\"invsrcalpha\" />					\n"
	"			</alphablend_state>										\n"
	"			<depth_state>											\n"
	"				<zfunc value=\"always\" />							\n"
	"				<zwrite value=\"false\" />							\n"
	"			</depth_state>											\n"
	"			<rasterizer_state>										\n"
	"				<cullmode value=\"none\" />							\n"
	"			</rasterizer_state>										\n"
	"			<sampler_state index=\"0\">								\n"
	"				<filter value=\"ppp\" />							\n"
	"				<addressu value=\"clamp\" />						\n"
	"				<addressv value=\"clamp\" />						\n"
	"				<texture value=\"$texture\" />						\n"
	"			</sampler_state>										\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"		
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

CAtom3DGraph::CAtom3DGraph (ATOM_RenderDevice *device)
{
	_isDrawing = false;
    _offsetX = 0;
    _offsetY = 0;
	_device = device;
}

int CAtom3DGraph::Init()
{
	_twMaterial = ATOM_MaterialManager::createMaterialFromCoreString(_device, twMaterial);
	if (!_twMaterial)
	{
		return 0;
	}
	_twMaterial->setActiveEffect ("default");

	_twMaterialText = ATOM_MaterialManager::createMaterialFromCoreString(_device, twTextMaterial);
	if (!_twMaterialText)
	{
		return 0;
	}
	_twMaterialText->setActiveEffect ("default");

	return 1;
}

int CAtom3DGraph::Shut()
{
	return 1;
}

void CAtom3DGraph::BeginDraw(int _WndWidth, int _WndHeight)
{
	_isDrawing = true;
	_offsetX = 0;
	_offsetY = 0;

	_savedViewport = ATOM_GetRenderDevice()->getViewport (0);
	_wWidth = _WndWidth;
	_wHeight = _WndHeight;

	ATOM_GetRenderDevice()->setViewport (0, 0, 0, _wWidth, _wHeight);
}

void CAtom3DGraph::EndDraw()
{
	_isDrawing = false;
	ATOM_GetRenderDevice()->setViewport (0, _savedViewport);
}

bool CAtom3DGraph::IsDrawing()
{
	return _isDrawing;
}

void CAtom3DGraph::Restore()
{
}

void CAtom3DGraph::DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color0, color32 _Color1, bool _AntiAliased)
{
    struct CVtx
    {
        float m_Pos[4];
        DWORD m_Color;
    };
    CVtx p[2];

    p[0].m_Pos[0] = (float)(_X0 + _offsetX);
    p[0].m_Pos[1] = (float)(_Y0 + _offsetY);
    p[0].m_Pos[2] = 0;
    p[0].m_Pos[3] = 0;
    p[0].m_Color  = _Color0;

    p[1].m_Pos[0] = (float)(_X1 + _offsetX);
    p[1].m_Pos[1] = (float)(_Y1 + _offsetY);
    p[1].m_Pos[2] = 0;
    p[1].m_Pos[3] = 0;
    p[1].m_Color  = _Color1;

	resizeIndicesCache (2);

	_twMaterial->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	unsigned num = _twMaterial->begin (device);
	for (unsigned i = 0; i < num; ++i)
	{
		if (_twMaterial->beginPass(device, i))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_LINES, 2, 2, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(CVtx), p, &_indicesCache[0]);
			_twMaterial->endPass (device, i);
		}
	}
	_twMaterial->end (device);
}

void CAtom3DGraph::DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color, bool _AntiAliased) 
{
	DrawLine(_X0, _Y0, _X1, _Y1, _Color, _Color, _AntiAliased); 
}

void CAtom3DGraph::DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color00, color32 _Color10, color32 _Color01, color32 _Color11)
{
    // border adjustment
    if(_X0<_X1)
        ++_X1;
    else if(_X0>_X1)
        ++_X0;
    if(_Y0<_Y1)
        ++_Y1;
    else if(_Y0>_Y1)
        ++_Y0;

    struct CVtx
    {
        float m_Pos[4];
        DWORD m_Color;
    };
    CVtx p[4];

    p[0].m_Pos[0] = (float)(_X1 + _offsetX);
    p[0].m_Pos[1] = (float)(_Y0 + _offsetY);
    p[0].m_Pos[2] = 0;
    p[0].m_Pos[3] = 1;
    p[0].m_Color  = _Color10;

    p[1].m_Pos[0] = (float)(_X0 + _offsetX);
    p[1].m_Pos[1] = (float)(_Y0 + _offsetY);
    p[1].m_Pos[2] = 0;
    p[1].m_Pos[3] = 1;
    p[1].m_Color  = _Color00;

    p[2].m_Pos[0] = (float)(_X1 + _offsetX);
    p[2].m_Pos[1] = (float)(_Y1 + _offsetY);
    p[2].m_Pos[2] = 0;
    p[2].m_Pos[3] = 1;
    p[2].m_Color  = _Color11;

    p[3].m_Pos[0] = (float)(_X0 + _offsetX);
    p[3].m_Pos[1] = (float)(_Y1 + _offsetY);
    p[3].m_Pos[2] = 0;
    p[3].m_Pos[3] = 1;
    p[3].m_Color  = _Color01;

	resizeIndicesCache (4);

	_twMaterial->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);
	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	unsigned num = _twMaterial->begin (device);
	for (unsigned i = 0; i < num; ++i)
	{
		if (_twMaterial->beginPass(device, i))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(CVtx), p, &_indicesCache[0]);
			_twMaterial->endPass (device, i);
		}
	}
	_twMaterial->end (device);
}

void CAtom3DGraph::DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color) 
{ 
	DrawRect(_X0, _Y0, _X1, _Y1, _Color, _Color, _Color, _Color); 
}

void CAtom3DGraph::DrawTriangles(int _NumTriangles, int *_Vertices, color32 *_Colors, Cull _CullMode)
{
    if( _NumTriangles <= 0 )
        return;

	ATOM_Material *m = _twMaterial.get();

    if( _CullMode==CULL_CW )
		m->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_Front);
    else if( _CullMode==CULL_CCW )
		m->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_Back);
    else
		m->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);

    if(_triVertices.size() < 3 * _NumTriangles )
	{
        _triVertices.resize(3 * _NumTriangles);
	}

    for( int i=0; i<3*_NumTriangles; ++i )
    {
        _triVertices[i].m_Pos[0] = (float)(_Vertices[2*i+0] + _offsetX);
        _triVertices[i].m_Pos[1] = (float)(_Vertices[2*i+1] + _offsetY);
        _triVertices[i].m_Pos[2] = 0;
        _triVertices[i].m_Pos[3] = 1;
        _triVertices[i].m_Color  = _Colors[i];
    }

	resizeIndicesCache (_triVertices.size());

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	unsigned num = m->begin (device);
	for (unsigned i = 0; i < num; ++i)
	{
		if (m->beginPass(device, i))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, _triVertices.size(), _triVertices.size(), ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(CTriVtx), &_triVertices[0], &_indicesCache[0]);
			m->endPass (device, i);
		}
	}
	m->end (device);
}

void * CAtom3DGraph::NewTextObj()
{
#if USE_ATOM3D_TEXT
	return ATOM_NEW(CAtom3DTextObject);
#else
	return ATOM_NEW(CTextObj);
#endif
}

void CAtom3DGraph::DeleteTextObj(void *_TextObj)
{
#if USE_ATOM3D_TEXT
	ATOM_DELETE ((CAtom3DTextObject*)_TextObj);
#else
	ATOM_DELETE((CTextObj*)_TextObj);
#endif
}

static ATOM_AUTOREF(ATOM_Texture) BindFont(ATOM_RenderDevice *_Dev, const CTexFont *_Font)
{
	ATOM_AUTOREF(ATOM_Texture) texture = _Dev->allocTexture (0, 0, _Font->m_TexWidth, _Font->m_TexHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS);
	if (texture)
	{
		ATOM_Texture::LockedRect lockedRect;
		if (texture->lock (0, &lockedRect))
		{
			unsigned *p = static_cast<unsigned*>(lockedRect.bits);
			for( int i=0; i<_Font->m_TexWidth*_Font->m_TexHeight; ++i, ++p )
				*p = 0x00ffffff | (((unsigned)(_Font->m_TexBytes[i]))<<24);
			texture->unlock(0);
		}
	}
    return texture;
}

void CAtom3DGraph::BuildText(void *_TextObj, const char **_TextLines, color32 *_LineColors, color32 *_LineBgColors, int _NbLines, const CTexFont *_Font, int _Sep, int _BgWidth)
{
#if USE_ATOM3D_TEXT
	CAtom3DTextObject *texObj = (CAtom3DTextObject*)_TextObj;
	texObj->removeAllLines ();

	ATOM_GUIFont::handle fontHandle = ATOM_GUIFont::getDefaultFont (_Font->m_CharHeight - 2, 0);
	if (fontHandle)
	{
		for (unsigned i = 0; i < _NbLines; ++i)
		{
			ATOM_AUTOREF(ATOM_Text) text = ATOM_HARDREF(ATOM_Text)();
			text->setFont (ATOM_GUIFont::getFontHandle(fontHandle));
			text->setCharMargin (0);
			text->setZValue (1.f);
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
			text->setString (_TextLines ? _TextLines[i] : "");
			texObj->appendLine (text.get(), (_LineColors ? _LineColors[i] : COLOR32_RED), (_LineBgColors ? _LineBgColors[i] : COLOR32_BLACK), _Font->m_CharHeight, _Sep, _BgWidth);
		}
	}
#else
    if( _Font != _fontTex )
    {
        _fontTexture = BindFont(ATOM_GetRenderDevice(), _Font);
        _fontTex = _Font;
    }

    CTextObj *TextObj = static_cast<CTextObj *>(_TextObj);
    TextObj->_textVerts.resize(0);
    TextObj->_bgVerts.resize(0);
    TextObj->_lineColors = (_LineColors!=NULL);
    TextObj->_lineBgColors = (_LineBgColors!=NULL);

    int x, x1, y, y1, i, Len;
    unsigned char ch;
    const unsigned char *Text;
    unsigned LineColor = COLOR32_RED;
    CTextVtx Vtx;
    Vtx._pos[2] = 0;
    Vtx._pos[3] = 1;
    CBgVtx BgVtx;
    BgVtx._pos[2] = 0;
    BgVtx._pos[3] = 1;
    for( int Line=0; Line<_NbLines; ++Line )
    {
        x = 0;
        y = Line * (_Font->m_CharHeight+_Sep);
        y1 = y+_Font->m_CharHeight;
        Len = (int)strlen(_TextLines[Line]);
        Text = (const unsigned char *)(_TextLines[Line]);
        if( _LineColors!=NULL )
            LineColor = _LineColors[Line];

        for( i=0; i<Len; ++i )
        {
            ch = Text[i];
            x1 = x + _Font->m_CharWidth[ch];

            Vtx._color  = LineColor;

            Vtx._pos[0] = (float)x;
            Vtx._pos[1] = (float)y;
            Vtx._uv [0] = _Font->m_CharU0[ch];
            Vtx._uv [1] = _Font->m_CharV0[ch];
            TextObj->_textVerts.push_back(Vtx);

            Vtx._pos[0] = (float)x1;
            Vtx._pos[1] = (float)y;
            Vtx._uv [0] = _Font->m_CharU1[ch];
            Vtx._uv [1] = _Font->m_CharV0[ch];
            TextObj->_textVerts.push_back(Vtx);

            Vtx._pos[0] = (float)x;
            Vtx._pos[1] = (float)y1;
            Vtx._uv [0] = _Font->m_CharU0[ch];
            Vtx._uv [1] = _Font->m_CharV1[ch];
            TextObj->_textVerts.push_back(Vtx);

            Vtx._pos[0] = (float)x1;
            Vtx._pos[1] = (float)y;
            Vtx._uv [0] = _Font->m_CharU1[ch];
            Vtx._uv [1] = _Font->m_CharV0[ch];
            TextObj->_textVerts.push_back(Vtx);

            Vtx._pos[0] = (float)x1;
            Vtx._pos[1] = (float)y1;
            Vtx._uv [0] = _Font->m_CharU1[ch];
            Vtx._uv [1] = _Font->m_CharV1[ch];
            TextObj->_textVerts.push_back(Vtx);

            Vtx._pos[0] = (float)x;
            Vtx._pos[1] = (float)y1;
            Vtx._uv [0] = _Font->m_CharU0[ch];
            Vtx._uv [1] = _Font->m_CharV1[ch];
            TextObj->_textVerts.push_back(Vtx);

            x = x1;
        }
        if( _BgWidth>0 )
        {
            if( _LineBgColors!=NULL )
                BgVtx._color = _LineBgColors[Line];
            else
                BgVtx._color  = COLOR32_BLACK;

            BgVtx._pos[0] = -1;
            BgVtx._pos[1] = (float)y;
            TextObj->_bgVerts.push_back(BgVtx);

            BgVtx._pos[0] = (float)(_BgWidth+1);
            BgVtx._pos[1] = (float)y;
            TextObj->_bgVerts.push_back(BgVtx);

            BgVtx._pos[0] = -1;
            BgVtx._pos[1] = (float)y1;
            TextObj->_bgVerts.push_back(BgVtx);

            BgVtx._pos[0] = (float)(_BgWidth+1);
            BgVtx._pos[1] = (float)y;
            TextObj->_bgVerts.push_back(BgVtx);

            BgVtx._pos[0] = (float)(_BgWidth+1);
            BgVtx._pos[1] = (float)y1;
            TextObj->_bgVerts.push_back(BgVtx);

            BgVtx._pos[0] = -1;
            BgVtx._pos[1] = (float)y1;
            TextObj->_bgVerts.push_back(BgVtx);
        }
    }
#endif
}

void CAtom3DGraph::DrawText(void *_TextObj, int _X, int _Y, color32 _Color, color32 _BgColor)
{
#if USE_ATOM3D_TEXT
	CAtom3DTextObject *texObj = (CAtom3DTextObject*)_TextObj;
	float x = (float)_X;
	float y = (float)_Y;

	ATOM_VECTOR<CAtom3DTextObject::CBgVtx> &bgVerts = *texObj->getBgVertices ();
	int i;
	int nb = bgVerts.size();

	if( nb>=4 )
	{
		for( i=0; i<nb; ++i )
		{
			bgVerts[i]._pos[0] += x + _offsetX;
			bgVerts[i]._pos[1] += y + _offsetY;
			if( _BgColor!=0)
				bgVerts[i]._color = _BgColor;
		}

		resizeIndicesCache (nb);

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		_twMaterial->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);
		unsigned num = _twMaterial->begin (device);
		for (unsigned i = 0; i < num; ++i)
		{
			if (_twMaterial->beginPass(device, i))
			{
				ATOM_GetRenderDevice()->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, nb, nb, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(CAtom3DTextObject::CBgVtx), &(bgVerts[0]), &_indicesCache[0]);
				_twMaterial->endPass (device, i);
			}
		}
		_twMaterial->end (device);

		for(unsigned i=0; i<nb; ++i )
		{
			bgVerts[i]._pos[0] -= x + _offsetX;
			bgVerts[i]._pos[1] -= y + _offsetY;
		}
	}

	for (unsigned i = 0; i < texObj->getNumLines (); ++i)
	{
		const CAtom3DTextObject::Line *line = texObj->getLine (i);
		int ascender = ATOM_GetFontAscender (line->line->getFont());
		line->line->render (_X, _Y + i * line->lineMargin + ascender, line->foregroundColor);
	}
#else
    CTextObj *TextObj = static_cast<CTextObj *>(_TextObj);
    float x = (float)_X;
    float y = (float)_Y;

    int i;
    int nv = (int)TextObj->_textVerts.size();
    int nb = (int)TextObj->_bgVerts.size();

    if( nb>=4 )
    {
        for( i=0; i<nb; ++i )
        {
            TextObj->_bgVerts[i]._pos[0] += x + _offsetX;
            TextObj->_bgVerts[i]._pos[1] += y + _offsetY;
            if( _BgColor!=0 || !TextObj->_lineBgColors )
                TextObj->_bgVerts[i]._color = _BgColor;
        }

		resizeIndicesCache (nb);

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		_twMaterial->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);
		unsigned num = _twMaterial->begin (device);
		for (unsigned i = 0; i < num; ++i)
		{
			if (_twMaterial->beginPass(device, i))
			{
				ATOM_GetRenderDevice()->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, nb, nb, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(CBgVtx), &(TextObj->_bgVerts[0]), &_indicesCache[0]);
				_twMaterial->endPass (device, i);
			}
		}
		_twMaterial->end (device);

        for(unsigned i=0; i<nb; ++i )
        {
            TextObj->_bgVerts[i]._pos[0] -= x + _offsetX;
            TextObj->_bgVerts[i]._pos[1] -= y + _offsetY;
        }
    }

    if( nv>=4 )
    {
        for( i=0; i<nv; ++i )
        {
            TextObj->_textVerts[i]._pos[0] += x + _offsetX;
            TextObj->_textVerts[i]._pos[1] += y + _offsetY;
        }
        if( _Color!=0 || !TextObj->_lineColors )
            for( i=0; i<nv; ++i )
                TextObj->_textVerts[i]._color = _Color;

		resizeIndicesCache (nv);

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		_twMaterialText->getParameterTable()->setTexture ("texture", _fontTexture.get());
		unsigned num = _twMaterialText->begin (device);
		for (unsigned i = 0; i < num; ++i)
		{
			if (_twMaterialText->beginPass(device, i))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, nv, nv, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(CTextVtx), &(TextObj->_textVerts[0]), &_indicesCache[0]);
				_twMaterialText->endPass (device, i);
			}
		}
		_twMaterialText->end (device);

        for(unsigned i=0; i<nv; ++i )
        {
            TextObj->_textVerts[i]._pos[0] -= x + _offsetX;
            TextObj->_textVerts[i]._pos[1] -= y + _offsetY;
        }
    }
#endif
}

void CAtom3DGraph::ChangeViewport(int _X0, int _Y0, int _Width, int _Height, int _OffsetX, int _OffsetY)
{
    if( _Width>0 && _Height>0 )
    {
		ATOM_GetRenderDevice()->setViewport (0, _X0, _Y0, _Width, _Height);
 
        _offsetX = _X0 + _OffsetX;
        _offsetY = _Y0 + _OffsetY - 1;
    }
}

void CAtom3DGraph::RestoreViewport()
{
	ATOM_GetRenderDevice()->setViewport (0, 0, 0, _wWidth, _wHeight);
	_offsetX = 0;
	_offsetY = 0;
}

void CAtom3DGraph::resizeIndicesCache (unsigned count)
{
	unsigned size = _indicesCache.size();
	if (size < count)
	{
		_indicesCache.resize (count);
		for (unsigned n = size; n < count; ++n)
		{
			_indicesCache[n] = n;
		}
	}
}

void CAtom3DGraph::DeleteThis ()
{
	ATOM_DELETE(this);
}


