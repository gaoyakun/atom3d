#include "audiodevice.h"
#include "audiodevice_al.h"
#include <ATOM_kernel.h>
#include <process.h>

ATOM_AUDIO_API ATOM_AudioDevice* ATOM_CreateAudioDevice(const char* name)
{
	ATOM_STACK_TRACE(ATOM_CreateAudioDevice);
	return ATOM_NEW(ATOM_AudioDevice_al);
}

ATOM_AUDIO_API void ATOM_DestroyAudioDevice(ATOM_AudioDevice* device)
{
	ATOM_STACK_TRACE(ATOM_DestroyAudioDevice);
	if(device)
	{
		ATOM_DELETE(device);
		device = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////


ATOM_AudioDeviceWrapper::ATOM_AudioDeviceWrapper(const char* deviceName, const char* configFilename)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::ATOM_AudioDeviceWrapper);

	_deviceName = deviceName;
	_configFilename = configFilename;
	InitializeCriticalSection(&_commandLock);
	_stop = false;
	_threadFinished = true;
	_soundVolumn = 1.f;
	_bgmVolumn = 1.f;
	_enableSound = true;
	_enableBgm = true;

	DWORD hThreadId;
	CreateThread(NULL, 0, AudioWorkThread, (LPVOID)this, 0, &hThreadId);
}

ATOM_AudioDeviceWrapper::~ATOM_AudioDeviceWrapper()
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::~ATOM_AudioDeviceWrapper);

	_stop = true;
	while(!_threadFinished)
	{
		Sleep(20);
	}
	DeleteCriticalSection(&_commandLock); 
}

void ATOM_AudioDeviceWrapper::playSound(int id)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::playSound);

	postCommand(AUDIOCOMMAND_PLAYSOUND, id);
}

void ATOM_AudioDeviceWrapper::playBgm(int id)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::playBgm);

	postCommand(AUDIOCOMMAND_PLAYBGM, id);
}

void ATOM_AudioDeviceWrapper::stopSound()
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::stopSound);

	postCommand(AUDIOCOMMAND_STOPSOUND, 0);
}

void ATOM_AudioDeviceWrapper::stopBgm()
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::stopBgm);

	postCommand(AUDIOCOMMAND_STOPBGM, 0);
}

void ATOM_AudioDeviceWrapper::setSoundVolumn(float vol)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::setSoundVolumn);

	_soundVolumn = vol;
	postCommand(AUDIOCOMMAND_SOUNDVOLUMN, vol);
}

float ATOM_AudioDeviceWrapper::getSoundVolumn() const
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::getSoundVolumn);

	return _soundVolumn;
}

void ATOM_AudioDeviceWrapper::setBgmVolumn(float vol)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::setBgmVolumn);

	_bgmVolumn = vol;
	postCommand(AUDIOCOMMAND_BGMVOLUMN, vol);
}

float ATOM_AudioDeviceWrapper::getBgmVolumn() const
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::getBgmVolumn);

	return _bgmVolumn;
}

void ATOM_AudioDeviceWrapper::enableSound(bool enable)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::enableSound);

	_enableSound = enable;
	postCommand(AUDIOCOMMAND_ENABLESOUND, enable);
}

bool ATOM_AudioDeviceWrapper::getEnableSound() const
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::getEnableSound);

	return _enableSound;
}

void ATOM_AudioDeviceWrapper::enableBgm(bool enable)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::enableBgm);

	_enableBgm = enable;
	postCommand(AUDIOCOMMAND_ENABLEBGM, enable);
}

bool ATOM_AudioDeviceWrapper::getEnableBgm() const
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::getEnableBgm);

	return _enableBgm;
}

void ATOM_AudioDeviceWrapper::postCommand(ATOM_AUDIOCOMMAND cmd, int value)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::postCommand);

	ATOM_AudioCommand command;
	command._type = cmd;
	command._value = value;
	EnterCriticalSection(&_commandLock);
	_commands.push(command);
	LeaveCriticalSection(&_commandLock);
}

DWORD WINAPI ATOM_AudioDeviceWrapper::AudioWorkThread(LPVOID lpThreadParameter)
{
	ATOM_STACK_TRACE(ATOM_AudioDeviceWrapper::AudioWorkThread);

	ATOM_AudioDeviceWrapper* deviceWrapper = (ATOM_AudioDeviceWrapper*)lpThreadParameter;
	ATOM_AudioDevice* device;
	ATOM_LIST<ATOM_Audio*> soundList;
	ATOM_Audio* bgm = NULL;
	ATOM_MAP<int, ATOM_STRING> fileNames;

	ATOM_Audio* audio;
	ATOM_LIST<ATOM_Audio*>::iterator itSound;
	ATOM_MAP<int, ATOM_STRING>::iterator itFilename;

	// 创建device
	deviceWrapper->_threadFinished = false;
	device = ATOM_CreateAudioDevice(deviceWrapper->_deviceName.c_str());
	if(NULL == device)
	{
		ATOM_LOGGER::error("[ATOM_AudioDeviceWrapper::AudioWorkThread] can't create device %s.\n", deviceWrapper->_deviceName.c_str());
		deviceWrapper->_threadFinished = true;
		return 0;
	}
	if(false == device->initialize("DirectSound3D"))
	{
		ATOM_DestroyAudioDevice(device);
		ATOM_LOGGER::error("[ATOM_AudioDeviceWrapper::AudioWorkThread] create %s failed.\n", "DirectSound3D");
		deviceWrapper->_threadFinished = true;
		return 0;
	}

	// 读取配置文件
	ATOM_TiXmlDocument doc;
	ATOM_AutoFile file(deviceWrapper->_configFilename.c_str(), ATOM_VFS::text|ATOM_VFS::read);
	if(file == NULL)
	{	
		ATOM_LOGGER::error("[ATOM_AudioDeviceWrapper::AudioWorkThread]can't load %s.\n", deviceWrapper->_configFilename.c_str());
	}
	else
	{
		int len = file->size();
		char* buffer = ATOM_NEW_ARRAY(char, len+1);
		assert(buffer);
		int readSize = file->read(buffer, len);
		buffer[readSize] = 0;
		doc.Parse(buffer);
		ATOM_DELETE_ARRAY(buffer);
		if (doc.Error ())
		{
			ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, lpThreadParameter, doc.ErrorDesc());
		}
		else
		{
			ATOM_TiXmlElement* root = doc.RootElement();
			if(NULL == root)
			{
				ATOM_LOGGER::error("[ATOM_AudioDeviceWrapper::AudioWorkThread] %s error.\n", deviceWrapper->_configFilename.c_str());
			}
			else
			{
				int id;
				const char* fileName;
				fileNames.clear();
				ATOM_TiXmlElement* elem = root->FirstChildElement("elem");
				for(; elem; elem = elem->NextSiblingElement("elem"))
				{
					if(NULL == elem->Attribute("id", &id))
					{
						continue;
					}

					fileName = elem->Attribute("file");
					if(NULL == fileName)
					{
						continue;
					}

					fileNames[id] = fileName;
				}
			}
		}
	}


	// 更新
	while(1)
	{
		if(deviceWrapper->_stop)
		{
			break;
		}
		Sleep(20);
		
		// 执行指令
		while(1)
		{
			EnterCriticalSection(&deviceWrapper->_commandLock);
			if(deviceWrapper->_commands.empty())
			{
				LeaveCriticalSection(&deviceWrapper->_commandLock);
				break;
			}
			ATOM_AudioCommand command = deviceWrapper->_commands.front();
			deviceWrapper->_commands.pop();
			LeaveCriticalSection(&deviceWrapper->_commandLock);

			switch(command._type)
			{
			case AUDIOCOMMAND_PLAYSOUND:
				if(deviceWrapper->_enableSound)
				{
					ATOM_MAP<int, ATOM_STRING>::iterator itFilename = fileNames.find(command._value);
					if(itFilename != fileNames.end())
					{
						audio = device->createAudio();
						audio->setFilename(itFilename->second.c_str());
						audio->setLoop(false);
						audio->setVolumn(deviceWrapper->_soundVolumn);
						if(false == audio->play())
						{
							device->destroyAudio(audio);
						}
						else
						{
							soundList.push_back(audio);
						}
					}
				}
				break;

			case AUDIOCOMMAND_PLAYBGM:
				if(deviceWrapper->_enableBgm)
				{
					ATOM_MAP<int, ATOM_STRING>::iterator itFilename = fileNames.find(command._value);
					if(itFilename != fileNames.end())
					{						
						if(bgm)
						{
							//ATOM_LOGGER::information("stop bgm first.\n");
							bgm->stop();
							device->destroyAudio(bgm);
							bgm = NULL;
						}
						//ATOM_LOGGER::information("play bgm %s.\n", itFilename->second.c_str());
						bgm = device->createAudio();
						bgm->setFilename(itFilename->second.c_str());
						bgm->setLoop(true);
						bgm->setVolumn(deviceWrapper->_bgmVolumn);
						bgm->play();
					}
				}
				break;

			case AUDIOCOMMAND_STOPSOUND:
				{
					for(itSound = soundList.begin(); itSound != soundList.end(); ++itSound)
					{
						device->destroyAudio(*itSound);
					}
					soundList.clear();
				}
				break;

			case AUDIOCOMMAND_STOPBGM:
				if(bgm)
				{
					//ATOM_LOGGER::information("stop bgm.\n");
					bgm->stop();
					device->destroyAudio(bgm);
					bgm = NULL;
				}
				break;

			case AUDIOCOMMAND_SOUNDVOLUMN:
				break;

			case AUDIOCOMMAND_BGMVOLUMN:
				if(bgm)
				{
					bgm->setVolumn(deviceWrapper->_bgmVolumn);
				}
				break;

			case AUDIOCOMMAND_ENABLESOUND:
				break;

			case AUDIOCOMMAND_ENABLEBGM:
				break;
			}
		}				
		
		// 更新音效
		for(itSound = soundList.begin(); itSound != soundList.end();)
		{
			audio = *itSound;
			audio->update();
			if(AUDIOSTATE_UNLOAD == audio->getState())
			{
				device->destroyAudio(audio);
				itSound = soundList.erase(itSound);
			}
			else
			{
				++itSound;
			}
		}

		if(bgm)
		{
			bgm->update();	
		}
	}

	// 销毁
	for(itSound = soundList.begin(); itSound != soundList.end(); ++itSound)
	{
		ATOM_DELETE(*itSound);
	}
	soundList.clear();

	if(bgm)
	{
		ATOM_DELETE(bgm);
		bgm = NULL;
	}
	device->deInitialize();
	ATOM_DestroyAudioDevice(device);
	deviceWrapper->_threadFinished = true;

	return 0;
}

