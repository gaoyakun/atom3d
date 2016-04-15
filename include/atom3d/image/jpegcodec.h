#ifndef __ATOM_IMAGE_IJPEGCODEC_H
#define __ATOM_IMAGE_IJPEGCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <jpeglib.h>
#include "imagecodec.h"

class ATOM_JPEGCodec : public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_JPEGCodec, ATOM_JPEGCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_JPEGCodec)

	struct SourceBuffer
	{
		struct jpeg_source_mgr jpeg_src;
		unsigned char* buffer;
		bool start;
		ATOM_RWops *input;
	};

	struct DestBuffer
	{
		struct jpeg_destination_mgr jpeg_dst;
		unsigned char* buffer;
		int size;
	};

public:
	ATOM_JPEGCodec (void);
	virtual ~ATOM_JPEGCodec (void);

public:
	virtual int getCodecType (void) const;
	virtual bool beginDecode (ATOM_RWops* input, CodecInfo* info);
	virtual bool decode (ATOM_RWops* output, CodecInfo* info);
	virtual void endDecode (CodecInfo* info);
	virtual bool beginEncode (ATOM_RWops* input, CodecInfo* info);
	virtual bool encode (ATOM_RWops* output, CodecInfo* info);
	virtual void endEncode (CodecInfo* info);
	virtual bool checkType (ATOM_RWops* input) const;

private:
	SourceBuffer* _M_source_buffer;
	DestBuffer* _M_dest_buffer;
	bool _M_in_decode_begin;
	bool _M_in_encode_begin;
	int _M_stream_pos;
	struct jpeg_decompress_struct decode_cinfo;
	struct jpeg_compress_struct encode_cinfo;
	struct jpeg_error_mgr jerr;

	static void _S_init_source_buffer(j_decompress_ptr cinfo);
	static boolean _S_fill_input_buffer(j_decompress_ptr cinfo);
	static void _S_buffer_skip_input_data(j_decompress_ptr cinfo, long size);
	static void _S_terminate_source(j_decompress_ptr cinfo);
	static boolean _S_buffer_resync_to_restart(j_decompress_ptr cinfo, int desired);

	static void _S_init_dest_buffer(j_compress_ptr cinfo);
	static boolean _S_empty_output_buffer(j_compress_ptr cinfo);
	static void _S_term_destination(j_compress_ptr cinfo);

	static void _S_error_throw(j_common_ptr cinfo);
	static void _S_output_message(j_common_ptr cinfo);
};

#endif // __ATOM_IMAGE_IJPEGCODEC_H
