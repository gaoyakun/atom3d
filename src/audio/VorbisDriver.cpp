#include <windows.h>
#include <process.h>
#include <mmsystem.h>

#include <ATOM_dbghlp.h>
#include "VorbisDriver.h"


ATOM_Vox * CreateVox()
{
	ATOM_STACK_TRACE(CreateVox);

    return ATOM_NEW(ATOM_VorbisDriver);	
}

bool ATOM_VorbisDriver::Delete()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Delete);

    ATOM_DELETE(this);
	return true;
}

ATOM_VorbisDriver::ATOM_VorbisDriver()
: fp( NULL ), pInfo( NULL ),
hWaveOut( NULL ), hSemaphore( NULL ), hThread( NULL ), ThreadState( THREAD_STOP ),
IsATOM_VorbisDriverInitialized( false ), DecodePage( 0 ), LoopCounter( 0 ), Volume( 1 ), SeekOffset( 0 ), IsFade( false ),
pSJISBuffer( NULL )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::ATOM_VorbisDriver);

	hSemaphore = CreateSemaphore( NULL, MAXBUFFER, MAXBUFFER, NULL );

	if( waveOutGetNumDevs() != 0 ) IsDeviceExist = true;
	else IsDeviceExist = false;
}

ATOM_VorbisDriver::~ATOM_VorbisDriver()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::~ATOM_VorbisDriver);

	ATOM_DELETE_ARRAY(pSJISBuffer);

	Release();

	if( hSemaphore ){
		CloseHandle( hSemaphore );
		hSemaphore = NULL;
	}
}

bool ATOM_VorbisDriver::Load( const char *pFileName )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Load);

	if( !IsDeviceExist ) return false;

	if( !hSemaphore )  return false;

	if( !hThread ){
		DWORD ThreadID;
		ThreadState = THREAD_PAUSE;
		hThread = (HANDLE)_beginthreadex( NULL, 0, (THREADFUNC)ThreadProc, (LPVOID)this, 0, (unsigned int *)&ThreadID );
		if( hThread == 0 ) return false;
	}

	if( ThreadState == THREAD_RUN ) return false;

	fp = fopen( pFileName, "rb" );
	if( !fp ) return false;

	if( ov_open( fp, &vf, NULL, 0 ) < 0 ) return false;
	pInfo = ov_info( &vf, -1 );
	vorbis_comment* pComment = ov_comment( &vf, -1 );
	Comment.user_comments	= pComment->user_comments;
	Comment.comment_lengths	= pComment->comment_lengths;
	Comment.comments		= pComment->comments;
	Comment.vendor			= pComment->vendor;

	IsATOM_VorbisDriverInitialized = true;

	WAVEFORMATEX wfex;
	ZeroMemory( &wfex, sizeof( wfex ) );
	wfex.wFormatTag      = WAVE_FORMAT_PCM;
	wfex.nChannels       = pInfo->channels;
	wfex.nSamplesPerSec  = pInfo->rate;
	wfex.wBitsPerSample  = 16;
	wfex.nBlockAlign     = wfex.nChannels * wfex.wBitsPerSample / 8;
	wfex.nAvgBytesPerSec = wfex.nBlockAlign * wfex.nSamplesPerSec;
	wfex.cbSize          = 0;

	
	MMRESULT rtn = waveOutOpen( &hWaveOut, 
		WAVE_MAPPER, 
		&wfex,
		(DWORD_PTR)WaveProc, 
		(DWORD_PTR)this,
		WAVE_ALLOWSYNC | CALLBACK_FUNCTION );
	if( rtn != MMSYSERR_NOERROR ) return false;

	for( int i = 0; i < MAXBUFFER; i++ ){
		memset( &WaveHdr[i], 0, sizeof( WAVEHDR ) );
		WaveHdr[i].lpData = ATOM_NEW_ARRAY(char, BUFFERSIZE); 
	}

	return true;
}

bool ATOM_VorbisDriver::Release()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Release);

	if( !IsDeviceExist ) return false;

	if( hThread ){
		ThreadState = THREAD_STOP;
		WaitForSingleObject( hThread, 5000 ); 

		if( CloseHandle( hThread ) == 0 ) return false;
		hThread = NULL;
	}

	if( hWaveOut )
	{
		waveOutReset( hWaveOut );
		for( int i = 0; i < MAXBUFFER; i++ ){
			while( waveOutUnprepareHeader( hWaveOut, &WaveHdr[i], sizeof( WAVEHDR ) ) == WAVERR_STILLPLAYING ) Sleep(100);
			ATOM_DELETE_ARRAY(WaveHdr[i].lpData);
		}
		waveOutClose( hWaveOut );
		hWaveOut = NULL;
	}

	if( IsATOM_VorbisDriverInitialized == true ){
		ov_clear( &vf );
		IsATOM_VorbisDriverInitialized = false;
	}

	if( fp ) fclose( fp );
	fp = NULL;

	ThreadState = THREAD_STOP;
	DecodePage = 0;
	LoopCounter = 0;
	Volume = 1;
	SeekOffset = 0;
	IsFade = false;

	return true;
}

int ATOM_VorbisDriver::Decode()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Decode);

	if( !IsDeviceExist ) return -1;

	if( LoopCounter < 0 ) return -1;

	int eof = 1;
	int Counter = 0;

	while( eof )
	{
		int CurrentSection;
		long Result = ov_read( &vf, ConvBuffer, sizeof( ConvBuffer ), 0, 2, 1, &CurrentSection );
		if( Result == 0 ) eof = 0;
		else if( Result < 0 ) ;
		else
		{
			short *pBuffer = (short*)ConvBuffer;
			int DataNum = Result / 2;
			for( int i = 0 ; i < DataNum; i++ ){
				pBuffer[i] = (short)( (float)pBuffer[i] * Volume );

				if( IsFade ){
					Volume += ( EndVolume - StartVolume ) / ( ( (float)FadeTime / 1000 ) * pInfo->rate * pInfo->channels );
					if( StartVolume < EndVolume ){
						if( Volume > EndVolume ) Volume = EndVolume;
					}
					else{
						if( Volume < EndVolume ) Volume = EndVolume;
					}
				}
			}

			memcpy( WaveHdr[DecodePage].lpData + Counter, ConvBuffer, Result );
			Counter += Result;
			if( Counter > BUFFERSIZE - ( 4096 + 10 ) ) break; 
		}
	}

	if( Counter != 0 ){
		if( waveOutUnprepareHeader( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ){
			return -1;
		}

		WaveHdr[DecodePage].dwBytesRecorded	= 0;
		WaveHdr[DecodePage].dwFlags			= 0;
		WaveHdr[DecodePage].dwLoops			= 0;
		WaveHdr[DecodePage].dwUser			= 0;
		WaveHdr[DecodePage].lpNext			= 0;
		WaveHdr[DecodePage].reserved		= 0;

		WaveHdr[DecodePage].dwBufferLength	= Counter;

		if( waveOutPrepareHeader( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;
		if( waveOutWrite( hWaveOut, &WaveHdr[DecodePage], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;

		++DecodePage;
		if( DecodePage >= MAXBUFFER ) DecodePage = 0;
	}

	if( eof == 0 )
	{
		DWORD MMTimeOld = 0;
		for( int i = 0; i < 3000; i++ )
		{ 
			MMTIME MMTime;
			MMTime.wType = TIME_BYTES;
			if( waveOutGetPosition( hWaveOut, &MMTime, sizeof( MMTIME ) ) != MMSYSERR_NOERROR ) break;
			if( MMTimeOld == MMTime.u.sample ) break;
			MMTimeOld = MMTime.u.sample;
			Sleep( 10 );
		}

		waveOutReset( hWaveOut );
		SeekOffset = 0;
		ov_pcm_seek( &vf, 0 ); 
	}

	if( Counter == 0 ) return -1;

	return eof;
}

void CALLBACK ATOM_VorbisDriver::WaveProc( HWAVEOUT hWaveOut, UINT msg,
		DWORD instance, DWORD param1, DWORD param2 )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::WaveProc);

	switch( msg )
	{
		case WOM_DONE:
			ATOM_VorbisDriver *pVorbisDriver = (class ATOM_VorbisDriver *)( (DWORD_PTR)instance );
			ReleaseSemaphore( pVorbisDriver->hSemaphore, 1, NULL );
			break;
	}
}

unsigned int __stdcall ATOM_VorbisDriver::ThreadProc( void* pParam )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::ThreadProc);

	ATOM_VorbisDriver *pVorbisDriver = (ATOM_VorbisDriver *)pParam;
	int Result;

	while(1)
	{
		while( 1 )
		{
			Sleep( 100 );

			if( pVorbisDriver->ThreadState == THREAD_STOP ) goto EXIT;		
			if( pVorbisDriver->ThreadState == THREAD_PAUSE ) goto CONTINUE;	
			if( !pVorbisDriver->fp ) goto CONTINUE;							

			int Result = WaitForSingleObject( pVorbisDriver->hSemaphore, 100 );
			if( Result == WAIT_OBJECT_0 ) break;
			if( Result == WAIT_FAILED || Result == WAIT_ABANDONED ) goto EXIT;
		}
		Result = pVorbisDriver->Decode();

		if( Result == 0 ) --pVorbisDriver->LoopCounter;
		if( Result == -1 ) 
		{
			ReleaseSemaphore( pVorbisDriver->hSemaphore, 1, NULL );
		}

		if( pVorbisDriver->IsFade )
		{
			DWORD Time = timeGetTime();
			if( pVorbisDriver->FadeStartTime + pVorbisDriver->FadeTime < Time ){
				pVorbisDriver->Volume = pVorbisDriver->EndVolume;
				pVorbisDriver->IsFade = false;
			}
		}
CONTINUE: ;
	}

EXIT:
	_endthreadex(0);

	return 0;
}

bool ATOM_VorbisDriver::Play()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Play);

	if( ThreadState == THREAD_PAUSE ) 
	{
		MMRESULT rtn = waveOutRestart( hWaveOut ); 
		if(rtn != MMSYSERR_NOERROR)
		{

		}
	}
	ThreadState = THREAD_RUN;

	return true;
}

bool ATOM_VorbisDriver::Pause()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Pause);

	waveOutPause( hWaveOut );
	ThreadState = THREAD_PAUSE;

	return true;
}

bool ATOM_VorbisDriver::SetLoop( int Count )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::SetLoop);

	if( Count < 0 ) return false;
	LoopCounter = Count;

	return true;
}

int ATOM_VorbisDriver::GetLoop() const
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetLoop);

	return LoopCounter;
}

bool ATOM_VorbisDriver::CheckDevice()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::CheckDevice);

	if( waveOutGetNumDevs() == 0 ) return false;
	
	return true;
}

bool ATOM_VorbisDriver::SetVolume( float Volume )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::SetVolume);

	if( Volume > 1.0f ) return false;

	this->Volume = Volume;

	return true;
}

float ATOM_VorbisDriver::GetVolume()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetVolume);

	return Volume;
}

bool ATOM_VorbisDriver::Fade( float StartVolume, float EndVolume, int FadeTime )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Fade);

	if( StartVolume > 1.0f || StartVolume < 0.0f ) return false;
	if( EndVolume   > 1.0f || EndVolume   < 0.0f ) return false;
	if( FadeTime < 0 ) return false;

	this->StartVolume	= StartVolume;
	this->EndVolume		= EndVolume;
	this->FadeTime		= FadeTime;
	FadeStartTime		= timeGetTime();
	Volume				= StartVolume;
	IsFade				= true;

	return true;
}

float ATOM_VorbisDriver::GetTotalTime()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetTotalTime);

	return ( (float)ov_pcm_total( &vf, -1 ) / pInfo->rate ) * 1000;
}

float ATOM_VorbisDriver::GetCurrentTime()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetCurrentTime);

	MMTIME MMTime;
	MMTime.wType = TIME_BYTES;
	waveOutGetPosition( hWaveOut, &MMTime, sizeof( MMTIME ) );

	return (float)MMTime.u.cb / pInfo->rate / pInfo->channels / 2 * 1000 + SeekOffset;
}

bool ATOM_VorbisDriver::Seek( float Time )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::Seek);

	if( Time > GetTotalTime() ) return false;
	ov_pcm_seek( &vf, (ogg_int64_t)( Time / 1000 * pInfo->rate ) );

	waveOutReset( hWaveOut );
	SeekOffset = Time;

	return true;
}

ATOM_Vox::VOXSTATE ATOM_VorbisDriver::GetStatus()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetStatus);

	if( ThreadState == THREAD_RUN && LoopCounter >= 0 ) return STATE_PLAYING;

	return STATE_PAUSE;
}

ATOM_Vox::VOXCOMMENT* ATOM_VorbisDriver::GetComment()
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::GetComment);

	return &Comment;
}

char* ATOM_VorbisDriver::ParseComment( char *pFieldName, CHARACTERCODE CharacterCode )
{
	ATOM_STACK_TRACE(ATOM_VorbisDriver::ParseComment);

	for( int i = 0; i < Comment.comments; ++i )
	{
		if( strncmp( Comment.user_comments[i], pFieldName, strlen( pFieldName ) ) == 0 )
		{
			char *pComment = Comment.user_comments[i] + strlen( pFieldName ) + 1;
			int BufferSize = MultiByteToWideChar( CP_UTF8, 0, pComment, -1, NULL, 0 );
			WCHAR* pUnicodeBuffer = ATOM_NEW_ARRAY(WCHAR, BufferSize + 16); 
			ZeroMemory( pUnicodeBuffer, sizeof( pUnicodeBuffer ) );
			MultiByteToWideChar( CP_UTF8, 0, pComment, -1, pUnicodeBuffer, BufferSize );
			BufferSize = WideCharToMultiByte( CP_ACP, 0, pUnicodeBuffer, -1, NULL, 0, NULL, NULL );
			ATOM_DELETE_ARRAY(pSJISBuffer);
			pSJISBuffer = ATOM_NEW_ARRAY(CHAR, BufferSize + 16); 
			ZeroMemory( pSJISBuffer, sizeof( pSJISBuffer ) );
			WideCharToMultiByte( CP_ACP, 0, pUnicodeBuffer, -1, pSJISBuffer, BufferSize, NULL, NULL );
 
			ATOM_DELETE_ARRAY(pUnicodeBuffer);

			return pSJISBuffer;
		}
	}

	return NULL;
}
