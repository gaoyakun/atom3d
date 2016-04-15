#ifndef __ATOM3D_NET_DOWNLOAD_CONTEXT_H
#define __ATOM3D_NET_DOWNLOAD_CONTEXT_H

#include <windows.h>

typedef int (*FuncCalcChecksum)(int lastCheckSum, const void *buffer, unsigned bufferSize);
typedef bool (*FuncCheckFile)(const char *filename, int checksum);

struct ATOM_DownloadContext
{
	char *URL;
	unsigned bandwidthLimit;
	char *localFileName;
	int downloadStatus;
	int compressed;
	int checksum;
	unsigned totalSize;
	unsigned currentSize;
	unsigned connectTime;
	unsigned downloadTime;
	float averageSpeed;
	HANDLE cancelEvent;
	HANDLE hThread;
	int (*funcCalcChecksum)(int, const void*, unsigned);
	void (*funcWaitIdle)(void*);
	void *waitIdleUserData;
};

#endif // __ATOM3D_NET_DOWNLOAD_CONTEXT_H
