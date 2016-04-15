#ifndef ATOM_AUDIO_AL_AUDIODEVICE_AL_H_
#define ATOM_AUDIO_AL_AUDIODEVICE_AL_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "audiodevice.h"
#include "alinit.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

class ATOM_File;
#define NUMBUFFERS 4
class ATOM_Audio_al : public ATOM_Audio
{
public:
	ATOM_Audio_al();
	virtual ~ATOM_Audio_al();

	virtual bool load(const char* filename);
	virtual bool play();
	virtual void stop();
	virtual void pause();
	virtual void update();
	virtual ATOM_AUDIOSTATE getState();
	virtual void setFilename(const char* filename);
	virtual const char* getFilename() const;
	virtual void setLoop(bool loop);
	virtual bool getLoop() const;
	virtual void setVolumn(float vol);
	virtual float getVolumn() const;

	ov_callbacks	_callbacks;
	OggVorbis_File	_ovFile;
	vorbis_info*	_ovInfo;
	ATOM_File *		_file;
	long			_ulFrequency;
	int				_ulChannels;
	int				_ulFormat;
	long			_ulBufferSize;
	char*			_decodeBuffer;
	ALuint		    _uiBuffers[NUMBUFFERS];
	ALuint		    _uiSource;
	ATOM_STRING		_filename;
	ATOM_AUDIOSTATE		_state;
	bool			_loop;
	float			_volumn;
};


class ATOM_AudioDevice_al : public ATOM_AudioDevice
{
public:
	//! 构造函数
	ATOM_AudioDevice_al();

	//! 析构函数
	virtual ~ATOM_AudioDevice_al();

	//! 初始化
	virtual bool initialize(const char* device);

	//! 销毁
	virtual void deInitialize();

	//! 创建声音
	virtual ATOM_Audio* createAudio();

	//! 销毁声音
	virtual void destroyAudio(ATOM_Audio* audio);	

private:
	ALCcontext *_context;
};

#endif//ATOM_AUDIO_AL_AUDIODEVICE_AL_H_
