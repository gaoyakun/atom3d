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
	throw ATOM_Exception("ATOM_HttpVFS::findFirst不可用");
}

bool ATOM_HttpVFS::findNext (ATOM_VFS::finddata_t *handle)
{
	throw ATOM_Exception("ATOM_HttpVFS::findNext不可用");
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
	throw ATOM_Exception("ATOM_HttpVFS::removeFile不可用");
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

	// 构造文件校验所需的标准文件名
	// 规定,生成KEY的文件名格式为\dir1\dir2\filename(必须是反斜杠,小写字母)
	// 在此规范化文件名
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

	// 构造本地物理文件名
	ATOM_STRING localFileName = _cachePath + filename;

	// 确保该文件的所在目录存在
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

	// 利用进程和线程ID来构造临时文件名,预防不同线程写入相同文件
	char tid[32];
	char pid[32];
	sprintf (tid, "_%d", ::GetCurrentThreadId ());
	sprintf (pid, "_%d", ::GetCurrentProcessId ());
	ATOM_STRING localTmpFileName = localFileName + pid + tid + ".tmp";

	// 下载文件
	ATOM_STRING httpFileName = domain;
	if (filename[0] != '/')
		httpFileName += '/';
	httpFileName += filename;

	// 构造文件版本号
	char strVer[64];
	const char *verParam = nullptr;
	if (!_version.empty() && _funcCheckFile)
	{
		// 获取当前服务器文件版本号
		int ver = _funcCheckFile (id, 0);
		if (ver)
		{
			// 以_version为模板构造版本字符串
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
			// 防止重入
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
			// 文件下载成功
			if (_compressed)
			{
				// 如果是压缩文件需要解压缩
				ATOM_LzmaFilePackage pack;

				bool decompressResult;

				{
					// 解压缩文件加锁,以免多个线程写入相同文件造成文件破坏
					ATOM_Mutex::ScopeMutex lock(_ioLock);
					decompressResult = pack.decompress (localTmpFileName.c_str(), dir);
				}

				if (decompressResult)
				{
					// 成功解压缩
					DeleteFileA (localTmpFileName.c_str());
					ATOM_DestroyDownload (context);
					if (_funcCheckFile)
					{
						// 需要校验文件合法性

						// 获取文件CRC校验码
						int testMode = ATOM_VFS::binary|ATOM_VFS::read;
						h = _cachedVFS->vfopen (localFileName.c_str(), testMode);
						int checksum = ATOM_VFS::vfsCRC32 (h);

						// 调用校验回调函数
						if (checksum != _funcCheckFile (id, checksum))
						{
							// 校验失败,文件非法
							if (force)
							{
								// 要求即使校验失败也打开文件
								ATOM_LOGGER::error ("File version mismatch: %s(0x%08X)\n", id, checksum);
								if (mode != testMode)
								{
									_cachedVFS->vfclose(h);
									h = _cachedVFS->vfopen (localFileName.c_str(), mode);
								}
							}
							else
							{
								// 校验失败不打开文件
								_cachedVFS->vfclose (h);
								h = ATOM_VFS::invalid_handle;
							}
						}
						else if (mode != testMode)
						{
							// 校验成功,如果原本不是按照二进制模式打开则需重新按照要求的模式打开
							_cachedVFS->vfclose(h);
							h = _cachedVFS->vfopen (localFileName.c_str(), mode);
						}
					}
					else
					{
						// 不需要校验则直接打开
						h = _cachedVFS->vfopen (localFileName.c_str(), mode);
					}

					return h;
				}
				else
				{
					// 解压缩失败
					ATOM_LOGGER::error ("Error decompressing downloaded file <%s>\n", localTmpFileName.c_str());
				}
			}
			else
			{
				// 非压缩文件
				ATOM_DestroyDownload (context);

				// 临时文件重命名为实际文件
				if (!::MoveFileA (localTmpFileName.c_str(), localFileName.c_str()))
				{
					// 有可能其它线程已经下载了这个文件导致MoveFile失败,但文件是Ok的,所以继续尝试打开该文件而非错误返回
					::DeleteFileA (localTmpFileName.c_str());
					ATOM_LOGGER::error ("Error renaming downloaded file <%s>\n", localTmpFileName.c_str());
				}
				if (_funcCheckFile)
				{
					// 需要校验
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
			// 下载文件失败
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

