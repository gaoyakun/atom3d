#ifndef ATOM_AUDIO_AUDIODEVICE_H_
#define ATOM_AUDIO_AUDIODEVICE_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include <Windows.h>
#include "basedefs.h"
#include "../ATOM_dbghlp.h"

#define INVALID_AUDIOID 0

enum ATOM_AUDIOSTATE
{
	AUDIOSTATE_UNLOAD = 0,
	AUDIOSTATE_PLAYING,
	AUDIOSTATE_PAUSE,
};

enum ATOM_AUDIOCOMMAND
{								// value
	AUDIOCOMMAND_NULL,			// 
	AUDIOCOMMAND_PLAYSOUND,		// soundid
	AUDIOCOMMAND_PLAYBGM,		// soundid
	AUDIOCOMMAND_STOPSOUND,		// 
	AUDIOCOMMAND_STOPBGM,		// 
	AUDIOCOMMAND_SOUNDVOLUMN,	// volumn
	AUDIOCOMMAND_BGMVOLUMN,		// volumn
	AUDIOCOMMAND_ENABLESOUND,	// enable
	AUDIOCOMMAND_ENABLEBGM,		// enable
};

struct ATOM_AudioCommand
{
	ATOM_AUDIOCOMMAND _type;
	int _value;
};

class ATOM_AUDIO_API ATOM_Audio
{
public:
	virtual ~ATOM_Audio(){}

	virtual bool play() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void update() = 0;
	virtual ATOM_AUDIOSTATE getState() = 0;
	virtual void setFilename(const char* filename) = 0;
	virtual const char* getFilename() const = 0;
	virtual void setLoop(bool loop) = 0;
	virtual bool getLoop() const = 0;
	virtual void setVolumn(float vol) = 0;
	virtual float getVolumn() const = 0;
};

class ATOM_AUDIO_API ATOM_AudioDevice
{
public:
	//! 构造函数
	ATOM_AudioDevice(){}

	//! 析构函数
	virtual ~ATOM_AudioDevice(){}

	
	//! 初始化
	virtual bool initialize(const char* filename) = 0;

	//! 销毁
	virtual void deInitialize() = 0;

	//! 创建声音
	virtual ATOM_Audio* createAudio() = 0;

	//! 销毁声音
	virtual void destroyAudio(ATOM_Audio* audio) = 0;
};


class ATOM_AUDIO_API ATOM_AudioDeviceWrapper
{
public:
	//! 构造函数
	ATOM_AudioDeviceWrapper(const char* deviceName, const char* configFilename);

	//! 析构函数
	virtual ~ATOM_AudioDeviceWrapper();

	void playSound(int id);
	void playBgm(int id);
	void stopSound();
	void stopBgm();
	void setSoundVolumn(float vol);
	float getSoundVolumn() const;
	void setBgmVolumn(float vol);
	float getBgmVolumn() const;
	void enableSound(bool enable);
	bool getEnableSound() const;
	void enableBgm(bool enable);
	bool getEnableBgm() const;	

protected:
	virtual void postCommand(ATOM_AUDIOCOMMAND cmd, int value);

	static DWORD WINAPI AudioWorkThread(LPVOID lpThreadParameter);

protected:
	ATOM_STRING _deviceName;
	ATOM_STRING _configFilename;
	CRITICAL_SECTION _commandLock;
	ATOM_QUEUE<ATOM_AudioCommand> _commands;
	bool _stop;
	bool _threadFinished;
	float _soundVolumn;
	float _bgmVolumn;
	bool _enableSound;
	bool _enableBgm;
};

ATOM_AUDIO_API ATOM_AudioDevice* ATOM_CALL ATOM_CreateAudioDevice(const char* name);
ATOM_AUDIO_API void ATOM_CALL ATOM_DestroyAudioDevice(ATOM_AudioDevice* device);

#endif//ATOM_AUDIO_AUDIODEVICE_H_
