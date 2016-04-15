#ifndef ATOM_AUDIO_AUDIODEVICE_VOX_H_
#define ATOM_AUDIO_AUDIODEVICE_VOX_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "audiodevice.h"
#include "VorbisDriver.h"

//! ����
class ATOM_AudioDevice_vox// : public ATOM_AudioDevice
{
public:
	//! ���캯��
	ATOM_AudioDevice_vox();

	//! ��������
	virtual ~ATOM_AudioDevice_vox();

	//! ��ʼ��
	virtual bool initialize(const char* filename);

	//! ����
	virtual void deInitialize();

	//! ����
	virtual void update();

	//! ����
	virtual void play(int id);

	//! ֹͣ����
	virtual void stop();

	//! �Ƿ��ڲ���
	virtual bool isPlaying() const;

	//! �򿪹ر�
	virtual void setEnable(bool enable);

	//! �Ƿ��
	virtual bool getEnable() const;

	//! ��������
	virtual void setVolume(float volume);

	//! ȡ������
	virtual float getVolume() const;

private:
	ATOM_MAP<int, ATOM_STRING> _fileNames;
	ATOM_LIST<ATOM_VorbisDriver*> _drivers;
	bool _enable;
	float _volume;
};

#endif	// ATOM_AUDIO_AUDIODEVICE_VOX_H_
