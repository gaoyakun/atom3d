#include <ATOM_net.h>
#include <ATOM_lzmapkg.h>
#include "httpvfs.h"
#include "physicfs.h"

ATOM_HttpVFS::ATOM_HttpVFS (const char *domain, const char *domain2, const char *cachePath, const char *version, FuncCheckFile funcCheckFile)
	: _domain (domain)
	, _domain2 (domain2?domain2:"")
	, _cachePath (cachePath)
	, _version (version?version:"")
	, _downloadHandler (NULL)
	, _downloaderHandlerUserData (NULL)
	, _handlerInExecuting (false)
	, _compressed (false)
	, _cachedVFS (ATOM_NEW(ATOM_PhysicVFS))
	, _funcCheckFile (funcCheckFile)
{
}

ATOM_HttpVFS::~ATOM_HttpVFS (void)
{
	ATOM_DELETE(_cachedVFS);
}

ATOM_VFS::finddata_t *ATOM_HttpVFS::findFirst (const char *dir)
{
	throw ATOM_Exception("ATOM_HttpVFS::findFirst������");
}

bool ATOM_HttpVFS::findNext (ATOM_VFS::finddata_t *handle)
{
	throw ATOM_Exception("ATOM_HttpVFS::findNext������");
}

bool ATOM_HttpVFS::mkDir (const char *dir)
{
	return _cachedVFS->mkDir(dir);
}

bool ATOM_HttpVFS::isDir (const char *dir)
{
	return _cachedVFS->isDir (dir);
}

bool ATOM_HttpVFS::removeFile (const char *file, bool force)
{
	throw ATOM_Exception("ATOM_HttpVFS::removeFile������");
}

bool ATOM_HttpVFS::doesFileExists (const char *file)
{
	return _cachedVFS->doesFileExists (file);
}

ATOM_VFS *ATOM_HttpVFS::getNativePath (const char *mount_path, char *buffer) const
{
	ATOM_STRING localFileName = _cachePath + mount_path;
	_cachedVFS->identifyPath (localFileName.c_str(), buffer);
	return _cachedVFS;
}

ATOM_VFS::handle ATOM_HttpVFS::vfopen (const char *filename, int mode)
{
	ATOM_STRING localFileName = _cachePath + filename;
	ATOM_VFS::handle h = _cachedVFS->vfopen (localFileName.c_str(), mode);
	if (h != ATOM_VFS::invalid_handle)
	{
		return h;
	}

	if ((mode & ATOM_VFS::write) == 0)
	{
		ATOM_LOGGER::log ("Local file %s not found, try downloading...\n", filename);
		h = _tryopen (_domain.c_str(), filename, mode, true);
		if (h == ATOM_VFS::invalid_handle)
		{
			ATOM_LOGGER::log ("try downloading again...\n", filename);
			h = _tryopen (_domain2.empty()?_domain.c_str():_domain2.c_str(), filename, mode, true);
		}
		return h;
	}

	return h;
}

ATOM_VFS::handle ATOM_HttpVFS::_tryopen (const char *domain, const char *filename, int mode, bool force)
{
	ATOM_VFS::handle h = ATOM_VFS::invalid_handle;

	// �����ļ�У������ı�׼�ļ���
	// �涨,����KEY���ļ�����ʽΪ\dir1\dir2\filename(�����Ƿ�б��,Сд��ĸ)
	// �ڴ˹淶���ļ���
	char id[ATOM_VFS::max_filename_length];
	identifyPath (filename, id);
	strlwr (id);
	for (char *ch = id; *ch; ++ch)
	{
		if (*ch == '/')
		{
			*ch = '\\';
		}
	}

	// ���챾�������ļ���
	ATOM_STRING localFileName = _cachePath + filename;

	// ȷ�����ļ�������Ŀ¼����
	char dir[ATOM_VFS::max_filename_length];
	_cachedVFS->identifyPath (localFileName.c_str(), dir);
	char *s = strrchr (dir, _cachedVFS->getSeperator());
	if (s) 
	{
		*s = '\0';
	}
	if (!_cachedVFS->doesFileExists (dir))
	{
		_cachedVFS->mkDir (dir);
	}
	if (!_cachedVFS->isDir (dir))
	{
		return ATOM_VFS::invalid_handle;
	}

	// ���ý��̺��߳�ID��������ʱ�ļ���,Ԥ����ͬ�߳�д����ͬ�ļ�
	char tid[32];
	char pid[32];
	sprintf (tid, "_%d", ::GetCurrentThreadId ());
	sprintf (pid, "_%d", ::GetCurrentProcessId ());
	ATOM_STRING localTmpFileName = localFileName + pid + tid + ".tmp";

	// �����ļ�
	ATOM_STRING httpFileName = domain;
	if (filename[0] != '/')
		httpFileName += '/';
	httpFileName += filename;

	// �����ļ��汾��
	char strVer[64];
	const char *verParam = nullptr;
	if (!_version.empty() && _funcCheckFile)
	{
		// ��ȡ��ǰ�������ļ��汾��
		int ver = _funcCheckFile (id, 0);
		if (ver)
		{
			// ��_versionΪģ�幹��汾�ַ���
			sprintf (strVer, _version.c_str(), ver);
			verParam = strVer;
		}
	}
	ATOM_DownloadContext *context = ATOM_NewDownload (httpFileName.c_str(),0,false,verParam);
	if (context)
	{
		int downloadResult = ATOM_NET_DOWNLOAD_FAILED;
		DWORD downloadTime = 0;
		DWORD startTick = ::GetTickCount ();
		ATOM_StartDownloading (context, localTmpFileName.c_str());
		if (_downloadHandler && !_handlerInExecuting)
		{
			// ��ֹ����
			_handlerInExecuting = true;

			downloadResult = _downloadHandler (context, _downloaderHandlerUserData);

			_handlerInExecuting = false;
		}
		else
		{
			downloadResult = ATOM_WaitForDownloading (context, INFINITE);
		}
		downloadTime = ::GetTickCount() - startTick;
		ATOM_LOGGER::debug ("<%s> downloaded in %dms, connect=%dms, download=%dms\n", httpFileName.c_str(), downloadTime, ATOM_GetDownloadConnectTime(context), ATOM_GetDownloadTime(context));

		if (downloadResult == ATOM_NET_DOWNLOAD_OK)
		{
			// �ļ����سɹ�
			if (_compressed)
			{
				// �����ѹ���ļ���Ҫ��ѹ��
				ATOM_LzmaFilePackage pack;

				bool decompressResult;

				{
					// ��ѹ���ļ�����,�������߳�д����ͬ�ļ�����ļ��ƻ�
					ATOM_Mutex::ScopeMutex lock(_ioLock);
					decompressResult = pack.decompress (localTmpFileName.c_str(), dir);
				}

				if (decompressResult)
				{
					// �ɹ���ѹ��
					DeleteFileA (localTmpFileName.c_str());
					ATOM_DestroyDownload (context);
					if (_funcCheckFile)
					{
						// ��ҪУ���ļ��Ϸ���

						// ��ȡ�ļ�CRCУ����
						int testMode = ATOM_VFS::binary|ATOM_VFS::read;
						h = _cachedVFS->vfopen (localFileName.c_str(), testMode);
						int checksum = ATOM_VFS::vfsCRC32 (h);

						// ����У��ص�����
						if (checksum != _funcCheckFile (id, checksum))
						{
							// У��ʧ��,�ļ��Ƿ�
							if (force)
							{
								// Ҫ��ʹУ��ʧ��Ҳ���ļ�
								ATOM_LOGGER::error ("File version mismatch: %s(0x%08X)\n", id, checksum);
								if (mode != testMode)
								{
									_cachedVFS->vfclose(h);
									h = _cachedVFS->vfopen (localFileName.c_str(), mode);
								}
							}
							else
							{
								// У��ʧ�ܲ����ļ�
								_cachedVFS->vfclose (h);
								h = ATOM_VFS::invalid_handle;
							}
						}
						else if (mode != testMode)
						{
							// У��ɹ�,���ԭ�����ǰ��ն�����ģʽ���������°���Ҫ���ģʽ��
							_cachedVFS->vfclose(h);
							h = _cachedVFS->vfopen (localFileName.c_str(), mode);
						}
					}
					else
					{
						// ����ҪУ����ֱ�Ӵ�
						h = _cachedVFS->vfopen (localFileName.c_str(), mode);
					}

					return h;
				}
				else
				{
					// ��ѹ��ʧ��
					ATOM_LOGGER::error ("Error decompressing downloaded file <%s>\n", localTmpFileName.c_str());
				}
			}
			else
			{
				// ��ѹ���ļ�
				ATOM_DestroyDownload (context);

				// ��ʱ�ļ�������Ϊʵ���ļ�
				if (!::MoveFileA (localTmpFileName.c_str(), localFileName.c_str()))
				{
					// �п��������߳��Ѿ�����������ļ�����MoveFileʧ��,���ļ���Ok��,���Լ������Դ򿪸��ļ����Ǵ��󷵻�
					::DeleteFileA (localTmpFileName.c_str());
					ATOM_LOGGER::error ("Error renaming downloaded file <%s>\n", localTmpFileName.c_str());
				}
				if (_funcCheckFile)
				{
					// ��ҪУ��
					int testMode = ATOM_VFS::binary|ATOM_VFS::read;
					h = _cachedVFS->vfopen (localFileName.c_str(), testMode);
					if (h != ATOM_VFS::invalid_handle)
					{
						int checksum = ATOM_VFS::vfsCRC32 (h);
						if (checksum != _funcCheckFile (id, checksum))
						{
							if (force)
							{
								ATOM_LOGGER::error ("File version mismatch: %s(0x%08X)\n", id, checksum);
								if (mode != testMode)
								{
									_cachedVFS->vfclose(h);
									h = _cachedVFS->vfopen (localFileName.c_str(), mode);
								}
							}
							else
							{
								_cachedVFS->vfclose (h);
								h = ATOM_VFS::invalid_handle;
							}
						}
					}
				}
				else
				{
					h = _cachedVFS->vfopen (localFileName.c_str(), mode);
				}
				return h;
			}
		}
		else
		{
			// �����ļ�ʧ��
			ATOM_LOGGER::error ("Error downloading file <%s> from <%s>\n", localTmpFileName.c_str(), httpFileName.c_str());
		}

		DeleteFileA (localTmpFileName.c_str());
		ATOM_DestroyDownload (context);
	}

	return h;
}

unsigned ATOM_HttpVFS::vfread (ATOM_VFS::handle f, void *buffer, unsigned size)
{
	return _cachedVFS->vfread (f, buffer, size);
}

unsigned ATOM_HttpVFS::vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size)
{
	return _cachedVFS->vfwrite (f, buffer, size);
}

long ATOM_HttpVFS::vfseek (ATOM_VFS::handle f, long offset, int origin)
{
	return _cachedVFS->vfseek (f, offset, origin);
}

long ATOM_HttpVFS::vftell (ATOM_VFS::handle f)
{
	return _cachedVFS->vftell (f);
}

long ATOM_HttpVFS::vfsize (ATOM_VFS::handle f)
{
	return _cachedVFS->vfsize (f);
}

void ATOM_HttpVFS::vfclose (ATOM_VFS::handle f)
{
	return _cachedVFS->vfclose (f);
}

bool ATOM_HttpVFS::vfeof (ATOM_VFS::handle f)
{
	return _cachedVFS->vfeof (f);
}

void ATOM_HttpVFS::setDownloadHandler (FuncDownloadHandler handler, void *userData)
{
	_downloadHandler = handler;
	_downloaderHandlerUserData = userData;
}

void ATOM_HttpVFS::enableCompressMode (bool enable)
{
	_compressed = enable;
}

