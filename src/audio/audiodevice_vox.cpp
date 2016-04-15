#include "audiodevice_vox.h"
#include <ATOM_kernel.h>

// 构造函数
ATOM_AudioDevice_vox::ATOM_AudioDevice_vox()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::ATOM_AudioDevice_vox);

	_enable = TRUE;
	_volume = 1.f;
}

// 析构函数
ATOM_AudioDevice_vox::~ATOM_AudioDevice_vox()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::~ATOM_AudioDevice_vox);

	deInitialize();
}

//! 初始化
bool ATOM_AudioDevice_vox::initialize(const char* filename)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::initialize);

	ATOM_TiXmlDocument doc;
	ATOM_AutoFile file(filename, ATOM_VFS::text|ATOM_VFS::read);
	if(file == NULL)
	{	
		return false;
	}
	int len = file->size();
	char* buffer = ATOM_NEW_ARRAY(char, len+1);
	assert(buffer);
	int readSize = file->read(buffer, len);
	buffer[readSize] = 0;
	doc.Parse(buffer);
	ATOM_DELETE_ARRAY(buffer);
	if (doc.Error ())
	{
		ATOM_LOGGER::error ("%s(%s) %s\n", __FUNCTION__, filename, doc.ErrorDesc());
		return false;
	}

	ATOM_TiXmlElement* root = doc.RootElement();
	if(NULL == root)
	{
		return false;
	}

	int id;
	const char* fileName;
	_fileNames.clear();
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

		_fileNames[id] = fileName;
	}

	return true;
}

//! 销毁
void ATOM_AudioDevice_vox::deInitialize()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::deInitialize);

	stop();
}

// 播放
void ATOM_AudioDevice_vox::play(int id)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::play);

	if(INVALID_AUDIOID == id)
	{
		return;
	}

	if(FALSE == _enable)
	{
		return;
	}

	ATOM_MAP<int, ATOM_STRING>::iterator iter = _fileNames.find(id);
	if(iter == _fileNames.end())
	{
		return;
	}

	char buf[MAX_PATH];
	ATOM_GetNativePathName(iter->second.c_str(), buf);

	ATOM_VorbisDriver* driver = ATOM_NEW(ATOM_VorbisDriver);
	driver->Load(buf);	
	driver->SetLoop(1);
	driver->SetVolume(_volume);
	driver->Play();
	_drivers.push_back(driver);
}

// 关闭所有
void ATOM_AudioDevice_vox::stop()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::stop);

	for(ATOM_LIST<ATOM_VorbisDriver*>::iterator iter=_drivers.begin(); iter != _drivers.end(); ++iter)
	{
		(*iter)->Release();
		ATOM_DELETE(*iter);
	}
	_drivers.clear();
}

// 是否在播放
bool ATOM_AudioDevice_vox::isPlaying() const
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::isPlaying);

	for(ATOM_LIST<ATOM_VorbisDriver*>::const_iterator iter=_drivers.begin(); iter != _drivers.end(); ++iter)
	{
		const ATOM_VorbisDriver* driver = (*iter);
		if(driver->GetLoop() > 0)
		{
			return true;
		}
	}
	return false;
}

// 打开关闭
void ATOM_AudioDevice_vox::setEnable(bool enable)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::setEnable);

	_enable = enable;
	if(false == enable)
	{
		stop();
	}
}

// 是否打开
bool ATOM_AudioDevice_vox::getEnable() const
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::getEnable);

	return _enable;
}

// 设置音量
void ATOM_AudioDevice_vox::setVolume(float volume)
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::setVolumn);

	_volume = volume;
}

// 取得音量
float ATOM_AudioDevice_vox::getVolume() const
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::getVolume);

	return _volume;
}

// 更新
void ATOM_AudioDevice_vox::update()
{
	ATOM_STACK_TRACE(ATOM_AudioDevice_vox::update);

	for(ATOM_LIST<ATOM_VorbisDriver*>::iterator iter=_drivers.begin(); iter != _drivers.end();)
	{
		ATOM_VorbisDriver* driver = *iter;
		if(driver->GetLoop() <= 0)
		{
			driver->Release();
			ATOM_DELETE(driver);
			iter = _drivers.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}
