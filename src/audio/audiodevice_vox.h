#ifndef ATOM_AUDIO_AUDIODEVICE_VOX_H_
#define ATOM_AUDIO_AUDIODEVICE_VOX_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "audiodevice.h"
#include "VorbisDriver.h"

//! 音乐
class ATOM_AudioDevice_vox// : public ATOM_AudioDevice
{
public:
	//! 构造函数
	ATOM_AudioDevice_vox();

	//! 析构函数
	virtual ~ATOM_AudioDevice_vox();

	//! 初始化
	virtual bool initialize(const char* filename);

	//! 销毁
	virtual void deInitialize();

	//! 更新
	virtual void update();

	//! 播放
	virtual void play(int id);

	//! 停止所有
	virtual void stop();

	//! 是否在播放
	virtual bool isPlaying() const;

	//! 打开关闭
	virtual void setEnable(bool enable);

	//! 是否打开
	virtual bool getEnable() const;

	//! 设置音量
	virtual void setVolume(float volume);

	//! 取得音量
	virtual float getVolume() const;

private:
	ATOM_MAP<int, ATOM_STRING> _fileNames;
	ATOM_LIST<ATOM_VorbisDriver*> _drivers;
	bool _enable;
	float _volume;
};

#endif	// ATOM_AUDIO_AUDIODEVICE_VOX_H_
