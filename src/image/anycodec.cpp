#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_AnyCodec)
	ATOM_ATTRIBUTES_BEGIN(ATOM_AnyCodec)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_AnyCodec, ATOM_ImageCodec)

ATOM_AnyCodec::ATOM_AnyCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::ATOM_AnyCodec);
	_M_codec = 0;
}

ATOM_AnyCodec::~ATOM_AnyCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::~ATOM_AnyCodec);
	ATOM_DELETE(_M_codec);
}

ATOM_ImageCodec *ATOM_AnyCodec::testCodec (ATOM_RWops* input) const 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::testCodec);
	ATOM_ImageCodec *codec = 0;

	codec = ATOM_NEW(ATOM_DDSCodec);
	if (codec->checkType(input))
	{
		return codec;
	}
	else
	{
		ATOM_DELETE(codec);
	}

	codec = ATOM_NEW(ATOM_JPEGCodec);
	if ( codec->checkType(input))
	{
		return codec;
	}
	else
	{
		ATOM_DELETE(codec);
	}

	codec = ATOM_NEW(ATOM_PNGCodec);
	if ( codec->checkType(input))
	{
		return codec;
	}
	else
	{
		ATOM_DELETE(codec);
	}

	return 0;
}

bool ATOM_AnyCodec::checkType (ATOM_RWops* input) const 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::checkType);
	ATOM_ImageCodec *p = testCodec(input);
	if (p)
	{
		ATOM_DELETE(p);
		return true;
	}

	return false;
}

bool ATOM_AnyCodec::beginDecode (ATOM_RWops* input, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::beginDecode);
	if ( _M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::BeginDecode] Last decoding process not terminated.\n");
		return false;
	}

	_M_codec = testCodec(input);
	if ( _M_codec == 0)
	{
		return false;
	}

	if ( _M_codec->beginDecode(input, info))
	{
		return true;
	}

	ATOM_DELETE(_M_codec);
	_M_codec = 0;

	return false;
}

bool ATOM_AnyCodec::decode (ATOM_RWops* output, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::decode);
	if ( !_M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::Decode] BeginDecode not successfully called.\n");
		return false;
	}

	return _M_codec->decode(output, info);
}

void ATOM_AnyCodec::endDecode (CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::endDecode);
	if ( !_M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::EndDecode] BeginDecode not successfully called.\n");
		return;
	}

	_M_codec->endDecode(info);

	ATOM_DELETE(_M_codec);
	_M_codec = 0;
}

bool ATOM_AnyCodec::beginEncode (ATOM_RWops* input, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::beginEncode);
	if ( _M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::beginEncode] Last decoding process not terminated.\n");
		return false;
	}

	switch ( info->codec)
	{
	case ATOM_ImageCodec::PNG:
		{
			_M_codec = ATOM_NEW(ATOM_PNGCodec);
			break;
		}
	case ATOM_ImageCodec::JPEG:
		{
			_M_codec = ATOM_NEW(ATOM_JPEGCodec);
			break;
		}
	case ATOM_ImageCodec::DDS:
		{
			_M_codec = ATOM_NEW(ATOM_DDSCodec);
			break;
		}
	default:
		{
			ATOM_LOGGER::error ("[ATOM_AnyCodec::BeginEncode] Invalid image format requested.\n");
			return false;
		}
	}

	ATOM_ASSERT(_M_codec);

	if ( _M_codec->beginEncode(input, info))
	{
		return true;
	}

	ATOM_DELETE(_M_codec);
	_M_codec = 0;

	return false;
}

bool ATOM_AnyCodec::encode (ATOM_RWops* output, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::encode);
	if ( !_M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::encode] beginEncode not successfully called.\n");
		return false;
	}

	return _M_codec->encode(output, info);
}

void ATOM_AnyCodec::endEncode (CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_AnyCodec::endEncode);
	if ( !_M_codec)
	{
		ATOM_LOGGER::error ("[ATOM_AnyCodec::EndEncode] BeginEncode not successfully called.\n");
		return;
	}

	_M_codec->endEncode(info);
	ATOM_DELETE(_M_codec);
	_M_codec = 0;
}

int ATOM_AnyCodec::getCodecType (void) const 
{
	return ATOM_ImageCodec::ANY;
}
