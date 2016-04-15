#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <ctype.h>
#include <vector>
#include "ATOM_dbghlp.h"
#include "download.h"
#include "downloadcontext.h"

#define USE_WINHTTP
//#undef USE_WINHTTP

#ifdef USE_WINHTTP
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#endif

#ifndef INTERNET_CONNECTION_OFFLINE
#define INTERNET_CONNECTION_OFFLINE         0x20
#endif

#ifndef INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY
#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4
#endif

static ATOM_DownloadParams _downloadParams = { 0, 0 };
static std::vector<ATOM_DownloadContext*> _allDownloads;

struct BandwidthParam
{
	unsigned long bufferSize;
	unsigned long interval;
};

static BandwidthParam measureBandWidthParam (unsigned long bandwidthLimit)
{
	BandwidthParam param;

	if (bandwidthLimit == 0)
	{
		param.bufferSize = 16 * 1024;
		param.interval = 10;
	}
	else
	{
		if (bandwidthLimit <= 32 * 1024)
		{
			param.bufferSize = 1 * 1024;
		}
		else if (bandwidthLimit <= 64 * 1024)
		{
			param.bufferSize = 2 * 1024;
		}
		else if (bandwidthLimit <= 128 * 1024)
		{
			param.bufferSize = 4 * 1024;
		}
		else if (bandwidthLimit <= 256 * 1024)
		{
			param.bufferSize = 8 * 1024;
		}
		else
		{
			param.bufferSize = 16 * 1024;
		}

		param.interval = ((float)param.bufferSize / (float)bandwidthLimit) * 1000;

		if (param.interval == 0)
		{
			param.interval = 1;
		}
	}

	return param;
}

ATOM_NET_API void ATOM_CALL ATOM_DownloadConfig (const ATOM_DownloadParams &params)
{
	_downloadParams = params;
}

static unsigned __stdcall DownloadThreadMain (void *arg)
{
	ATOM_DownloadContext *context = (ATOM_DownloadContext *)arg;
	unsigned tick = ::GetTickCount();

	context->downloadStatus = ATOM_NET_DOWNLOAD_INPROGRESS;
	context->connectTime = 0xFFFFFFFF;
	context->downloadTime = 0xFFFFFFFF;
	context->averageSpeed = 0;

#ifndef USE_WINHTTP
	DWORD inetFlags;
	::InternetGetConnectedState (&inetFlags, 0);
	if ((inetFlags & INTERNET_CONNECTION_OFFLINE) != 0)
	{
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}

	DWORD access = ((inetFlags & INTERNET_CONNECTION_PROXY) == 0) ? INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY : INTERNET_OPEN_TYPE_PRECONFIG;
	HINTERNET hInternet = ::InternetOpenA ("atom3d", access, NULL, NULL, 0);
#else
	HINTERNET hInternet = ::WinHttpOpen (L"atom3d", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#endif
	if (hInternet == NULL)
	{
		ATOM_LOGGER::error ("Open internet connection failed\n");
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}

	const char *strHead = "Accept: */*\r\n\r\n";

#ifndef USE_WINHTTP
	HINTERNET hURL = ::InternetOpenUrlA (hInternet, context->URL, strHead, strlen(strHead), INTERNET_FLAG_NO_UI|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD|INTERNET_FLAG_RESYNCHRONIZE|INTERNET_FLAG_DONT_CACHE, 0);
	if (hURL == NULL)
	{
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		::InternetCloseHandle (hInternet);
		return 0;
	}

	DWORD bufferLength = sizeof(DWORD);

	DWORD dwStatus = 0;
	if (!HttpQueryInfo(hURL, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &bufferLength, NULL) || (dwStatus != HTTP_STATUS_OK && dwStatus != HTTP_STATUS_REDIRECT))
	{
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		::InternetCloseHandle (hURL);
		::InternetCloseHandle (hInternet);
		return 0;
	}

	DWORD size;
	bufferLength = sizeof(DWORD);
	if (!::HttpQueryInfoA (hURL, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, (void*)&size, &bufferLength, NULL))
	{
		size = 0;
	}
#else
	char buffer[256] = "";
	wchar_t wRequest[256];
	HINTERNET hConnect = 0;
	const char *request = 0;
	if (!strnicmp (context->URL, "http://", 7))
	{
		const char *start = context->URL + 7;
		const char *end = strchr(start, '/');
		if (!end)
		{
			strcpy (buffer, start);
			request = "/";
		}
		else
		{
			strncpy (buffer, start, end - start);
			buffer[end-start] = '\0';
			request = end;
		}
		INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
		char *portPos = strrchr(buffer, ':');
		if (portPos)
		{
			port = atoi(portPos + 1);
			*portPos = '\0';
		}
		::MultiByteToWideChar (CP_ACP, 0, buffer, -1, wRequest, 256);
		hConnect = ::WinHttpConnect(hInternet, wRequest, port, 0);
	}
	if (!hConnect)
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Connect to server %s failed with error code %d(0x%08X)\n", buffer, err, err);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}

	::MultiByteToWideChar (CP_ACP, 0, request, -1, wRequest, 256);
	HINTERNET hRequest = ::WinHttpOpenRequest (hConnect, L"GET", wRequest, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	if (!hRequest)
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Open request failed with error code %d(0x%08X)\n", err, err);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}
	DWORD dwFlags;
	DWORD dwBufLen = sizeof(dwFlags);
	::WinHttpQueryOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBufLen);
	dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
	dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
	dwFlags |= SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
	::WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

	if (!::WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Send request failed with error code %d(0x%08X)\n", err, err);
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}
	if (!::WinHttpReceiveResponse (hRequest, NULL))
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Receive response failed with error code %d(0x%08X)\n", err, err);
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}

	DWORD dwStatus = 0;
	DWORD bufferLength = sizeof(DWORD);
	if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatus, &bufferLength, WINHTTP_NO_HEADER_INDEX) || (dwStatus != HTTP_STATUS_OK && dwStatus != HTTP_STATUS_REDIRECT))
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Query status code failed with error code %d(0x%08X)\n", err, err);
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}

	DWORD size = 0;
	if (!::WinHttpQueryHeaders (hRequest, WINHTTP_QUERY_CONTENT_LENGTH|WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &size, &bufferLength, WINHTTP_NO_HEADER_INDEX))
	{
		DWORD err = ::GetLastError ();
		ATOM_LOGGER::error ("Query content length failed with error code %d(0x%08X)\n", err, err);
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
		return 0;
	}
#endif

	context->totalSize = size;
	context->currentSize = 0;
	context->checksum = 0;

	FILE *fp = fopen (context->localFileName, "wb");
	if (!fp)
	{
		ATOM_LOGGER::error ("Open file for writing failed\n");
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
#ifndef USE_WINHTTP
		::InternetCloseHandle (hURL);
		::InternetCloseHandle (hInternet);
#else
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
#endif
		return 0;
	}

	BandwidthParam param = measureBandWidthParam (context->bandwidthLimit);
	char *databuffer = (char*)malloc(param.bufferSize);
	if (!databuffer)
	{
		context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
#ifndef USE_WINHTTP
		::InternetCloseHandle (hURL);
		::InternetCloseHandle (hInternet);
#else
		::WinHttpCloseHandle (hRequest);
		::WinHttpCloseHandle (hConnect);
		::WinHttpCloseHandle (hInternet);
#endif
		fclose (fp);
		return 0;
	}

	unsigned tick2 = ::GetTickCount();
	context->connectTime = tick2 - tick;
	unsigned downloadTimeAcc = 0;
	unsigned downloadSizeAcc = 0;

	for (;;)
	{
		if (::WaitForSingleObject (context->cancelEvent, param.interval) == WAIT_OBJECT_0)
		{
			// canceled
			context->downloadStatus = ATOM_NET_DOWNLOAD_CANCELED;
			break;
		}

		DWORD bytesRead = 0;
		DWORD tickReadStart = ::GetTickCount();

#ifdef USE_WINHTTP
		if (!::WinHttpQueryDataAvailable (hRequest, &bytesRead))
		{
			DWORD err = ::GetLastError ();
			ATOM_LOGGER::error ("Query data failed with error code %d(0x%08X)\n", err, err);
			context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
			break;
		}
		if (bytesRead)
		{
			DWORD readSize = bytesRead > param.bufferSize ? param.bufferSize : bytesRead;
			if (!::WinHttpReadData (hRequest, databuffer, readSize, &bytesRead))
			{
				DWORD err = ::GetLastError ();
				ATOM_LOGGER::error ("Read data failed with error code %d(0x%08X)\n", err, err);
				context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
				break;
			}
		}
#else
		if (!::InternetReadFile (hURL, databuffer, param.bufferSize, &bytesRead))
		{
			DWORD error;
			DWORD bufferLength = 256;
			char errBuffer[256];
			::InternetGetLastResponseInfoA (&error, errBuffer, &bufferLength);
			context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
			break;
		}
#endif

		downloadTimeAcc += (::GetTickCount() - tickReadStart);
		downloadSizeAcc += bytesRead;
		if (downloadTimeAcc)
		{
			context->averageSpeed = (float)downloadSizeAcc*1000.f/float(downloadTimeAcc);
		}

		if (bytesRead == 0)
		{
			// EOF
			context->downloadStatus = ATOM_NET_DOWNLOAD_OK;
			break;
		}
		else
		{
#if ATOM3D_COMPILER_MSVC
			::InterlockedExchangeAdd ((volatile LONG*)&context->currentSize, bytesRead);
#else
			::InterlockedExchangeAdd ((LONG*)&context->currentSize, bytesRead);
#endif
			if (bytesRead != fwrite (databuffer, 1, bytesRead, fp))
			{
				ATOM_LOGGER::error ("Write file failed\n");
				context->downloadStatus = ATOM_NET_DOWNLOAD_FAILED;
				break;
			}

			if (context->funcCalcChecksum)
			{
				context->checksum = context->funcCalcChecksum(context->checksum, databuffer, bytesRead);
			}
		}
	}

	fflush (fp);
	fclose (fp);
	free (databuffer);

#ifndef USE_WINHTTP
	::InternetCloseHandle (hURL);
	::InternetCloseHandle (hInternet);
#else
	::WinHttpCloseHandle (hRequest);
	::WinHttpCloseHandle (hConnect);
	::WinHttpCloseHandle (hInternet);
#endif

	context->downloadTime = ::GetTickCount() - tick2;

	return 0;
}

ATOM_NET_API void ATOM_CALL ATOM_SetDownloadCheckSumFunction (ATOM_DownloadContext *context, int (*funcCalcChecksum)(int, const void*, unsigned))
{
	context->funcCalcChecksum = funcCalcChecksum;
}

ATOM_NET_API void ATOM_CALL ATOM_SetDownloadWaitIdleFunction (ATOM_DownloadContext *context, void (*funcWaitIdle)(void*), void *waitIdleUserData)
{
	context->funcWaitIdle = funcWaitIdle;
	context->waitIdleUserData = waitIdleUserData;
}

ATOM_NET_API ATOM_DownloadContext * ATOM_CALL ATOM_NewDownload (const char *URL, unsigned bandwidthLimit, bool compressed, const char *param)
{
	if (!URL)
	{
		return NULL;
	}

	ATOM_DownloadContext *context = new ATOM_DownloadContext;
	memset (context, 0, sizeof(ATOM_DownloadContext));

	char buffer[1024];
	strcpy (buffer, URL);

	// 转为小写(避免linux服务器大小写问题)
	strlwr (buffer);

	// 跳过最前面的空格字符
	char *p = buffer;
	while (isspace (*p))
	{
		p++;
	}

	if (param)
	{
		strcat(p, "?");
		strcat(p, param);
	}

	context->URL = _strdup (p);
	context->bandwidthLimit = bandwidthLimit;
	context->compressed = compressed?1:0;
	context->cancelEvent = ::CreateEventA (NULL, FALSE, FALSE, NULL);

	_allDownloads.push_back (context);

	return context;
}

ATOM_NET_API void ATOM_CALL ATOM_DestroyDownload (ATOM_DownloadContext *context)
{
	if (context)
	{
		free (context->localFileName);
		free (context->URL);
		::CloseHandle (context->cancelEvent);

		for (int i = 0; i < _allDownloads.size(); ++i)
		{
			if (_allDownloads[i] == context)
			{
				_allDownloads.erase (_allDownloads.begin()+i);
				break;
			}
		}

		delete context;
	}
}

ATOM_NET_API bool ATOM_CALL ATOM_StartDownloading (ATOM_DownloadContext *context, const char *localFileName)
{
	if (!context)
	{
		return false;
	}

	if (!context->localFileName)
	{
		context->localFileName = (char*)malloc(MAX_PATH);
	}

	if (localFileName)
	{
		::GetFullPathNameA (localFileName, MAX_PATH, context->localFileName, NULL);
	}
	else
	{
		char tmpDir[MAX_PATH];
		::GetTempPathA (MAX_PATH, tmpDir);
		::GetTempFileNameA (tmpDir, "~ATOMCTL", 0, context->localFileName);
	}

	context->downloadStatus = ATOM_NET_DOWNLOAD_INPROGRESS;
	context->totalSize = 0;
	context->currentSize = 0;
	context->hThread = (HANDLE)::_beginthreadex (NULL, 0, &DownloadThreadMain, context, 0, NULL);

	return true;
}

ATOM_NET_API int ATOM_CALL ATOM_GetDownloadingStatus (ATOM_DownloadContext *context)
{
	return context ? context->downloadStatus : ATOM_NET_DOWNLOAD_OK;
}

ATOM_NET_API int ATOM_CALL ATOM_WaitForDownloading (ATOM_DownloadContext *context, unsigned timeout)
{
	if (!context)
	{
		return ATOM_NET_DOWNLOAD_OK;
	}

	if (context->downloadStatus == ATOM_NET_DOWNLOAD_NOTSTART)
	{
		return ATOM_NET_DOWNLOAD_NOTSTART;
	}

	DWORD wait = ::WaitForSingleObject (context->hThread, timeout);
	if (wait == WAIT_OBJECT_0)
	{
		return context->downloadStatus;
	}
	else
	{
		return ATOM_NET_DOWNLOAD_INPROGRESS;
	}
}

ATOM_NET_API void ATOM_CALL ATOM_CancelDownloading (ATOM_DownloadContext *context)
{
	if (context)
	{
		::SetEvent (context->cancelEvent);
	}
}

ATOM_NET_API const char * ATOM_CALL ATOM_GetDownloadFileName (ATOM_DownloadContext *context)
{
	return context ? context->URL: 0;
}

ATOM_NET_API const char * ATOM_CALL ATOM_GetDownloadedFileName (ATOM_DownloadContext *context)
{
	return context ? context->localFileName : 0;
}

ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadFileLength (ATOM_DownloadContext *context)
{
	return context ? context->totalSize : 0;
}

ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadedFileLength (ATOM_DownloadContext *context)
{
	return context ? context->currentSize : 0;
}

ATOM_NET_API float ATOM_CALL ATOM_GetDownloadPercentage (ATOM_DownloadContext *context)
{
	if (context)
	{
		unsigned long total = ATOM_GetDownloadFileLength (context);
		if (total != 0)
		{
			unsigned long current = ATOM_GetDownloadedFileLength (context);
			return (float)current/(float)total;
		}
	}
	return 0.f;
}

ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadConnectTime (ATOM_DownloadContext *context)
{
	return context ? context->connectTime : 0;
}

ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadTime (ATOM_DownloadContext *context)
{
	return context ? context->downloadTime : 0;
}

ATOM_NET_API int ATOM_CALL ATOM_GetDownloadCheckSum (ATOM_DownloadContext *context)
{
	return context ? context->checksum : 0;
}

ATOM_NET_API float ATOM_CALL ATOM_GetDownloadAverageSpeed (ATOM_DownloadContext *context)
{
	return context ? context->averageSpeed : 0.f;
}

ATOM_NET_API unsigned long ATOM_CALL ATOM_GetNumDownloads (void)
{
	return _allDownloads.size ();
}

ATOM_NET_API ATOM_DownloadContext * ATOM_CALL ATOM_GetDownload (unsigned long index)
{
	return _allDownloads[index];
}

