/**	\file animatedtexturechannel.h
 *	顶点常量的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_VERTEXATTRIB_H
#define __ATOM_GLRENDER_VERTEXATTRIB_H

//  We only support up to ATOM_MAX_TEXTURE_LAYER_COUNT texture stages per vertex.
enum {
  ATOM_MAX_TEXTURE_LAYER_COUNT = 8,
};

// assumes 32 bits word long
enum 
{
  // General vertex attributes

  ATOM_VERTEX_ATTRIB_COORD = (1<<0),         // (float, float, float)
  ATOM_VERTEX_ATTRIB_NORMAL = (1<<1),         // (float, float, float)
  ATOM_VERTEX_ATTRIB_PRIMARY_COLOR = (1<<2),         // unsigned
  ATOM_VERTEX_ATTRIB_COORD_XYZW = (1<<3),         // (float float float float)
  ATOM_VERTEX_ATTRIB_COORD_XYZRHW = (1<<4),		// (float float float float)
  ATOM_VERTEX_ATTRIB_PSIZE = (1<<5),				// float
  ATOM_VERTEX_ATTRIB_TANGENT = (1<<6),				// (float, float, float)
  ATOM_VERTEX_ATTRIB_BINORMAL = (1<<7),				// (float, float, float)

  // TexCoord may be 1 component or 2 components or 3 components
  // We must dealing this at every texture stage.

  ATOM_VERTEX_ATTRIB_TEX1_1 = (1 << 29),               // float
  ATOM_VERTEX_ATTRIB_TEX1_2 = (2 << 29),               // (float, float)
  ATOM_VERTEX_ATTRIB_TEX1_3 = (3 << 29),               // (float, float, float)
  ATOM_VERTEX_ATTRIB_TEX1_4 = (4 << 29),
  ATOM_VERTEX_ATTRIB_TEX1_MASK = ATOM_VERTEX_ATTRIB_TEX1_1|ATOM_VERTEX_ATTRIB_TEX1_2|ATOM_VERTEX_ATTRIB_TEX1_4,
  ATOM_VERTEX_ATTRIB_TEX1_SHIFT = 29,
  ATOM_VERTEX_ATTRIB_TEX2_1 = (1 << 26),
  ATOM_VERTEX_ATTRIB_TEX2_2 = (2 << 26),
  ATOM_VERTEX_ATTRIB_TEX2_3 = (3 << 26),
  ATOM_VERTEX_ATTRIB_TEX2_4 = (4 << 26),
  ATOM_VERTEX_ATTRIB_TEX2_MASK = ATOM_VERTEX_ATTRIB_TEX2_1|ATOM_VERTEX_ATTRIB_TEX2_2|ATOM_VERTEX_ATTRIB_TEX2_4,
  ATOM_VERTEX_ATTRIB_TEX2_SHIFT = 26,
  ATOM_VERTEX_ATTRIB_TEX3_1 = (1 << 23),
  ATOM_VERTEX_ATTRIB_TEX3_2 = (2 << 23),
  ATOM_VERTEX_ATTRIB_TEX3_3 = (3 << 23),
  ATOM_VERTEX_ATTRIB_TEX3_4 = (4 << 23),
  ATOM_VERTEX_ATTRIB_TEX3_MASK = ATOM_VERTEX_ATTRIB_TEX3_1|ATOM_VERTEX_ATTRIB_TEX3_2|ATOM_VERTEX_ATTRIB_TEX3_4,
  ATOM_VERTEX_ATTRIB_TEX3_SHIFT = 23,
  ATOM_VERTEX_ATTRIB_TEX4_1 = (1 << 20),
  ATOM_VERTEX_ATTRIB_TEX4_2 = (2 << 20),
  ATOM_VERTEX_ATTRIB_TEX4_3 = (3 << 20),
  ATOM_VERTEX_ATTRIB_TEX4_4 = (4 << 20),
  ATOM_VERTEX_ATTRIB_TEX4_MASK = ATOM_VERTEX_ATTRIB_TEX4_1|ATOM_VERTEX_ATTRIB_TEX4_2|ATOM_VERTEX_ATTRIB_TEX4_4,
  ATOM_VERTEX_ATTRIB_TEX4_SHIFT = 20,
  ATOM_VERTEX_ATTRIB_TEX5_1 = (1 << 17),
  ATOM_VERTEX_ATTRIB_TEX5_2 = (2 << 17),
  ATOM_VERTEX_ATTRIB_TEX5_3 = (3 << 17),
  ATOM_VERTEX_ATTRIB_TEX5_4 = (4 << 17),
  ATOM_VERTEX_ATTRIB_TEX5_MASK = ATOM_VERTEX_ATTRIB_TEX5_1|ATOM_VERTEX_ATTRIB_TEX5_2|ATOM_VERTEX_ATTRIB_TEX5_4,
  ATOM_VERTEX_ATTRIB_TEX5_SHIFT = 17,
  ATOM_VERTEX_ATTRIB_TEX6_1 = (1 << 14),
  ATOM_VERTEX_ATTRIB_TEX6_2 = (2 << 14),
  ATOM_VERTEX_ATTRIB_TEX6_3 = (3 << 14),
  ATOM_VERTEX_ATTRIB_TEX6_4 = (4 << 14),
  ATOM_VERTEX_ATTRIB_TEX6_MASK = ATOM_VERTEX_ATTRIB_TEX6_1|ATOM_VERTEX_ATTRIB_TEX6_2|ATOM_VERTEX_ATTRIB_TEX6_4,
  ATOM_VERTEX_ATTRIB_TEX6_SHIFT = 14,
  ATOM_VERTEX_ATTRIB_TEX7_1 = (1 << 11),
  ATOM_VERTEX_ATTRIB_TEX7_2 = (2 << 11),
  ATOM_VERTEX_ATTRIB_TEX7_3 = (3 << 11),
  ATOM_VERTEX_ATTRIB_TEX7_4 = (4 << 11),
  ATOM_VERTEX_ATTRIB_TEX7_MASK = ATOM_VERTEX_ATTRIB_TEX7_1|ATOM_VERTEX_ATTRIB_TEX7_2|ATOM_VERTEX_ATTRIB_TEX7_4,
  ATOM_VERTEX_ATTRIB_TEX7_SHIFT = 11,
  ATOM_VERTEX_ATTRIB_TEX8_1 = (1 << 8),
  ATOM_VERTEX_ATTRIB_TEX8_2 = (2 << 8),
  ATOM_VERTEX_ATTRIB_TEX8_3 = (3 << 8),
  ATOM_VERTEX_ATTRIB_TEX8_4 = (4 << 8),
  ATOM_VERTEX_ATTRIB_TEX8_MASK = ATOM_VERTEX_ATTRIB_TEX8_1|ATOM_VERTEX_ATTRIB_TEX8_2|ATOM_VERTEX_ATTRIB_TEX8_4,
  ATOM_VERTEX_ATTRIB_TEX8_SHIFT = 8,
  ATOM_VERTEX_ATTRIB_ALL = ATOM_VERTEX_ATTRIB_COORD | ATOM_VERTEX_ATTRIB_NORMAL | ATOM_VERTEX_ATTRIB_PSIZE | ATOM_VERTEX_ATTRIB_PRIMARY_COLOR | ATOM_VERTEX_ATTRIB_TANGENT | ATOM_VERTEX_ATTRIB_BINORMAL | ATOM_VERTEX_ATTRIB_TEX1_MASK | ATOM_VERTEX_ATTRIB_TEX2_MASK | ATOM_VERTEX_ATTRIB_TEX3_MASK | ATOM_VERTEX_ATTRIB_TEX4_MASK | ATOM_VERTEX_ATTRIB_TEX5_MASK | ATOM_VERTEX_ATTRIB_TEX6_MASK | ATOM_VERTEX_ATTRIB_TEX7_MASK | ATOM_VERTEX_ATTRIB_TEX8_MASK,
  ATOM_VERTEX_ATTRIB_COORD_MASK = ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_COORD_XYZRHW ,
};

inline int ATOM_VERTEX_ATTRIB_TEX_SHIFT(int layer) {
  return 32 - ((layer + 1) * 3);
}

inline unsigned ATOM_VERTEX_ATTRIB_TEX(int layer, int components) {
  return (unsigned) (components << ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer));
}

inline unsigned ATOM_VERTEX_ATTRIB_TEX_MASK(int layer) {
  return (unsigned) (7 << ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer));
}

inline int ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(unsigned attrib, int layer) {
  return (attrib & ATOM_VERTEX_ATTRIB_TEX_MASK(layer)) >> ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer);
}


//-----------------------------------------------------//
// wangjian modified for testing float16

#if 0

inline int ATOM_GetVertexSize(unsigned Attributes) {
  int ret = 0;

  if ( Attributes & ATOM_VERTEX_ATTRIB_COORD)
  {
	  ret += 3 * sizeof(float);
  }
  else if (Attributes & ATOM_VERTEX_ATTRIB_COORD_XYZW)
  {
    ret += 4 * sizeof(float);
  }
  else if (Attributes & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
  {
    ret += 4 * sizeof(float);
  }

  if ( Attributes & ATOM_VERTEX_ATTRIB_NORMAL)
  {
    ret += 3 * sizeof(float);
  }

  if ( Attributes & ATOM_VERTEX_ATTRIB_PSIZE)
  {
	ret += sizeof(float);
  }

  if ( Attributes & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
  {
    ret += sizeof(unsigned);
  }

  if ( Attributes & ATOM_VERTEX_ATTRIB_TANGENT)
  {
    ret += 3 * sizeof(float);
  }

  if ( Attributes & ATOM_VERTEX_ATTRIB_BINORMAL)
  {
    ret += 3 * sizeof(float);
  }

  for (int i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
  {
    ret += sizeof(float) * ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(Attributes, i);
  }

  return ret;
}

#else

enum
{
	ATTRIBUTE_FLAG_NONE = 0,
	ATTRIBUTE_FLAG_COMPRESSED = 1 << 0,		// 使用压缩格式
	ATTRIBUTE_FLAG_BUMPMAPPING = 1 << 1,	// 使用法线映射
	ATTRIBUTE_FLAG_TOTAL,
};

inline int ATOM_GetVertexSize(unsigned Attributes,unsigned flags = ATTRIBUTE_FLAG_NONE ) 
{
	int ret = 0;

	if ( Attributes & ATOM_VERTEX_ATTRIB_COORD)
	{
		if( flags & ATTRIBUTE_FLAG_COMPRESSED )
			ret += 2 * sizeof(float);
		else
			ret += 3 * sizeof(float);
	}
	else if (Attributes & ATOM_VERTEX_ATTRIB_COORD_XYZW)
	{
		ret += 4 * sizeof(float);
	}
	else if (Attributes & ATOM_VERTEX_ATTRIB_COORD_XYZRHW)
	{
		ret += 4 * sizeof(float);
	}

	if ( Attributes & ATOM_VERTEX_ATTRIB_NORMAL)
	{
		if( flags & ATTRIBUTE_FLAG_COMPRESSED )
		{
			if( flags & ATTRIBUTE_FLAG_BUMPMAPPING )		// 如果使用法线映射 则需要使用float16_4(8个字节）
				ret += 2 * sizeof(float);
			else											// 否则，只需要使用到float16_2（4个字节）
				ret += 2 * sizeof(float);
		}
		else
			ret += 3 * sizeof(float);
	}

	if ( Attributes & ATOM_VERTEX_ATTRIB_TANGENT)
	{
		if( flags & ATTRIBUTE_FLAG_COMPRESSED )
#if 0
			ret += 0;								// + 0 因为我们把切线信息保存到法线属性中去
#else
			ret += 2 * sizeof(float);
#endif
		else
			ret += 3 * sizeof(float);
	}

	if ( Attributes & ATOM_VERTEX_ATTRIB_BINORMAL)
	{
		if( flags & ATTRIBUTE_FLAG_COMPRESSED )
#if 0
			ret += 0;								// + 0 因为我们不需要传副法线信息
#else
			ret += 2 * sizeof(float);
#endif
		else
			ret += 3 * sizeof(float);
	}

	if ( Attributes & ATOM_VERTEX_ATTRIB_PSIZE)
	{
		ret += sizeof(float);
	}

	if ( Attributes & ATOM_VERTEX_ATTRIB_PRIMARY_COLOR)
	{
		ret += sizeof(unsigned);
	}

	for (int i = 0; i < ATOM_MAX_TEXTURE_LAYER_COUNT; ++i)
	{
		if( flags & ATTRIBUTE_FLAG_COMPRESSED )
			ret += sizeof(unsigned short) * ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(Attributes, i);
		else
			ret += sizeof(float) * ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(Attributes, i);
	}

	return ret;
}


inline bool ATOM_canUseCompress(unsigned Attributes) 
{
#define ATOM_VERTEX_ATTRIB_BLENDWEIGHTS ATOM_VERTEX_ATTRIB_TEX2_4
#define ATOM_VERTEX_ATTRIB_BLENDINDICES ATOM_VERTEX_ATTRIB_TEX3_4

	return	(	Attributes & (	ATOM_VERTEX_ATTRIB_COORD | ATOM_VERTEX_ATTRIB_NORMAL | 
								ATOM_VERTEX_ATTRIB_TANGENT | ATOM_VERTEX_ATTRIB_BINORMAL |
								ATOM_VERTEX_ATTRIB_BLENDWEIGHTS | ATOM_VERTEX_ATTRIB_BLENDINDICES |
								ATOM_VERTEX_ATTRIB_TEX1_2 | ATOM_VERTEX_ATTRIB_TEX1_4 | 
								ATOM_VERTEX_ATTRIB_TEX2_2 | ATOM_VERTEX_ATTRIB_TEX2_4 | 
								ATOM_VERTEX_ATTRIB_TEX3_2 | ATOM_VERTEX_ATTRIB_TEX3_4 | 
								ATOM_VERTEX_ATTRIB_TEX4_2 | ATOM_VERTEX_ATTRIB_TEX4_4 | 
								ATOM_VERTEX_ATTRIB_TEX5_2 | ATOM_VERTEX_ATTRIB_TEX5_4 | 
								ATOM_VERTEX_ATTRIB_TEX6_2 | ATOM_VERTEX_ATTRIB_TEX6_4 | 
								ATOM_VERTEX_ATTRIB_TEX7_2 | ATOM_VERTEX_ATTRIB_TEX7_4 | 
								ATOM_VERTEX_ATTRIB_TEX8_2 | ATOM_VERTEX_ATTRIB_TEX8_4 ) );
}

inline unsigned ATOM_getCompressAttribComponent(unsigned attribute, unsigned & uncompressd )
{
	if( !ATOM_canUseCompress(attribute) )
		return 0;

#define ATOM_VERTEX_ATTRIB_BLENDWEIGHTS ATOM_VERTEX_ATTRIB_TEX2_4
#define ATOM_VERTEX_ATTRIB_BLENDINDICES ATOM_VERTEX_ATTRIB_TEX3_4

	unsigned compressed = 0;

	switch( attribute )
	{
	case ATOM_VERTEX_ATTRIB_COORD:
	case ATOM_VERTEX_ATTRIB_NORMAL:
	case ATOM_VERTEX_ATTRIB_TANGENT:
	case ATOM_VERTEX_ATTRIB_BINORMAL:
		uncompressd = 3;
		compressed = 4;
		break;

	case ATOM_VERTEX_ATTRIB_BLENDWEIGHTS :
	case ATOM_VERTEX_ATTRIB_BLENDINDICES :
		uncompressd = 4;
		compressed = 4;
		break;

	default:
		compressed = uncompressd = ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attribute,0);
		break;
	}

	return compressed;
}

#endif
/*
#define ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer) (32 - (((layer) + 1)<<1))
    // ATOM_VERTEX_ATTRIB_TEX(0, 1) equals ATOM_VERTEX_ATTRIB_TEX1_1, etc.
#define ATOM_VERTEX_ATTRIB_TEX(layer, components) \
    ((TVertexAttrib)((components) << ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer)))
#define ATOM_VERTEX_ATTRIB_TEX_MASK(layer) ATOM_VERTEX_ATTRIB_TEX(layer, 3)
#define ATOM_VERTEX_ATTRIB_TEX_COMPONENTS(attrib, layer) \
    (((attrib) & ATOM_VERTEX_ATTRIB_TEX_MASK(layer)) >> ATOM_VERTEX_ATTRIB_TEX_SHIFT(layer))
*/

#endif // __ATOM_GLRENDER_VERTEXATTRIB_H
/*! @} */
