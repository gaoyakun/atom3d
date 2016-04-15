#include "StdAfx.h"
#include <wininet.h>
#include "send_error.h"

#ifndef INTERNET_CONNECTION_OFFLINE
#define INTERNET_CONNECTION_OFFLINE         0x20
#endif

#ifndef INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY
#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4
#endif

static char ftp_url[256] = "";
static int ftp_port = 21;
static char ftp_user[256] = "anonymous";
static char ftp_pwd[256] = "";

static char smtp_server[256] = "";
static int smtp_port = 25;
static char smtp_user[256] = "";
static char smtp_pwd[256] = "";

bool ATOM_ErrorSender::setFtpSite (const char *url, int port, const char *username, const char *password)
{
	if (!ftp_url)
	{
		return false;
	}

	strcpy (ftp_url, url);
	ftp_port = port;

	if (username)
	{
		strcpy (ftp_user, username);
		strcpy (ftp_pwd, password ? password : "");
	}
	else
	{
		strcpy (ftp_user, "anonymous");
		strcpy (ftp_pwd, password ? password : "pass@word");
	}

	return true;
}

static bool _sendErrorFile (const char *fileName, const char *destDir, bool text)
{
	if (!ftp_url[0] || !fileName || !fileName[0] || !destDir || !destDir[0])
	{
		return false;
	}

	DWORD inetFlags;
	::InternetGetConnectedState (&inetFlags, 0);
	if ((inetFlags & INTERNET_CONNECTION_OFFLINE) != 0)
	{
		return false;
	}

	DWORD access = ((inetFlags & INTERNET_CONNECTION_PROXY) == 0) ? INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY : INTERNET_OPEN_TYPE_PRECONFIG;
	HINTERNET hInternet = ::InternetOpenA ("IEXPLORER.EXE", access, NULL, NULL, 0);
	if (!hInternet)
	{
		return false;
	}

	HINTERNET hConnect = ::InternetConnectA (hInternet, ftp_url, ftp_port, ftp_user, ftp_pwd, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (!hConnect)
	{
		::InternetCloseHandle (hInternet);
		return false;
	}

	if (!::FtpSetCurrentDirectoryA (hConnect, destDir))
	{
		if (!::FtpCreateDirectoryA (hConnect, destDir))
		{
			::InternetCloseHandle (hConnect);
			::InternetCloseHandle (hInternet);
			return false;
		}
		if (!::FtpSetCurrentDirectoryA (hConnect, destDir))
		{
			return false;
		}
	}

	if (!::FtpSetCurrentDirectoryA (hConnect, "/"))
	{
		return false;
	}

	char localFileName[MAX_PATH];
	char *Name = 0;
	if (!::GetFullPathNameA (fileName, MAX_PATH, localFileName, &Name))
	{
		::InternetCloseHandle (hConnect);
		::InternetCloseHandle (hInternet);
		return false;
	}

	char remoteFileName[256];
	strcpy (remoteFileName, destDir);
	if (remoteFileName[strlen(remoteFileName)-1] != '/')
	{
		strcat (remoteFileName, "/");
	}
	strcat (remoteFileName, Name);

	BOOL ret = ::FtpPutFileA (hConnect, localFileName, remoteFileName, text ? FTP_TRANSFER_TYPE_ASCII : FTP_TRANSFER_TYPE_BINARY, 0);
	::InternetCloseHandle (hConnect);
	::InternetCloseHandle (hInternet);

	return ret == TRUE;
}

bool _isDirectory (const char *dir)
{
	DWORD attributes = ::GetFileAttributesA(dir);
	return attributes != 0xFFFFFFFF && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool _makeDirectory (char *dd)
{
	if (_isDirectory (dd))
	{
		return true;
	}

	char *p = strrchr (dd, '\\');
	if (!p)
	{
		return false;
	}
	
	*p = '\0';
	
	if (!_makeDirectory (dd))
	{
		return false;
	}

	*p = '\\';

	return ::CreateDirectoryA (dd, NULL) == TRUE;
}

void ATOM_ErrorSender::setMailServer (const char *smtp_svr, int port, const char *username, const char *password)
{
	strcpy (smtp_server, smtp_svr?smtp_svr:"");
	smtp_port = port?port:25;
	strcpy (smtp_user, username);
	strcpy (smtp_pwd, password);
}

bool ATOM_ErrorSender::sendFileViaEmail (const char *fileName)
{
	return true;
}

bool ATOM_ErrorSender::sendFile (const char *fileName, const char *destDir, bool text)
{
	if (_sendErrorFile (fileName, destDir, text))
	{
		return true;
	}

	if (smtp_server[0])
	{
		sendFileViaEmail (fileName);
	}

	if (!fileName || !fileName[0] || !destDir || !destDir[0])
	{
		return false;
	}

	if (destDir[0] == '/')
	{
		destDir++;
	}

	char localFileName[MAX_PATH];
	char *Name = 0;
	if (!::GetFullPathNameA (fileName, MAX_PATH, localFileName, &Name))
	{
		return false;
	}

	char tmpPath[MAX_PATH];
	strcpy (tmpPath, "dmpfiles\\");
	strcat (tmpPath, destDir);

	char fullPath[MAX_PATH];
	if (!::GetFullPathNameA (tmpPath, MAX_PATH, fullPath, NULL))
	{
		return false;
	}

	if (!_makeDirectory (fullPath))
	{
		return false;
	}

	if (!_isDirectory (fullPath))
	{
		return false;
	}
	
	if (fullPath[strlen(fullPath)-1] != '\\')
	{
		strcat (fullPath, "\\");
	}
	strcat (fullPath, Name);

	if (!::CopyFileA (localFileName, fullPath, FALSE))
	{
		return false;
	}

	return true;
}

