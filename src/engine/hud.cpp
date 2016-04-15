#include "StdAfx.h"

unsigned hudCount = 0;

ATOM_Hud::ATOM_Hud (void)
{
	hudCount++;

	_enableGeoClipping = false;
	_screenSpace = false;
	_x = 0;
	_y = 0;
	_z = 0;
	_width = 0;
	_height = 0;
	_diffuseColor = 0xFFFFFFFF;
	_alignment = ALIGN_CENTER;
	_screenCenter.set (0.f, 0.f, 0.f);
	_callback = NULL;
	_userdata = NULL;
	memset (&_textProp, 0, sizeof(TextProp));
}

ATOM_Hud::~ATOM_Hud (void)
{
	hudCount--;

	if (_textProp.text)
	{
		ATOM_FREE(_textProp.text);
	}
}

void ATOM_Hud::setPosition (int x, int y)
{
	_x = x;
	_y = y;
}

void ATOM_Hud::getPosition (int &x, int &y) const
{
	x = _x;
	y = _y;
}

void ATOM_Hud::setZ (float z)
{
	_z = z;
}

float ATOM_Hud::getZ (void) const
{
	return _z;
}

void ATOM_Hud::setScreenSpace (bool b)
{
	_screenSpace = b;
}

bool ATOM_Hud::isScreenSpace (void) const
{
	return _screenSpace;
}

void ATOM_Hud::setWidth (unsigned width)
{
	_width = width;
}

void ATOM_Hud::setHeight (unsigned height)
{
	_height = height;
}

unsigned ATOM_Hud::getWidth (void) const
{
	return _width;
}

unsigned ATOM_Hud::getHeight (void) const
{
	return _height;
}

void ATOM_Hud::setColor (ATOM_ColorARGB color)
{
	_diffuseColor = color;
	_material = 0;
}

ATOM_ColorARGB ATOM_Hud::getColor (void) const
{
	return _diffuseColor;
}

void ATOM_Hud::setTexture (ATOM_Texture *texture)
{
	_texture = texture;
}

ATOM_Texture *ATOM_Hud::getTexture (void) const
{
	return _texture.get();
}

void ATOM_Hud::setPositionCallback (PositionCallback callback, void *userdata)
{
	_callback = callback;
	_userdata = userdata;
}

void ATOM_Hud::setTextProperties (const TextProp &textProp)
{
	ATOM_STACK_TRACE(ATOM_Hud::setTextProperties);

	if (_textProp.text)
	{
		ATOM_FREE ((void*)_textProp.text);
	}

	_textProp = textProp;

	if (textProp.text)
	{
		if (!_textProp.fontHandle)
		{
			ATOM_GUIFont::handle defaultFont = ATOM_GUIFont::getDefaultFont (12, 0);
			if (defaultFont == ATOM_GUIFont::invalid_handle)
			{
				_textProp.text = 0;
				return;
			}
			_textProp.fontHandle = ATOM_GUIFont::getFontHandle (defaultFont);
		}

		unsigned n = strlen (textProp.text);
		_textProp.text = (char*)ATOM_MALLOC(n + 1);
		strcpy (_textProp.text, textProp.text);

		if (!_text)
		{
			_text = ATOM_HARDREF(ATOM_Text)();
		}

		_text->setString (_textProp.text);
		_text->setCharMargin (_textProp.charMargin);
		_text->setDisplayMode (isScreenSpace() ? ATOM_Text::DISPLAYMODE_2D : ATOM_Text::DISPLAYMODE_3D);
		_text->setFont (_textProp.fontHandle);

		ATOM_SetCharMargin (_textProp.charMargin);
		ATOM_CalcStringBounds (_textProp.fontHandle, _textProp.text, n, &_textRect[0], &_textRect[1], &_textRect[2], &_textRect[3]);
	}
	else
	{
		_text = 0;
	}
}

void ATOM_Hud::getTextProperties (TextProp &textProp) const
{
	textProp = _textProp;
}

void ATOM_Hud::setAlignment (unsigned alignment)
{
	_alignment = alignment;
}

unsigned ATOM_Hud::getAlignment (void) const
{
	return _alignment;
}

void ATOM_Hud::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Hud::accept);

	visitor.visit(*this);
}

void ATOM_Hud::update (const ATOM_Rect2Di &viewport, const ATOM_Matrix4x4f &mvp)
{
	ATOM_STACK_TRACE(ATOM_Hud::update);

	_screenCenter.x = ATOM_floor((mvp.m30/mvp.m33 + 1.f) * viewport.size.w / 2);
	_screenCenter.y = ATOM_floor((1.f - mvp.m31/mvp.m33) * viewport.size.h / 2);
	_screenCenter.z = mvp.m32/mvp.m33;

	if (_callback)
	{
		_screenCenter = _callback (ATOM_Vector3f(_screenCenter.x, _screenCenter.y, _screenCenter.z), _userdata);
	}
}

bool ATOM_Hud::onLoad (ATOM_RenderDevice *device)
{
	_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/hud.mat");
	if (!_material)
	{
		return false;
	}
	_load_flag = LOAD_ALLFINISHED;
	return true;
}

bool ATOM_Hud::draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (!_material)
	{
		return false;
	}

	ATOM_STACK_TRACE(ATOM_Hud::render);

	float sx = _screenSpace ? _x : _screenCenter.x;
	float sy = _screenSpace ? _y : _screenCenter.y;
	float sz = _screenSpace ? _z : _screenCenter.z;

	float l, t, r, b;
	if (_alignment & ALIGN_LEFT)
	{
		l = sx - 0.5f;
	}
	else if (_alignment & ALIGN_RIGHT)
	{
		l = sx - _width - 0.5f;
	}
	else
	{
		l = sx - _width / 2 - 0.5f;
	}

	if (_alignment & ALIGN_TOP)
	{
		t = sy - 0.5f;
	}
	else if (_alignment & ALIGN_BOTTOM)
	{
		t = sy - _height - 0.5f;
	}
	else
	{
		t = sy - _height / 2 - 0.5f;
	}

	r = l + _width;
	b = t + _height;

	const unsigned short indices[4] = { 0, 1, 2, 3 };

	struct Vertex
	{
		float x, y, z, rhw;
		unsigned color;
		float u, v;
	};

	const Vertex v[4] = {
		{ l, t, sz, 1.f, _diffuseColor, 0.f, 0.f },
		{ r, t, sz, 1.f, _diffuseColor, 1.f, 0.f },
		{ r, b, sz, 1.f, _diffuseColor, 1.f, 1.f },
		{ l, b, sz, 1.f, _diffuseColor, 0.f, 1.f }
	};

	_material->getParameterTable()->setInt ("zfunc", _screenSpace ? ATOM_RenderAttributes::CompareFunc_Always : ATOM_RenderAttributes::CompareFunc_LessEqual);
	_material->getParameterTable()->setInt ("enableBlend", _diffuseColor.getByteA() != 0xFF ? 1 : 0);
	_material->getParameterTable()->setTexture ("texture", _texture ? _texture.get() : ATOM_GetColorTexture (0xFFFFFFFF));

	unsigned numPasses = _material->begin (device);
	if (numPasses)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(Vertex), v, indices);
				material->endPass (device, pass);
			}
		}
	}
	material->end (device);

	if (_text)
	{
		int x, y;

		if (_textProp.alignment & ALIGN_LEFT)
		{
			x = ATOM_ftol (sx - _width * 0.5f + _textProp.borderX);
		}
		else if (_textProp.alignment & ALIGN_RIGHT)
		{
			x = ATOM_ftol (sx + _width * 0.5f - _textProp.borderX - _textRect[2]);
		}
		else
		{
			x = ATOM_ftol (sx - _textRect[2] * 0.5f);
		}

		if (_textProp.alignment & ALIGN_TOP)
		{
			y = ATOM_ftol (sy - _height * 0.5f + _textProp.borderY);
		}
		else if (_textProp.alignment & ALIGN_BOTTOM)
		{
			y = ATOM_ftol (sy + _height * 0.5f - _textProp.borderY - _textRect[3]);
		}
		else
		{
			y = ATOM_ftol (sy - _textRect[3] * 0.5f);
		}

		int vp[4];
		device->getViewport (device->getCurrentView(), &vp[0], &vp[1], &vp[2], &vp[3], 0, 0);
		x -= _textRect[0];
		x -= vp[0];
		y -= _textRect[1];
		y -= vp[1];

		_text->setZValue (sz);
		_text->render (x, y, _textProp.textColor);
	}

	return true;
}

float ATOM_Hud::computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix)
{
	if (_screenSpace)
	{
		return _z;
	}
	else
	{
		return ATOM_Drawable::computeSquaredDistanceToCamera (camera, worldMatrix);
	}
}

ATOM_Material *ATOM_Hud::getMaterial (void)
{
	return _material.get();
}

void ATOM_Hud::buildBoundingbox() const
{
  _boundingBox.setMin(ATOM_Vector3f(0,0,0));
  _boundingBox.setMax(ATOM_Vector3f(0,0,0));
}

const ATOM_Vector3f &ATOM_Hud::getCenterPosition (void) const
{
	return _screenCenter;
}

