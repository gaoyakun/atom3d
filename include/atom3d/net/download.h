#ifndef ATOM_NET_DOWNLOAD_H
#define ATOM_NET_DOWNLOAD_H

#ifdef _MSC_VER
# pragma comment(lib,"wininet.lib")
#endif

#include "basedefs.h"

struct ATOM_DownloadContext;

struct ATOM_DownloadParams
{
	unsigned long bandwidthLimit;
	unsigned long threadLimit;
};

ATOM_NET_API void ATOM_CALL ATOM_DownloadConfig (const ATOM_DownloadParams &params);
ATOM_NET_API ATOM_DownloadContext * ATOM_CALL ATOM_NewDownload (const char *URL, unsigned bandwidthLimit = 0, bool compressed = false, const char *param = 0);
ATOM_NET_API void ATOM_CALL ATOM_SetDownloadCheckSumFunction (ATOM_DownloadContext *context, int (*funcCalcChecksum)(int, const void*, unsigned));
ATOM_NET_API void ATOM_CALL ATOM_SetDownloadWaitIdleFunction (ATOM_DownloadContext *context, void (*funcWaitIdle)(void*), void *waitIdleUserData);
ATOM_NET_API void ATOM_CALL ATOM_DestroyDownload (ATOM_DownloadContext *context);
ATOM_NET_API bool ATOM_CALL ATOM_StartDownloading (ATOM_DownloadContext *context, const char *localFileName = 0);
ATOM_NET_API int ATOM_CALL ATOM_GetDownloadingStatus (ATOM_DownloadContext *context);
ATOM_NET_API int ATOM_CALL ATOM_WaitForDownloading (ATOM_DownloadContext *context, unsigned timeout);
ATOM_NET_API void ATOM_CALL ATOM_CancelDownloading (ATOM_DownloadContext *context);
ATOM_NET_API float ATOM_CALL ATOM_GetDownloadAverageSpeed (ATOM_DownloadContext *context);
ATOM_NET_API const char * ATOM_CALL ATOM_GetDownloadFileName (ATOM_DownloadContext *context);
ATOM_NET_API const char * ATOM_CALL ATOM_GetDownloadedFileName (ATOM_DownloadContext *context);
ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadFileLength (ATOM_DownloadContext *context);
ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadedFileLength (ATOM_DownloadContext *context);
ATOM_NET_API float ATOM_CALL ATOM_GetDownloadPercentage (ATOM_DownloadContext *context);
ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadConnectTime (ATOM_DownloadContext *context);
ATOM_NET_API unsigned long ATOM_CALL ATOM_GetDownloadTime (ATOM_DownloadContext *context);
ATOM_NET_API int ATOM_CALL ATOM_GetDownloadCheckSum (ATOM_DownloadContext *context);
ATOM_NET_API unsigned long ATOM_CALL ATOM_GetNumDownloads (void);
ATOM_NET_API ATOM_DownloadContext * ATOM_CALL ATOM_GetDownload (unsigned long index);

#endif // ATOM_NET_DOWNLOAD_H
