#include "StdAfx.h"
#include "bkimage.h"

//--- wangjian modified ---//
// 当前暂时将BKIMAGE放到shading的渲染遍中进行，这样可以显示特效，但是需要关闭HDR(原来是在PostComposition
static const char textureMaterialSrc[] = 
	"<?xml version=\"1.0\"?>\n\
	<material auto=\"1\">\n\
		<param name=\"texture\" type=\"texture\" />\n\
		<param name=\"s\" type=\"sampler2D\">	\n\
			<filter value=\"ppp\" />	\n\
			<addressu value=\"clamp\" />	\n\
			<addressv value=\"clamp\" />	\n\
			<texture value=\"$texture\" />	\n\
			<srgb value=\"true\" />\n\
		</param>	\n\
		<code><![CDATA[						\n\
			struct a2v			\n\
			{	\n\
				float4 position : POSITION;\n\
				float2 texcoord : TEXCOORD0;\n\
			};\n\
			struct v2p											\n\
			{							\n\
				float4 position : POSITION;	\n\
				float2 texcoord0 : TEXCOORD0;	\n\
			};	\n\
			v2p vs ( in a2v _Input)\n\
			{\n\
				v2p _Output;\n\
				_Output.position = _Input.position;\n\
				_Output.texcoord0 = _Input.texcoord;\n\
				return _Output;\n\
			}\n\
			float4 ps (in v2p IN) : COLOR0	\n\
			{	\n\
				return tex2D(s, IN.texcoord0);	\n\
			}	\n\
		]]></code>	\n\
		<effect name=\"default\" scheme=\"deferred\" schemelayer=\"shading\">\n\
			<pass>\n\
				<streamdefine>\n\
					<stream semantec=\"position\"/>\n\
					<stream semantec=\"texcoord0\" type=\"float2\" />\n\
				</streamdefine>\n\
				<alphablend_state>\n\
					<enable value=\"true\" />\n\
					<srcblend value=\"srcalpha\" />\n\
					<destblend value=\"invsrcalpha\" />\n\
				</alphablend_state>\n\
				<depth_state>\n\
					<zfunc value=\"lessequal\" />\n\
					<zwrite value=\"false\" />\n\
				</depth_state>\n\
				<rasterizer_state>\n\
					<cullmode value=\"none\" />\n\
				</rasterizer_state>\n\
				<vertexshader entry=\"vs\" target=\"vs_2_0\" />	\n\
				<pixelshader entry=\"ps\" target=\"ps_2_0\" />	\n\
			</pass>\n\
		</effect>\n\
		<effect name=\"defaultForward\" scheme=\"custom\" schemelayer=\"Solid\">\n\
		<pass>\n\
		<streamdefine>\n\
		<stream semantec=\"position\"/>\n\
		<stream semantec=\"texcoord0\" type=\"float2\" />\n\
		</streamdefine>\n\
		<alphablend_state>\n\
		<enable value=\"true\" />\n\
		<srcblend value=\"srcalpha\" />\n\
		<destblend value=\"invsrcalpha\" />\n\
		</alphablend_state>\n\
		<depth_state>\n\
		<zfunc value=\"lessequal\" />\n\
		<zwrite value=\"false\" />\n\
		</depth_state>\n\
		<rasterizer_state>\n\
		<cullmode value=\"none\" />\n\
		</rasterizer_state>\n\
		<vertexshader entry=\"vs\" target=\"vs_2_0\" />	\n\
		<pixelshader entry=\"ps\" target=\"ps_2_0\" />	\n\
		</pass>\n\
		</effect>\n\
	</material>\n";

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_BkImage)
	ATOM_ATTRIBUTES_BEGIN(ATOM_BkImage)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_BkImage, "ImageFile",	getImageFile, setImageFile, "", "group=ATOM_BkImage;type=vfilename;desc='VFS image file name'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_BkImage, ATOM_Node)

ATOM_BkImage::ATOM_BkImage (void)
{
	_imageChanged = false;
}

ATOM_BkImage::~ATOM_BkImage (void)
{
}

struct TextureColorVertex_bkImage
{
	enum { ATTRIB = ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2 };
	float x, y, z, w;
	float u, v;
};

bool ATOM_BkImage::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_imageChanged)
	{
		_imageChanged = false;
		_imageTexture = ATOM_FindTextureResource (_imageFileName.c_str());
		if (!_imageTexture)
		{
			_imageTexture = ATOM_CreateEmptyTextureResource (_imageFileName.c_str());
			_imageTexture->setFlags (ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
			_imageTexture->loadTexImageFromFile(_imageFileName.c_str(), 0, 0, ATOM_PIXEL_FORMAT_BGRA8888);
		}
	}

	ATOM_Rect2Di vp = device->getViewport (0);

	TextureColorVertex_bkImage tc_vertices[4];
	unsigned short indices[6];

	TextureColorVertex_bkImage *v = tc_vertices;
	unsigned short *idx = indices;

	// wangjian modified
#if 0
	v->x = -0.5f;
	v->y = -0.5f;
#else
	v->x = -1;//vp.point.x-0.5f;
	v->y = 1;//vp.point.y-0.5f;
#endif
	v->z = 1.f;
	v->w = 1.f;
	v->u = 0.f;
	v->v = 0.f;
	v++;

#if 0
	v->x = vp.size.w-0.5f;
	v->y = -0.5f;
#else
	v->x = 1;//vp.point.x + vp.size.w-0.5f;
	v->y = 1;//vp.point.y-0.5f;
#endif
	v->z = 1.f;
	v->w = 1.f;
	v->u = 1.f;
	v->v = 0.f;
	v++;

#if 0
	v->x = vp.size.w-0.5f;
	v->y = vp.size.h-0.5f;
#else
	v->x = 1;//vp.point.x+vp.size.w-0.5f;
	v->y = -1;//vp.point.y+vp.size.h-0.5f;
#endif
	v->z = 1.f;
	v->w = 1.f;
	v->u = 1.f;
	v->v = 1.f;
	v++;

#if 0
	v->x = -0.5f;
	v->y = vp.size.h-0.5f;
#else
	v->x = -1;//vp.point.x-0.5f;
	v->y = -1;//vp.point.y+vp.size.h-0.5f;
#endif
	v->z = 1.f;
	v->w = 1.f;
	v->u = 0.f;
	v->v = 1.f;
	v++;

	*idx++ = 0;
	*idx++ = 1;
	*idx++ = 2;
	*idx++ = 0;
	*idx++ = 2;
	*idx++ = 3;

	_material->getParameterTable()->setTexture ("texture", _imageTexture ? _imageTexture.get() : ATOM_GetColorTexture(0xFFFFFFFF));
	unsigned numPasses = _material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_material->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, 6, 4, TextureColorVertex_bkImage::ATTRIB, sizeof(TextureColorVertex_bkImage), tc_vertices, indices);
			_material->endPass (device, pass);
		}
	}
	_material->end (device);

	return true;
}

void ATOM_BkImage::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

const ATOM_Matrix4x4f &ATOM_BkImage::getWorldMatrix (void) const
{
	return ATOM_Matrix4x4f::getIdentityMatrix ();
}

void ATOM_BkImage::buildBoundingbox(void) const
{
	_boundingBox.setMin (ATOM_Vector3f(-10000.f, -10000.f, -10000.f));
	_boundingBox.setMax (ATOM_Vector3f(10000.f, 10000.f, 10000.f));
}

void ATOM_BkImage::buildWorldMatrix (void) const
{
	_worldMatrix.makeIdentity();
}

bool ATOM_BkImage::onLoad(ATOM_RenderDevice *pDevice)
{
	_material = ATOM_MaterialManager::createMaterialFromCoreString (pDevice, textureMaterialSrc);
	if (!_material)
	{
		return false;
	}
	
	// wangjian modified
#if 0
	_material->setActiveEffect ("default");
#else
	_material->setActiveEffect ( ATOM_RenderSettings::isNonDeferredShading() ? "defaultForward" : "default" );
#endif

	_load_flag = LOAD_ALLFINISHED;

	return true;
}

void ATOM_BkImage::setImageFile (const ATOM_STRING &filename)
{
	_imageFileName = filename;
	_imageChanged = true;
}

const ATOM_STRING &ATOM_BkImage::getImageFile (void) const
{
	return _imageFileName;
}

ATOM_Material *ATOM_BkImage::getMaterial(void) const
{
	return _material.get();
}

void ATOM_BkImage::setTexture (ATOM_Texture *texture)
{
	_imageFileName = "";
	_imageChanged = false;
	_imageTexture = texture;
}

ATOM_Texture *ATOM_BkImage::getTexture (void) const
{
	return _imageTexture.get();
}

