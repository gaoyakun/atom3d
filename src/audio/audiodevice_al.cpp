#include "audiodevice_al.h"
#include "alinit.h"
#include <ATOM_kernel.h>

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	ATOM_STACK_TRACE(ov_read_func);

	return ATOM_VFS::vfsRead ((ATOM_VFS::handle)datasource, ptr, nmemb);
	//return fread(ptr, size, nmemb, (FILE*)datasource);
}

int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	ATOM_STACK_TRACE(ov_seek_func);

	return ATOM_VFS::vfsSeek((ATOM_VFS::handle)datasource,(long) offset, whence);
	//return fseek((FILE*)datasource, (long)offset, whence);
}

int ov_close_func(void *datasource)
{
	//ATOM_STACK_TRACE(ov_close_func);
	//ATOM_VFS::vfsClose((ATOM_VFS::handle)datasource);
	//ATOM_DELETE(ATOM_VFS::)
	return 0;
	/*return fclose((FILE*)datasource);*/
}

long ov_tell_func(void *datasource)
{
	ATOM_STACK_TRACE(ov_tell_func);
	return ATOM_VFS::vfsTell((ATOM_VFS::handle)datasource);
	//return ftell((FILE*)datasource);
}

unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
	ATOM_STACK_TRACE(DecodeOggVorbis);

	int current_section;
	long lDecodeSize;
	unsigned long ulSamples;
	short *pSamples;

	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (ulChannels == 6)
	{		
		pSamples = (short*)pDecodeBuffer;
		for (ulSamples = 0; ulSamples < (ulBufferSize>>1); ulSamples+=6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			std::swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
			std::swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
			std::swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
		}
	}

	return ulBytesDone;
}

//////////////////////////////////////////////////////////////////////////


ATOM_Audio_al::ATOM_Audio_al()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::ATOM_Audio_al);

	_state = AUDIOSTATE_UNLOAD;
	_volumn = 1.f;
}

ATOM_Audio_al::~ATOM_Audio_al()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::~ATOM_Audio_al);

	stop();
}

bool ATOM_Audio_al::load(const char* filename)
{
	ATOM_STACK_TRACE(ATOM_Audio_al::load);

	stop();

	_callbacks.read_func = ov_read_func;
	_callbacks.seek_func = ov_seek_func;
	_callbacks.close_func = ov_close_func;
	_callbacks.tell_func = ov_tell_func;

	// Open the OggVorbis file
//	char path[MAX_PATH];
	//ATOM_GetNativePathName(filename, path);
	//ATOM_GetPhysicalPathName(filename, path);
	_file = ATOM_OpenFile(filename, ATOM_VFS::binary|ATOM_VFS::read);
	//_file(filename,ATOM_VFS::binary|ATOM_VFS::read);
	//_file = fopen(path, "rb");
	if (!_file)
	{
		return false;
	}

	// Create an OggVorbis file stream
	if (ov_open_callbacks((void *)_file->getHandle(), &_ovFile, NULL, 0, _callbacks))
	{
		//fclose(_file);
		ATOM_CloseFile(_file);
		return false;
	}

	// Get some information about the file (Channels, Format, and Frequency)
	_ovInfo = ov_info(&_ovFile, -1);
	if (NULL == _ovInfo)
	{
		ov_clear(&_ovFile);
		ATOM_CloseFile(_file);
		//fclose(_file);
		return false;
	}

	_ulFrequency = _ovInfo->rate;
	_ulChannels = _ovInfo->channels;
	if (_ovInfo->channels == 1)
	{
		_ulFormat = AL_FORMAT_MONO16;
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		_ulBufferSize = _ulFrequency >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		_ulBufferSize -= (_ulBufferSize % 2);
	}
	else if (_ovInfo->channels == 2)
	{
		_ulFormat = AL_FORMAT_STEREO16;
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		_ulBufferSize = _ulFrequency;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		_ulBufferSize -= (_ulBufferSize % 4);
	}
	else if (_ovInfo->channels == 4)
	{
		_ulFormat = pfn_alGetEnumValue("AL_FORMAT_QUAD16");
		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		_ulBufferSize = _ulFrequency * 2;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		_ulBufferSize -= (_ulBufferSize % 8);
	}
	else if (_ovInfo->channels == 6)
	{
		_ulFormat = pfn_alGetEnumValue("AL_FORMAT_51CHN16");
		// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		_ulBufferSize = _ulFrequency * 3;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		_ulBufferSize -= (_ulBufferSize % 12);
	}
	else
	{
		ov_clear(&_ovFile);
		//fclose(_file);
		ATOM_CloseFile(_file);
		return false;
	}

	// Allocate a buffer to be used to store decoded data for all Buffers
	_decodeBuffer = (char*)ATOM_MALLOC(_ulBufferSize);

	// Generate some AL Buffers for streaming
	pfn_alGenBuffers( NUMBUFFERS, _uiBuffers );

	// Generate a Source to playback the Buffers
	pfn_alGenSources( 1, &_uiSource );

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	unsigned long ulBytesWritten;
	for (int iLoop = 0; iLoop < NUMBUFFERS; iLoop++)
	{
		ulBytesWritten = DecodeOggVorbis(&_ovFile, _decodeBuffer, _ulBufferSize, _ulChannels);
		if (ulBytesWritten)
		{
			pfn_alBufferData(_uiBuffers[iLoop], _ulFormat, _decodeBuffer, ulBytesWritten, _ulFrequency);
			pfn_alSourceQueueBuffers(_uiSource, 1, &_uiBuffers[iLoop]);
		}
	}

	_state = AUDIOSTATE_PAUSE;

	return true;
}

bool ATOM_Audio_al::play()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::play);

	if(false == load(_filename.c_str()))
	{
		ATOM_LOGGER::error("load %s failed.\n", _filename.c_str());
		return false;
	}

	//ATOM_LOGGER::information("play %s.\n", _filename.c_str());
	// Start playing source
	pfn_alSourcef(_uiSource, AL_MIN_GAIN, 0.f);
	pfn_alSourcef(_uiSource, AL_MAX_GAIN, 1.f);
	pfn_alSourcePlay(_uiSource);
	pfn_alSourcef(_uiSource, AL_GAIN, _volumn);
	_state = AUDIOSTATE_PLAYING;
	return true;
}

void ATOM_Audio_al::stop()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::stop);

	if(AUDIOSTATE_UNLOAD == _state)
	{
		return;
	}

	//ATOM_LOGGER::information("destroy audio.\n");
	pfn_alSourceStop(_uiSource);
	pfn_alSourcei(_uiSource, AL_BUFFER, 0);

	if (_decodeBuffer)
	{
		ATOM_FREE(_decodeBuffer);
		_decodeBuffer = NULL;
	}

	// Clean up buffers and sources
	pfn_alDeleteSources( 1, &_uiSource );
	pfn_alDeleteBuffers( NUMBUFFERS, _uiBuffers );

	ov_clear(&_ovFile);

	if(_file)
	{
		//fclose(_file);
		ATOM_CloseFile(_file);
		_file = NULL;
	}	
	_state = AUDIOSTATE_UNLOAD;
}

void ATOM_Audio_al::pause()
{

}

void ATOM_Audio_al::update()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::update);

	if(AUDIOSTATE_PLAYING != _state)
	{
		return;
	}

	// Request the number of OpenAL Buffers have been processed (played) on the Source
	ALint			iBuffersProcessed = 0;
	ALuint		    uiBuffer;
	unsigned long	ulBytesWritten;
	ALint			iState;
	ALint			iQueuedBuffers;

	pfn_alGetSourcei(_uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

	// Keep a running count of number of buffers processed (for logging purposes only)

	// For each processed buffer, remove it from the Source Queue, read next chunk of audio
	// data from disk, fill buffer with new data, and add it to the Source Queue
	while (iBuffersProcessed)
	{
		// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
		uiBuffer = 0;
		pfn_alSourceUnqueueBuffers(_uiSource, 1, &uiBuffer);

		// Read more audio data (if there is any)
		ulBytesWritten = DecodeOggVorbis(&_ovFile, _decodeBuffer, _ulBufferSize, _ulChannels);
		if (ulBytesWritten)
		{
			pfn_alBufferData(uiBuffer, _ulFormat, _decodeBuffer, ulBytesWritten, _ulFrequency);
			pfn_alSourceQueueBuffers(_uiSource, 1, &uiBuffer);
		}

		iBuffersProcessed--;
	}

	// Check the status of the Source.  If it is not playing, then playback was completed,
	// or the Source was starved of audio data, and needs to be restarted.
	pfn_alGetSourcei(_uiSource, AL_SOURCE_STATE, &iState);
	if (iState != AL_PLAYING)
	{
		// If there are Buffers in the Source Queue then the Source was starved of audio
		// data, so needs to be restarted (because there is more audio data to play)
		pfn_alGetSourcei(_uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		if (iQueuedBuffers)
		{
			pfn_alSourcePlay(_uiSource);
		}
		else
		{
			// Finished playing
			if(_loop)
			{
				//ATOM_LOGGER::information("loop\n");
				play();
			}
			else
			{
				stop();
			}			
		}
	}
}

ATOM_AUDIOSTATE ATOM_Audio_al::getState()
{
	ATOM_STACK_TRACE(ATOM_Audio_al::getState);

	return _state;
}

void ATOM_Audio_al::setFilename(const char* filename)
{
	ATOM_STACK_TRACE(ATOM_Audio_al::setFilename);

	_filename = filename;
}


const char* ATOM_Audio_al::getFilename() const
{
	ATOM_STACK_TRACE(ATOM_Audio_al::getFilename);

	return _filename.c_str();
}

void ATOM_Audio_al::setLoop(bool loop)
{
	ATOM_STACK_TRACE(ATOM_Audio_al::setLoop);

	_loop = loop;
}

bool ATOM_Audio_al::getLoop() const
{
	ATOM_STACK_TRACE(ATOM_Audio_al::getLoop);

	return _loop;
}

void ATOM_Audio_al::setVolumn(float vol)
{
	ATOM_STACK_TRACE(ATOM_Audio_al::setVolumn);

	_volumn = vol;
	pfn_alSourcef(_uiSource, AL_GAIN, vol);
}

float ATOM_Audio_al::getVolumn() const
{
	ATOM_STACK_TRACE(ATOM_Audio_al::getVolumn);

	return _volumn;
}

//////////////////////////////////////////////////////////////////////////

ATOM_AudioDevice_al::ATOM_AudioDevice_al()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::ATOM_AudioDevice_al);

	_context = 0;
}

ATOM_AudioDevice_al::~ATOM_AudioDevice_al()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::~ATOM_AudioDevice_al);
}

bool ATOM_AudioDevice_al::initialize(const char *device)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::initialize);

	if( !device )
	{
		ATOM_LOGGER::error ("[ATOM_AudioDevice_al::initialize]: Invalid params.\r\n");
		return false;
	}

	if ( !initOpenALLib ())
	{
		ATOM_LOGGER::error ("[ATOM_AudioDevice_al::initialize]: Initialize OpenAL failed.\r\n");
		return false;
	}

	ALCdevice* p_device				= pfn_alcOpenDevice(device);
	if( pfn_alcGetError(p_device) != ALC_NO_ERROR )
		return false;
	_context			= pfn_alcCreateContext(p_device, NULL);
	if( pfn_alcGetError(p_device) != ALC_NO_ERROR )
		return false;
	pfn_alcMakeContextCurrent(_context);
	if( pfn_alcGetError(p_device) != ALC_NO_ERROR )
		return false;

	pfn_alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	if(pfn_alGetError() != ALC_NO_ERROR )
	{
		ATOM_LOGGER::information ("[ATOM_AudioDevice_al::initialize]:AL_INVERSE_DISTANCE_CLAMPED not supported.\r\n");
	}
	return true;
}

void ATOM_AudioDevice_al::deInitialize()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::deInitialize);

	if (_context)
	{
		ALCdevice *device =pfn_alcGetContextsDevice(_context);  
		pfn_alcMakeContextCurrent(NULL);  
		pfn_alcDestroyContext(_context);  
		pfn_alcCloseDevice(device);  
		_context = NULL;
	}
}

ATOM_Audio* ATOM_AudioDevice_al::createAudio()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::createAudio);

	return ATOM_NEW(ATOM_Audio_al);
}

void ATOM_AudioDevice_al::destroyAudio(ATOM_Audio* audio)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_al::destroyAudio);

	if(audio)
	{
		ATOM_DELETE(audio);
		audio = NULL;
	}
}
