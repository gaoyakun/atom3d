#include "stdafx.h"
#include "gui_canvas.h"

static const char textureMaterialSrcColored[] = "\
	<?xml version=\"1.0\"?>	\
	<material auto=\"0\">	\
		<param name=\"zfunc\" type=\"int\" />	\
		<code><![CDATA[	\
			struct v2p	\
			{	\
				float4 position : POSITION0;	\
				float4 diffuse : COLOR0;	\
			};	\
			float4 ps (in v2p IN) : COLOR0	\
			{	\
				return IN.diffuse;	\
			}	\
		]]></code>	\
		<effect name=\"default\">	\
			<pass>	\
				<streamdefine>	\
					<stream semantec=\"position\"/>	\
					<stream semantec=\"color0\" />	\
				</streamdefine>	\
				<alphablend_state>	\
					<enable value=\"true\" />	\
					<srcblend value=\"srcalpha\" />	\
					<destblend value=\"invsrcalpha\" />	\
				</alphablend_state>	\
				<depth_state>	\
					<zfunc value=\"$zfunc\" />	\
					<zwrite value=\"false\" />	\
				</depth_state>	\
				<rasterizer_state>	\
					<cullmode value=\"none\" />	\
				</rasterizer_state>	\
				<pixelshader entry=\"ps\" target=\"ps_2_0\" />	\
			</pass>	\
		</effect>	\
	</material>";

static const char textureMaterialSrcTextured[] = "\
	<?xml version=\"1.0\"?>	\
	<material auto=\"0\">	\
		<param name=\"texture\" type=\"texture\" />	\
		<param name=\"filter\" type=\"int\" />	\
		<param name=\"diffuseSampler\" type=\"sampler2D\">	\
			<filter value=\"$filter\" />	\
			<addressu value=\"clamp\" />	\
			<addressv value=\"clamp\" />	\
			<texture value=\"$texture\" />	\
		</param>	\
		<param name=\"zfunc\" type=\"int\" />	\
		<code><![CDATA[	\
			struct v2p	\
			{	\
				float4 position : POSITION0;	\
				float4 diffuse : COLOR0;	\
				float2 tex : TEXCOORD0;	\
			};	\
			float4 ps (in v2p IN) : COLOR0	\
			{	\
				return tex2D(diffuseSampler, IN.tex) * IN.diffuse;	\
			}	\
		]]></code>	\
		<effect name=\"default\">	\
			<pass>	\
				<streamdefine>	\
					<stream semantec=\"position\"/>	\
					<stream semantec=\"color0\" />	\
					<stream semantec=\"texcoord0\" type=\"float2\" />	\
				</streamdefine>	\
				<alphablend_state>	\
					<enable value=\"true\" />	\
					<srcblend value=\"srcalpha\" />	\
					<destblend value=\"invsrcalpha\" />	\
				</alphablend_state>	\
				<depth_state>	\
					<zfunc value=\"$zfunc\" />	\
					<zwrite value=\"false\" />	\
				</depth_state>	\
				<rasterizer_state>	\
					<cullmode value=\"none\" />	\
				</rasterizer_state>	\
				<pixelshader entry=\"ps\" target=\"ps_2_0\" />	\
			</pass>	\
		</effect>	\
	</material>";

struct ColorVertex
{
	enum { ATTRIB_FF = ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR };
	float x, y, z, w;
	unsigned diffuse;
};

struct TextureColorVertex
{
	enum { ATTRIB_FF = ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2|ATOM_VERTEX_ATTRIB_TEX2_2 };
	float x, y, z, w;
	unsigned diffuse;
	float u, v;
	float u2, v2;
};

static const float pixeloffset = -0.5f;

// the draw funcitons will always be executed at the primary thread, so static variable can be used
static ATOM_VECTOR<ColorVertex> c_vertices;
static ATOM_VECTOR<TextureColorVertex> tc_vertices;
static ATOM_VECTOR<unsigned short> indices;

static ATOM_AUTOPTR(ATOM_Material) _solidColorMaterial;
static ATOM_AUTOPTR(ATOM_Material) _textureMaterial;

static bool realizeMaterials (void)
{
  ATOM_STACK_TRACE(realizeMaterials);

  if (!_solidColorMaterial)
  {
	  _solidColorMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/guicanvas_color.mat");
	  if (!_solidColorMaterial)
	  {
		  _solidColorMaterial = ATOM_MaterialManager::createMaterialFromCoreString (ATOM_GetRenderDevice(), textureMaterialSrcColored);
		  if (!_solidColorMaterial)
		  {
			return false;
		  }
	  }
	  _solidColorMaterial->setActiveEffect ("default");
  }

  if (!_textureMaterial)
  {
	  _textureMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/guicanvas_texture.mat");
	  if (!_textureMaterial)
	  {
		  _textureMaterial = ATOM_MaterialManager::createMaterialFromCoreString (ATOM_GetRenderDevice(), textureMaterialSrcTextured);
		  if (!_textureMaterial)
		  {
			return false;
		  }
	  }
	  _textureMaterial->setActiveEffect ("default");
  }

  return true;
}

ATOM_GUICanvas::ATOM_GUICanvas (ATOM_Widget *widget)
{
	ATOM_Rect2Di rcWidget = widget->getWidgetRect();
	rcWidget.point = - widget->getClientRect().point;
	widget->clientToViewport (&rcWidget.point);

	resize (rcWidget.size);
	setOrigin (rcWidget.point);

	_widget = widget;
}

ATOM_GUICanvas::~ATOM_GUICanvas (void)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::~ATOM_GUICanvas);

	_widget = 0;
}

void ATOM_GUICanvas::resize (const ATOM_Size2Di &size)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::resize);

	_size = size;
}

const ATOM_Size2Di & ATOM_GUICanvas::getSize (void) const
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::getSize);

	return _size;
}

void ATOM_GUICanvas::setOrigin (const ATOM_Point2Di &point)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::setOrigin);

	_origin = point;
}

const ATOM_Point2Di & ATOM_GUICanvas::getOrigin (void) const
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::getOrigin);

	return _origin;
}

void ATOM_GUICanvas::setTargetRect (const ATOM_Rect2Di &rect)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::setTargetRect);

	_targetRect = rect;
}

const ATOM_Rect2Di & ATOM_GUICanvas::getTargetRect (void) const
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::getTargetRect);

	return _targetRect;
}

void ATOM_GUICanvas::fillRect (const ATOM_Rect2Di &rect, ATOM_ColorARGB color, float rotation)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::fillRect);

	fillRectList (1, &rect, &color, &rotation);
}

void ATOM_GUICanvas::fillRectList (unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, const float *rotations)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::fillRectList);

	if (num == 0)
	{

	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	c_vertices.resize (num * 4);
	indices.resize (num * 6);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog() ? 1.f : 0.f;

	ColorVertex *v = &c_vertices[0];
	unsigned short *idx = &indices[0];
	float sv, cv, tx, ty;

	for (unsigned i = 0; i < num; ++i)
	{
		const ATOM_Rect2Di &rc = rects[i];
		const unsigned c = colors[i];
		float l = rc.point.x + _origin.x;// + pixeloffset;
		float t = rc.point.y + _origin.y;// + pixeloffset;
		float r = l + rc.size.w;
		float b = t + rc.size.h;

		bool rot = rotations && rotations[i] != 0.f;
		if (rot)
		{
			ATOM_sincos (rotations[i], &sv, &cv);
			float cx = l + rc.size.w * 0.5f;
			float cy = t + rc.size.h * 0.5f;
			tx = cx * (1.f - cv) + cy * sv;
			ty = cy * (1.f - cv) - cx * sv;
		}

		v->x = rot ? (l * cv - t * sv + tx) : l;
		v->y = rot ? (l * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		v->x = rot ? (r * cv - t * sv + tx) : r;
		v->y = rot ? (r * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		v->x = rot ? (r * cv - b * sv + tx) : r;
		v->y = rot ? (r * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		v->x = rot ? (l * cv - b * sv + tx) : l;
		v->y = rot ? (l * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		*idx++ = i * 4;
		*idx++ = i * 4 + 1;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4 + 3;
	}

	unsigned attrib = ColorVertex::ATTRIB_FF;
	_solidColorMaterial->getParameterTable()->setInt ("zfunc", _widget->getRenderer()->is3DMode() ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
	unsigned nPasses = _solidColorMaterial->begin (device);
	for (unsigned pass = 0; pass < nPasses; ++pass)
	{
		if (_solidColorMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, num * 6, num * 4, attrib, sizeof(ColorVertex), &c_vertices[0], &indices[0]);
			_solidColorMaterial->endPass (device, pass);
		}
	}
    _solidColorMaterial->end (device);
}

void ATOM_GUICanvas::drawTexturedRect (ATOM_Material *material, const ATOM_Rect2Di &rect, ATOM_ColorARGB color, ATOM_Texture *texture, bool bilinearFilter, float rotation)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTexturedRect);

	drawTexturedRectList (material, 1, &rect, &color, texture, bilinearFilter, &rotation);
}

void ATOM_GUICanvas::drawTexturedRectList (ATOM_Material *material, unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, ATOM_Texture *texture, bool bilinearFilter, const float *rotations)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTexturedRectList);

	if (num == 0)
	{
		return;
	}

	if (!texture)
	{
		fillRectList (num, rects, colors);
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	tc_vertices.resize (num * 4);
	indices.resize (num * 6);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog() ? 1.f : 0.f;

	TextureColorVertex *v = &tc_vertices[0];
	unsigned short *idx = &indices[0];
	float sv, cv, tx, ty;
	float u2_min = -float(_targetRect.point.x)/float(_targetRect.size.w);
	float u2_max = float(_size.w - _targetRect.point.x)/float(_targetRect.size.w);
	float v2_min = -float(_targetRect.point.y)/float(_targetRect.size.h);
	float v2_max = float(_size.h - _targetRect.point.y)/float(_targetRect.size.h);

	for (unsigned i = 0; i < num; ++i)
	{
		const ATOM_Rect2Di &rc = rects[i];
		const unsigned c = colors[i];
		float l = rc.point.x + _origin.x + pixeloffset;
		float t = rc.point.y + _origin.y + pixeloffset;
		float r = l + rc.size.w;
		float b = t + rc.size.h;

		bool rot = rotations && rotations[i] != 0.f;
		if (rot)
		{
			ATOM_sincos (rotations[i], &sv, &cv);
			float cx = l + rc.size.w * 0.5f;
			float cy = t + rc.size.h * 0.5f;
			tx = cx * (1.f - cv) + cy * sv;
			ty = cy * (1.f - cv) - cx * sv;
		}

		v->x = rot ? (l * cv - t * sv + tx) : l;
		v->y = rot ? (l * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = 0.f;
		v->v = 0.f;
		v->u2 = u2_min;
		v->v2 = v2_min;
		v++;

		v->x = rot ? (r * cv - t * sv + tx) : r;
		v->y = rot ? (r * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = 1.f;
		v->v = 0.f;
		v->u2 = u2_max;
		v->v2 = v2_min;
		v++;

		v->x = rot ? (r * cv - b * sv + tx) : r;
		v->y = rot ? (r * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = 1.f;
		v->v = 1.f;
		v->u2 = u2_max;
		v->v2 = v2_max;
		v++;

		v->x = rot ? (l * cv - b * sv + tx) : l;
		v->y = rot ? (l * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = 0.f;
		v->v = 1.f;
		v->u2 = u2_min;
		v->v2 = v2_max;
		v++;

		*idx++ = i * 4;
		*idx++ = i * 4 + 1;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4 + 3;
	}

	ATOM_Material *m = material ? material : _textureMaterial.get();
	if (m)
	{
		bool is3DMode = _widget->getRenderer()->is3DMode ();
		m->getParameterTable()->setInt ("zfunc", is3DMode ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
		m->getParameterTable()->setInt ("filter", bilinearFilter ? ATOM_RenderAttributes::SamplerFilter_LLN : ATOM_RenderAttributes::SamplerFilter_PPN);
		m->getParameterTable()->setTexture ("texture", texture);

		unsigned attrib = TextureColorVertex::ATTRIB_FF;
		unsigned numPasses = m->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, num * 6, num * 4, attrib, sizeof(TextureColorVertex), &tc_vertices[0], &indices[0]);
				m->endPass (device, pass);
			}
		}
		m->end (device);
	}
}

void ATOM_GUICanvas::drawTexturedRectEx (ATOM_Material *material, const ATOM_Rect2Di &rect, ATOM_ColorARGB color, ATOM_Texture *texture, const ATOM_Rect2Di &region, bool bilinearFilter, float rotation)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTexturedRectEx);

	return drawTexturedRectListEx (material, 1, &rect, &color, texture, &region, bilinearFilter, &rotation);
}

void ATOM_GUICanvas::drawTexturedRectListEx (ATOM_Material *material, unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, ATOM_Texture *texture, const ATOM_Rect2Di *regions, bool bilinearFilter, const float *rotations)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTexturedRectListEx);

	if (num == 0 || _targetRect.size.w == 0 || _targetRect.size.h == 0)
	{
		return;
	}

	if (!texture)
	{
		fillRectList (num, rects, colors);
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	tc_vertices.resize (num * 4);
	indices.resize (num * 6);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog() ? 1.f : 0.f;

	TextureColorVertex *v = &tc_vertices[0];
	unsigned short *idx = &indices[0];
	float sv, cv, tx, ty;
	float u2_min = -float(_targetRect.point.x)/float(_targetRect.size.w);
	float u2_max = float(_size.w - _targetRect.point.x)/float(_targetRect.size.w);
	float v2_min = -float(_targetRect.point.y)/float(_targetRect.size.h);
	float v2_max = float(_size.h - _targetRect.point.y)/float(_targetRect.size.h);

	for (unsigned i = 0; i < num; ++i)
	{
		const ATOM_Rect2Di &rc = rects[i];
		const ATOM_Rect2Di &rcRegion = regions[i];
		bool validRegion = rcRegion.size.w != 0 && rcRegion.size.h != 0;
		float min_tx = validRegion ? float(rcRegion.point.x)/float(texture->getWidth()) : 0.f;
		float max_tx = validRegion ? float(rcRegion.point.x + rcRegion.size.w)/float(texture->getWidth()) : 1.f;
		float min_ty = validRegion ? float(rcRegion.point.y)/float(texture->getHeight()) : 0.f;
		float max_ty = validRegion ? float(rcRegion.point.y + rcRegion.size.h)/float(texture->getHeight()) : 1.f;

		const unsigned c = colors[i];
		float l = rc.point.x + _origin.x + pixeloffset;
		float t = rc.point.y + _origin.y + pixeloffset;
		float r = l + rc.size.w;
		float b = t + rc.size.h;

		bool rot = rotations && rotations[i] != 0.f;
		if (rot)
		{
			ATOM_sincos (rotations[i], &sv, &cv);
			float cx = l + rc.size.w * 0.5f;
			float cy = t + rc.size.h * 0.5f;
			tx = cx * (1.f - cv) + cy * sv;
			ty = cy * (1.f - cv) - cx * sv;
		}

		v->x = rot ? (l * cv - t * sv + tx) : l;
		v->y = rot ? (l * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = min_tx;
		v->v = min_ty;
		v->u2 = u2_min;
		v->v2 = v2_min;
		v++;

		v->x = rot ? (r * cv - t * sv + tx) : r;
		v->y = rot ? (r * sv + t * cv + ty) : t;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = max_tx;
		v->v = min_ty;
		v->u2 = u2_max;
		v->v2 = v2_min;
		v++;

		v->x = rot ? (r * cv - b * sv + tx) : r;
		v->y = rot ? (r * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->u = max_tx;
		v->v = max_ty;
		v->u2 = u2_max;
		v->v2 = v2_max;
		v->diffuse = c;
		v++;

		v->x = rot ? (l * cv - b * sv + tx) : l;
		v->y = rot ? (l * sv + b * cv + ty) : b;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v->u = min_tx;
		v->v = max_ty;
		v->u2 = u2_min;
		v->v2 = v2_max;
		v++;

		*idx++ = i * 4;
		*idx++ = i * 4 + 1;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4;
		*idx++ = i * 4 + 2;
		*idx++ = i * 4 + 3;
	}

	ATOM_Material *m = material ? material : _textureMaterial.get();
	if (m)
	{
		bool is3DMode = _widget->getRenderer()->is3DMode ();
		m->getParameterTable()->setInt ("zfunc", is3DMode ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
		m->getParameterTable()->setInt ("filter", bilinearFilter ? ATOM_RenderAttributes::SamplerFilter_LLN : ATOM_RenderAttributes::SamplerFilter_PPN);
		m->getParameterTable()->setTexture ("texture", texture);

		unsigned numPasses = m->begin(device);
		unsigned attrib = TextureColorVertex::ATTRIB_FF;
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, num * 6, num * 4, attrib, sizeof(TextureColorVertex), &tc_vertices[0], &indices[0]);
				m->endPass (device, pass);
			}
		}
		m->end (device);
	}
}

void ATOM_GUICanvas::drawLine (const ATOM_Point2Di &p1, const ATOM_Point2Di &p2, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawLine);

	drawLineList (1, &p1, &p2, &color);
}

void ATOM_GUICanvas::drawLineList (unsigned num, const ATOM_Point2Di *p1, const ATOM_Point2Di *p2, const ATOM_ColorARGB *colors)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawLineList);

	if (!num)
	{
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	c_vertices.resize (num * 2);
	indices.resize (num * 2);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog() ? 1.f : 0.f;

	ColorVertex *v = &c_vertices[0];
	unsigned short *idx = &indices[0];
	for (unsigned i = 0; i < num; ++i)
	{
		const ATOM_Point2Di &start = p1[i];
		const ATOM_Point2Di &end = p2[i];
		unsigned c = colors[i];

		v->x = start.x + _origin.x;// + pixeloffset;
		v->y = start.y + _origin.y;// + pixeloffset;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		v->x = end.x + _origin.x;// + pixeloffset;
		v->y = end.y + _origin.y;// + pixeloffset;
		v->z = z;
		v->w = 1.f;
		v->diffuse = c;
		v++;

		*idx++ = i * 2;
		*idx++ = i * 2 + 1;
	}

	_solidColorMaterial->getParameterTable()->setInt ("zfunc", _widget->getRenderer()->is3DMode() ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
	unsigned attrib = ColorVertex::ATTRIB_FF;
	unsigned numPasses = _solidColorMaterial->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_solidColorMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed(ATOM_PRIMITIVE_LINES, num * 2, num * 2, attrib, sizeof(ColorVertex), &c_vertices[0], &indices[0]);
			_solidColorMaterial->endPass (device, pass);
		}
	}
	_solidColorMaterial->end (device);
}

void ATOM_GUICanvas::drawRect (const ATOM_Rect2Di &rect, ATOM_ColorARGB color, int lineWidth)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawRect);

	int l = rect.point.x;
	int t = rect.point.y;
	int r = rect.point.x + rect.size.w - 1;
	int b = rect.point.y + rect.size.h - 1;

	ATOM_Point2Di start[4] = { ATOM_Point2Di(l, t), ATOM_Point2Di(l, t), ATOM_Point2Di(r, t), ATOM_Point2Di(l, b) };
	ATOM_Point2Di end[4] = { ATOM_Point2Di(r, t), ATOM_Point2Di(l, b), ATOM_Point2Di(r, b), ATOM_Point2Di(r, b) };
	ATOM_ColorARGB colors[4] = { color, color, color, color };

	drawLineList (4, start, end, colors);
}

void ATOM_GUICanvas::drawText (const char *text, ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawText);

	ATOM_Point2Di pt(x + _origin.x, y + _origin.y);
	//_widget->clientToViewport (&pt);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	device->outputText (font, pt.x, pt.y, color, text);
}

void ATOM_GUICanvas::drawTextOutline (const char *text, ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTextOutline);

	ATOM_Point2Di pt(x + _origin.x, y + _origin.y);
	//_widget->clientToViewport (&pt);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	device->outputTextOutline (font, pt.x, pt.y, color, colorOutline, text);
}

void ATOM_GUICanvas::drawTextShadow (const char *text, ATOM_FontHandle font, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTextShadow);

	ATOM_Point2Di pt(x + _origin.x, y + _origin.y);
	//_widget->clientToViewport (&pt);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	device->outputTextShadow (font, pt.x, pt.y, offsetx, offsety, color, colorShadow, text);
}

void ATOM_GUICanvas::drawText (ATOM_Text *text, int x, int y, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawText);

	if (text)
	{
		ATOM_Point2Di pt (x + _origin.x, y + _origin.y);

		if (_widget->getRenderer()->is3DMode ())
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D_ZTEST);
			text->setZValue (_widget->getRenderer()->getZValue());
		}
		else
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
		}

		text->render (pt.x, pt.y, color);
	}
}

void ATOM_GUICanvas::drawTextOutline (ATOM_Text *text, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTextOutline);

	if (text)
	{
		ATOM_Point2Di pt (x + _origin.x, y + _origin.y);

		if (_widget->getRenderer()->is3DMode ())
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D_ZTEST);
			text->setZValue (_widget->getRenderer()->getZValue());
		}
		else
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
		}

		text->renderOutline (pt.x, pt.y, color, colorOutline);
	}
}

void ATOM_GUICanvas::drawTextShadow (ATOM_Text *text, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTextShadow);

	if (text)
	{
		ATOM_Point2Di pt (x + _origin.x, y + _origin.y);

		if (_widget->getRenderer()->is3DMode ())
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D_ZTEST);
			text->setZValue (_widget->getRenderer()->getZValue());
		}
		else
		{
			text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
		}

		text->renderShadow (pt.x, pt.y, offsetx, offsety, color, colorShadow);
	}
}

void ATOM_GUICanvas::drawTextEx (const DrawTextInfo *info)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTextEx);

	if (info)
	{
		int l, t, w, h;
		ATOM_FontHandle fontHandle;
		bool underline = (info->flags & DRAWTEXT_UNDERLINE) != 0;

		if ((info->flags & DRAWTEXT_TEXTSTRING) != 0 && info->textString && info->textString[0])
		{
			fontHandle = info->font;

			if ((info->flags & DRAWTEXT_OUTLINE) != 0)
			{
				drawTextOutline (info->textString, fontHandle, info->x, info->y, info->textColor, info->outlineColor);
			}
			else if ((info->flags & DRAWTEXT_SHADOW) != 0)
			{
				drawTextShadow (info->textString, fontHandle, info->x, info->y, info->shadowOffsetX, info->shadowOffsetY, info->textColor, info->shadowColor);
			}
			else
			{
				drawText (info->textString, fontHandle, info->x, info->y, info->textColor);
			}

			if (underline)
			{
				ATOM_CalcStringBounds (fontHandle, info->textString, strlen(info->textString), &l, &t, &w, &h);
			}
		}
		else if ((info->flags & DRAWTEXT_TEXTOBJECT) != 0 && info->textObject)
		{
			fontHandle = info->textObject->getFont ();

			if ((info->flags & DRAWTEXT_OUTLINE) != 0)
			{
				drawTextOutline (info->textObject, info->x, info->y, info->textColor, info->outlineColor);
			}
			else if ((info->flags & DRAWTEXT_SHADOW) != 0)
			{
				drawTextShadow (info->textObject, info->x, info->y, info->shadowOffsetX, info->shadowOffsetY, info->textColor, info->shadowColor);
			}
			else
			{
				drawText (info->textObject, info->x, info->y, info->textColor);
			}

			if (underline)
			{
				info->textObject->getBounds (&l, &t, &w, &h);
			}
		}
		else
		{
			return;
		}

		if ((info->flags & DRAWTEXT_UNDERLINE) != 0)
		{
			ATOM_UnderLineInfo uli;
			if (ATOM_GetUnderLineInfo (fontHandle, &uli))
			{
				if (uli.underLineThickness == 1)
				{
					//下划线太接近字体，下移2像素
					drawLine (ATOM_Point2Di(info->x, info->y - uli.underLinePosition+2), ATOM_Point2Di(info->x + w, info->y - uli.underLinePosition+2), info->underlineColor);
				}
				else
				{
					fillRect (ATOM_Rect2Di (info->x, info->y - uli.underLinePosition, w, uli.underLineThickness), info->underlineColor);
				}
			}
		}
	}
}

void ATOM_GUICanvas::drawConvexPolygon (unsigned numPoints, const ATOM_Point2Di *points, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawConvexPolygon);

	if (numPoints == 0)
	{
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog () ? 1.f : 0.f;

	c_vertices.resize (numPoints);
	indices.resize (numPoints + 1);

	for (unsigned i = 0;i < numPoints; ++i)
	{
		c_vertices[i].x = points[i].x + _origin.x;// + pixeloffset;
		c_vertices[i].y = points[i].y + _origin.y;// + pixeloffset;
		c_vertices[i].z = z;
		c_vertices[i].w = 1.f;
		c_vertices[i].diffuse = color;
		indices[i] = i;
	}
	indices[numPoints] = 0;

	_solidColorMaterial->getParameterTable()->setInt ("zfunc", _widget->getRenderer()->is3DMode() ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
	unsigned numPasses = _solidColorMaterial->begin(device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_solidColorMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed(ATOM_PRIMITIVE_LINE_STRIP, numPoints + 1, numPoints, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(ColorVertex), &c_vertices[0], &indices[0]);
			_solidColorMaterial->endPass (device, pass);
		}
	}
	_solidColorMaterial->end (device);
}

void ATOM_GUICanvas::fillConvexPolygon (unsigned numPoints, const ATOM_Point2Di *points, ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::fillConvexPolygon);

	if (numPoints == 0)
	{
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog () ? 1.f : 0.f;

	c_vertices.resize (numPoints);
	indices.resize (numPoints);

	for (unsigned i = 0;i < numPoints; ++i)
	{
		c_vertices[i].x = points[i].x + _origin.x;// + pixeloffset;
		c_vertices[i].y = points[i].y + _origin.y;// + pixeloffset;
		c_vertices[i].z = z;
		c_vertices[i].w = 1.f;
		c_vertices[i].diffuse = color;
		indices[i] = i;
	}

	_solidColorMaterial->getParameterTable()->setInt ("zfunc", _widget->getRenderer()->is3DMode() ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
	unsigned numPasses = _solidColorMaterial->begin(device);

	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_solidColorMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed(ATOM_PRIMITIVE_TRIANGLE_FAN, numPoints, numPoints, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, sizeof(ColorVertex), &c_vertices[0], &indices[0]);
			_solidColorMaterial->endPass (device, pass);
		}
	}
	_solidColorMaterial->end (device);
}

void ATOM_GUICanvas::drawTexturedConvexPolygon (ATOM_Material *material, unsigned numPoints, const ATOM_Point2Di *points, const ATOM_Vector2f *uv, ATOM_ColorARGB color, ATOM_Texture *texture, bool bilinearFilter)
{
	ATOM_STACK_TRACE(ATOM_GUICanvas::drawTexturedConvexPolygon);

	if (numPoints == 0)
	{
		return;
	}

	if (!texture)
	{
		fillConvexPolygon (numPoints, points, color);
		return;
	}

	realizeMaterials ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	float z = _widget->getRenderer()->getZValue ();
	//float fog = _widget->getRenderer()->isIgnoreFog () ? 1.f : 0.f;

	tc_vertices.resize (numPoints);
	indices.resize (numPoints);

	for (unsigned i = 0;i < numPoints; ++i)
	{
		tc_vertices[i].x = points[i].x + _origin.x + pixeloffset;
		tc_vertices[i].y = points[i].y + _origin.y + pixeloffset;
		tc_vertices[i].z = z;
		tc_vertices[i].w = 1.f;
		tc_vertices[i].diffuse = color;
		tc_vertices[i].u = uv[i].x;
		tc_vertices[i].v = uv[i].y;
		tc_vertices[i].u2 = uv[i].x;
		tc_vertices[i].v2 = uv[i].y;
		indices[i] = i;
	}

	ATOM_Material *m = material ? material : _textureMaterial.get();
	if (m)
	{
		bool is3DMode = _widget->getRenderer()->is3DMode();
		m->getParameterTable()->setInt ("zfunc", is3DMode ? ATOM_RenderAttributes::CompareFunc_LessEqual : ATOM_RenderAttributes::CompareFunc_Always);
		m->getParameterTable()->setInt ("filter", bilinearFilter ? ATOM_RenderAttributes::SamplerFilter_LLN : ATOM_RenderAttributes::SamplerFilter_PPN);
		m->getParameterTable()->setTexture ("texture", texture);

		unsigned numPasses = m->begin(device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed(ATOM_PRIMITIVE_TRIANGLE_FAN, numPoints, numPoints, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(TextureColorVertex), &tc_vertices[0], &indices[0]);
				m->endPass (device, pass);
			}
		}
		m->end (device);
	}
}

