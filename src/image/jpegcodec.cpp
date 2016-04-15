#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

extern"C" {
#include <jpeglib.h>
}

#define INPUT_BUFFER_SIZE 4096

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_JPEGCodec)
	ATOM_ATTRIBUTES_BEGIN(ATOM_JPEGCodec)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_JPEGCodec, ATOM_ImageCodec)

static jmp_buf jmpbuf;

ATOM_JPEGCodec::ATOM_JPEGCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::ATOM_JPEGCodec);
	_M_in_decode_begin = false;
	_M_in_encode_begin = false;
	_M_source_buffer = 0;
	_M_dest_buffer = 0;
}

ATOM_JPEGCodec::~ATOM_JPEGCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::~ATOM_JPEGCodec);
	if ( _M_source_buffer)
	{
		ATOM_DELETE_ARRAY(_M_source_buffer->buffer);
		ATOM_DELETE(_M_source_buffer);
	}
	if ( _M_dest_buffer)
	{
		ATOM_DELETE_ARRAY(_M_dest_buffer->buffer);
		ATOM_DELETE(_M_dest_buffer);
	}
}

bool ATOM_JPEGCodec::beginDecode (ATOM_RWops* input, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::beginDecode);
	if ( _M_in_decode_begin || _M_in_encode_begin)
	{
		return false;
	}

	if ( input == 0)
	{
		return false;
	}

	int length = input->size();
	if ( length <= 0)
	{
		return false;
	}

	_M_source_buffer = ATOM_NEW(SourceBuffer);
	_M_source_buffer->input = input;
	_M_source_buffer->buffer = ATOM_NEW_ARRAY(unsigned char, INPUT_BUFFER_SIZE);

	decode_cinfo.err = jpeg_std_error(&jerr);
	decode_cinfo.err->error_exit = &_S_error_throw;
	decode_cinfo.err->output_message = &_S_output_message;

	jpeg_create_decompress(&decode_cinfo);

	decode_cinfo.src = (struct jpeg_source_mgr *) _M_source_buffer;
	_M_source_buffer->jpeg_src.init_source = &_S_init_source_buffer;
	_M_source_buffer->jpeg_src.fill_input_buffer = &_S_fill_input_buffer;
	_M_source_buffer->jpeg_src.skip_input_data = &_S_buffer_skip_input_data;
	_M_source_buffer->jpeg_src.resync_to_restart = &_S_buffer_resync_to_restart;
	_M_source_buffer->jpeg_src.term_source = &_S_terminate_source;

	if ( setjmp(jmpbuf) == 0)
	{
		jpeg_read_header(&decode_cinfo, TRUE);
		jpeg_start_decompress(&decode_cinfo);
	}
	else
	{
		ATOM_LOGGER::error ("[ATOM_JPEGCodec::BeginDecode] failed to decode jpeg image.\n");
		ATOM_DELETE_ARRAY(_M_source_buffer->buffer);
		ATOM_DELETE(_M_source_buffer);
		_M_source_buffer = 0;
		return false;
	}

	if ( decode_cinfo.output_components != 3 && decode_cinfo.output_components != 1)
	{
		ATOM_DELETE_ARRAY(_M_source_buffer->buffer);
		ATOM_DELETE(_M_source_buffer);
		_M_source_buffer = 0;
		return false;
	}

	if ( info)
	{
		info->width = decode_cinfo.output_width;
		info->height = decode_cinfo.output_height;
		switch (decode_cinfo.output_components)
		{
		case 1: 
			{
				info->format = ATOM_PIXEL_FORMAT_GREY8; 
				break;
			}
		case 3:
			{
				info->format = ATOM_PIXEL_FORMAT_BGR888; 
				break;
			}
		}
		info->buffersize = info->width * info->height * decode_cinfo.output_components;
	}

	_M_in_decode_begin = true;
	info->codec = ATOM_ImageCodec::JPEG;

	return true;
}

bool ATOM_JPEGCodec::decode (ATOM_RWops* output, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::decode);
	if ( output == 0 || !_M_in_decode_begin)
	{
		return false;
	}

	int row_size = decode_cinfo.output_width* decode_cinfo.output_components;
	unsigned char * row_buffer = ATOM_NEW_ARRAY(unsigned char, row_size);

	if ( setjmp(jmpbuf) == 0)
	{
		while ( decode_cinfo.output_scanline < decode_cinfo.output_height)
		{
			jpeg_read_scanlines(&decode_cinfo, &row_buffer, 1);

			if (decode_cinfo.output_components == 3)
			{
				unsigned char *p = row_buffer;

				for (unsigned n = 0; n < decode_cinfo.output_width; ++n)
				{
					unsigned char tmp = p[0];
					p[0] = p[2];
					p[2] = tmp;
					p += 3;
				}
			}

			if ( row_size != output->write(row_buffer, 1, row_size))
			{
				_M_in_decode_begin = false;
				ATOM_DELETE_ARRAY(row_buffer);
				return false;
			}
		}
	}
	else
	{
		_M_in_decode_begin = false;
		ATOM_DELETE_ARRAY(row_buffer);

		ATOM_LOGGER::error ("[ATOM_JPEGCodec::Decode] failed to decode jpeg image.\n");
		return false;
	}

	ATOM_DELETE_ARRAY(row_buffer);
	return true;
}

void ATOM_JPEGCodec::endDecode (CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::endDecode);
	if ( !_M_in_decode_begin)
	{
		return;
	}

	if ( setjmp(jmpbuf) == 0)
	{
		jpeg_finish_decompress(&decode_cinfo);
		jpeg_destroy_decompress(&decode_cinfo);
	}
	else
	{
		ATOM_LOGGER::error("[ATOM_JPEGCodec::EndDecode] End decode failed.\n");
	}

	if ( _M_source_buffer)
	{
		ATOM_DELETE_ARRAY(_M_source_buffer->buffer);
		ATOM_DELETE(_M_source_buffer);
		_M_source_buffer = 0;
	}

	_M_in_decode_begin = false;
}

bool ATOM_JPEGCodec::beginEncode (ATOM_RWops* input, CodecInfo* info) 
{
	return false;
}

bool ATOM_JPEGCodec::encode (ATOM_RWops* output, CodecInfo* info) 
{
	return false;
}

void ATOM_JPEGCodec::endEncode (CodecInfo* info) 
{
	return;
}

bool ATOM_JPEGCodec::checkType (ATOM_RWops* input) const 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::checkType);
	if ( input == 0)
	{
		ATOM_LOGGER::error("[ATOM_JPEGCodec::CheckType] Invalid arguments: 0x%08X.\n", input);
		return false;
	}

	int pos = input->tell ();
	unsigned char sig[2];
	unsigned n = input->read (sig, 1, 2);
	input->seek (pos, SEEK_SET);
	return n == 2 && sig[0] == 0xFF && sig[1] == 0xD8;
}

void ATOM_JPEGCodec::_S_init_source_buffer (j_decompress_ptr decode_cinfo) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_init_source_buffer);
	SourceBuffer* buf = (SourceBuffer*) decode_cinfo->src;
	buf->jpeg_src.next_input_byte = 0;
	buf->jpeg_src.bytes_in_buffer = 0;
	buf->start = true;
}

boolean ATOM_JPEGCodec::_S_fill_input_buffer (j_decompress_ptr decode_cinfo) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_fill_input_buffer);
	SourceBuffer* buf = (SourceBuffer*) decode_cinfo->src;
	int bytesRead = buf->input->read (buf->buffer, 1, INPUT_BUFFER_SIZE);
	boolean result = true;
	if (bytesRead == 0 && buf->start)
	{
		buf->buffer[0] = 0xFF;
		buf->buffer[1] = JPEG_EOI;
		bytesRead = 2;
		result = false;
	}
	buf->jpeg_src.next_input_byte = buf->buffer;
	buf->jpeg_src.bytes_in_buffer = bytesRead;
	buf->start = false;
	return result;
}

void ATOM_JPEGCodec::_S_buffer_skip_input_data (j_decompress_ptr decode_cinfo, long skip) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_buffer_skip_input_data);
	SourceBuffer* buf = (SourceBuffer*) decode_cinfo->src;
	if ( skip > 0)
	{
		while (skip > (long)buf->jpeg_src.bytes_in_buffer)
		{
			skip -= (long)buf->jpeg_src.bytes_in_buffer;
			_S_fill_input_buffer (decode_cinfo);
		}
		buf->jpeg_src.next_input_byte += skip;
		buf->jpeg_src.bytes_in_buffer -= skip;
	}
}

void ATOM_JPEGCodec::_S_terminate_source (j_decompress_ptr decode_cinfo) 
{
}

boolean ATOM_JPEGCodec::_S_buffer_resync_to_restart (j_decompress_ptr decode_cinfo, int desired) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_buffer_resync_to_restart);
	return jpeg_resync_to_restart(decode_cinfo, desired);
}

void ATOM_JPEGCodec::_S_error_throw (j_common_ptr cinfo) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_error_throw);
	(*cinfo->err->output_message) (cinfo);
	jpeg_destroy(cinfo);
	longjmp(jmpbuf, 1);
}

void ATOM_JPEGCodec::_S_output_message (j_common_ptr cinfo) 
{
	ATOM_STACK_TRACE(ATOM_JPEGCodec::_S_output_message);
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message) (cinfo, buffer);
	ATOM_LOGGER::error("[ATOM_JPEGCodec] %s\n", buffer);
}

int ATOM_JPEGCodec::getCodecType (void) const 
{
	return ATOM_ImageCodec::JPEG;
}
