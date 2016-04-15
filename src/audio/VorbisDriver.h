#pragma once

#include <windows.h>
#include <stdio.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>
#include <MMSystem.h>
#include "vox.h"


#define MAXBUFFER	4
#define BUFFERSIZE	( 1024 * 64 )

typedef unsigned int (__stdcall *THREADFUNC)( void* pParam );

class ATOM_VorbisDriver : public ATOM_Vox
{
	enum THREADSTATE
	{
		THREAD_RUN,
		THREAD_PAUSE,
		THREAD_STOP
	};

public:
	ATOM_VorbisDriver();
	~ATOM_VorbisDriver();
	bool Load( const char *pFileName );
	bool Play();
	bool Pause();
	bool Release();
	bool SetLoop( int Count );
	int GetLoop() const;
	bool CheckDevice();
	bool SetVolume( float Volume );
	float GetVolume();
	bool Fade( float StartVolume, float EndVolume, int FadeTime );

	bool Delete();

	float GetTotalTime();
	float GetCurrentTime();
	bool Seek( float Time );
	VOXSTATE GetStatus();
	VOXCOMMENT* GetComment();

	char* ParseComment( char *pFieldName, CHARACTERCODE CharacterCode = CODE_SJIS );

private:
	int Decode();
	static void CALLBACK WaveProc( HWAVEOUT hWaveOut, UINT msg,
			DWORD instance, DWORD param1, DWORD param2 );
	static unsigned int __stdcall ThreadProc( void* pThreadParam );

private:
	FILE *fp;
	char ConvBuffer[4096]; /* take 4k out of the data segment, not the stack */

	OggVorbis_File vf;
	vorbis_info *pInfo;
	VOXCOMMENT Comment;
	CHAR* pSJISBuffer; // ParseComment

	HWAVEOUT hWaveOut;
	WAVEHDR	WaveHdr[MAXBUFFER];

	HANDLE hSemaphore;
	HANDLE hThread;
	THREADSTATE ThreadState;

	bool IsDeviceExist;
	bool IsATOM_VorbisDriverInitialized;
	int DecodePage;
	int LoopCounter;
	float Volume;
	float SeekOffset;

	bool IsFade;
	float StartVolume, EndVolume;
	DWORD FadeStartTime;
	int FadeTime;
};
