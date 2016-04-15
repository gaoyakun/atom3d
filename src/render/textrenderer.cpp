#include "StdAfx.h"
#include "textrenderer.h"
#include "statecache.h"

static ATOM_AUTOREF(ATOM_RenderStateSet) textRenderStates;
static ATOM_AUTOREF(ATOM_GPUProgram) textGPUProgram;
static ATOM_AUTOREF(ATOM_Shader) textShaderVS;
static ATOM_AUTOREF(ATOM_GPUProgram) emptyGPUProgram;

static const char srcVS[] = "\
	float4x4 MVP: register(c0);	\
	struct a2v {	\
		float4 position: POSITION;	\
		float2 texcoord: TEXCOORD0;	\
	};	\
	struct v2p {	\
		float4 position: POSITION;	\
		float2 texcoord: TEXCOORD0;	\
	};	\
	void vs (in a2v IN, out v2p OUT) {	\
		OUT.position = mul(IN.position, MVP);	\
		OUT.texcoord = IN.texcoord;	\
	}";

static const char srcPS[] = "\
	float4 color: register(c0);	\
	sampler2D textMap: register(s0); \
	struct v2p {	\
		float4 position: POSITION;	\
		float2 texcoord: TEXCOORD0;	\
	};	\
	float4 ps (in v2p IN): COLOR0 {	\
		float4 c = tex2D(textMap, IN.texcoord);\
		return float4 (color.rgb, color.a * c.a); \
	}";

ATOM_TextRenderer::ATOM_TextRenderer (void)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::ATOM_TextRenderer);
  _M_need_sort = false;
}

ATOM_TextRenderer::~ATOM_TextRenderer (void)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::~ATOM_TextRenderer);
  Clear ();
}

void ATOM_TextRenderer::AddText (ATOM_Texture *texture, const ATOM_Text::GlyphVertex *vertices, const ATOM_Text::GlyphVertex *vertices_outline, unsigned numVertices)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::AddText);
  _M_text_vector.resize (_M_text_vector.size() + 1);
  TextUnit &unit = _M_text_vector.back();
  unit.texture = texture;
  unit.vertices.resize(numVertices);
  memcpy (&unit.vertices[0], vertices, numVertices * sizeof(*vertices));
  if (vertices_outline)
  {
	  unit.vertices_outline.resize (numVertices * 4);
	  memcpy (&unit.vertices_outline[0], vertices_outline, numVertices * 4 * sizeof(*vertices));
  }

  _M_need_sort = true;
}

void ATOM_TextRenderer::Clear (void)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::Clear);
  _M_text_vector.resize(0);
}

namespace
{
  struct IndexGen
  {
    static ATOM_VECTOR<unsigned short> indices; 
    static inline void generate (unsigned short n) {
      unsigned start = indices.size() / 3;
      if (n > start)
      {
        indices.resize (n * 3);
        for (unsigned short i = start; i < n; ++i)
        {
          unsigned short n0 = (i >> 1) * 4;
          unsigned short n1 = n0 + (i & 1) + 1;
          unsigned short n2 = n1 + 1;
          indices[i*3+0] = n0;
          indices[i*3+1] = n2;
          indices[i*3+2] = n1;
        }
      }
    }
    static inline const unsigned short *get (void) {
      return &indices[0];
    }
  };

  ATOM_VECTOR<unsigned short> IndexGen::indices;
}

void ATOM_TextRenderer::InternalRender (ATOM_RenderDevice *device
										, const ATOM_Vector4f &textColor
										, bool outline)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::InternalRender);
  // This function should always not been used multithreaded, so use static variable here.
  static ATOM_VECTOR<ATOM_Text::GlyphVertex> vertices;

  if (_M_text_vector.empty())
  {
    return;
  }

  if (_M_need_sort)
  {
    _M_need_sort = false;
    _radixsort.sort ((const unsigned*)&(_M_text_vector.begin()->texture), _M_text_vector.size(), false, sizeof(TextUnit));
  }

  unsigned *p = _radixsort.getIndices();
  unsigned *pEnd = p + _M_text_vector.size();
  vertices.resize (0);

  textRenderStates->commit ();	

  do
  {
    TextUnit *unit = &_M_text_vector[*p++];
    TextUnit *nextUnit = p == pEnd ? 0 : &_M_text_vector[*p];

	ATOM_Text::GlyphVertex *v;
	unsigned num;
	if (outline)
	{
		if (unit->vertices_outline.empty())
		{
			return;
		}
		v = &unit->vertices_outline[0];
		num = unit->vertices_outline.size();
	}
	else
	{
		v = &unit->vertices[0];
		num = unit->vertices.size();
	}

	for (unsigned i = 0; i < num; ++i)
	{
	  vertices.push_back (v[i]);
	}

    if (p == pEnd || nextUnit->texture != unit->texture)
    {
        IndexGen::generate (vertices.size() / 2);
        const void *streams = &vertices[0];

		//device->setTexture (0, unit->texture.get());
		textRenderStates->useSampler (0, 0)->setTexture (unit->texture.get());
		//textRenderStates->commit ();	
        device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, 3 * (vertices.size() / 2), vertices.size(), ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2,
                    sizeof(ATOM_Text::GlyphVertex), streams, IndexGen::get());
        vertices.resize (0);
    }
  }
  while (p != pEnd);
  device->getStateCache()->getDesiredStates()->replace (device->getStateCache()->getDefaultStates());
}

void ATOM_TextRenderer::Render (ATOM_RenderDevice *device
								, int displayMode
								, const ATOM_Vector4f &textColor
								, const ATOM_Vector4f &outlineColor
								, int x
								, int y
								, float zval
								, int viewportW
								, int viewportH
								, bool outline)
{
  ATOM_STACK_TRACE(ATOM_TextRenderer::Render);
  float offsetX = -1.f / viewportW;// : 0.f;
  float offsetY = -1.f / viewportH;// : 0.f;
  float posX = 2.f * x / float(viewportW);
  float posY = 2.f * y / float(viewportH);
  ATOM_Matrix4x4f matrixWorld (
    2.f / viewportW, 0.f, 0.f, 0.f,
    0.f, -2.f / viewportH, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f,
    posX + offsetX - 1.f, -posY + offsetY + 1.f, zval, 1.f
    );
  matrixWorld.transpose();
  //ATOM_Matrix4x4f matrixWorld = ATOM_Matrix4x4f::getTranslateMatrix (ATOM_Vector3f(posX + offsetX, -posY + offsetY, 0.f));
  /*
  device->pushMatrix(ATOM_MATRIXMODE_WORLD);
  device->pushMatrix(ATOM_MATRIXMODE_VIEW);
  device->pushMatrix(ATOM_MATRIXMODE_PROJECTION);
  device->setTransform(ATOM_MATRIXMODE_WORLD, matrixWorld);
  device->loadIdentity(ATOM_MATRIXMODE_VIEW);
  device->loadIdentity(ATOM_MATRIXMODE_PROJECTION);
  */

  if (!textRenderStates)
  {
	  CreateStateSet (device);
  }

  switch (displayMode)
  {
  case ATOM_Text::DISPLAYMODE_2D:
	  textRenderStates->useDepth(0)->enableDepthWrite(false);
	  textRenderStates->useDepth(0)->setDepthFunc (ATOM_RenderAttributes::CompareFunc_Always);
	  break;
  case ATOM_Text::DISPLAYMODE_2D_ZTEST:
	  textRenderStates->useDepth(0)->enableDepthWrite(false);
	  textRenderStates->useDepth(0)->setDepthFunc (ATOM_RenderAttributes::CompareFunc_LessEqual);
	  break;
  case ATOM_Text::DISPLAYMODE_3D:
	  textRenderStates->useDepth(0)->enableDepthWrite(true);
	  textRenderStates->useDepth(0)->setDepthFunc (ATOM_RenderAttributes::CompareFunc_LessEqual);
	  break;
  }

  textGPUProgram->bind ();
  device->setConstantsVS (0, matrixWorld.m, 4);

  if (outline && outlineColor.w != 0.f)
  {
	device->setConstantsPS (0, outlineColor.xyzw, 1);
	InternalRender (device, outlineColor, true);
  }
  device->setConstantsPS (0, textColor.xyzw, 1);
  InternalRender (device, textColor, false);
  /*
  device->popMatrix(ATOM_MATRIXMODE_WORLD);
  device->popMatrix(ATOM_MATRIXMODE_VIEW);
  device->popMatrix(ATOM_MATRIXMODE_PROJECTION);
  */

  emptyGPUProgram->bind ();
}

void ATOM_TextRenderer::CreateStateSet (ATOM_RenderDevice *device)
{
	textRenderStates = device->allocRenderStateSet (0);
	textRenderStates->useAlphaBlending (0)->enableAlphaBlending (true);
	textRenderStates->useAlphaBlending (0)->setSrcBlend (ATOM_RenderAttributes::BlendFunc_SrcAlpha);
	textRenderStates->useAlphaBlending (0)->setDestBlend (ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
	textRenderStates->useSampler (0, 0)->setFilter (ATOM_RenderAttributes::SamplerFilter_PPP);

	emptyGPUProgram = device->allocGPUProgram (0);
	textGPUProgram = device->allocGPUProgram (0);

	ATOM_AUTOREF(ATOM_Shader) vs = device->allocVertexShader (0);
	vs->create (srcVS, "vs", ATOM_SHADER_COMPILE_TARGET_VS_2_0);
	textGPUProgram->addShader (vs.get());

	ATOM_AUTOREF(ATOM_Shader) ps = device->allocPixelShader (0);
	ps->create (srcPS, "ps", ATOM_SHADER_COMPILE_TARGET_PS_2_0);
	textGPUProgram->addShader (ps.get());
}

