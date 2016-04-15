#pragma once

class ATOM_Vox
{
public:
	enum VOXSTATE
	{
		STATE_PAUSE = 0,
		STATE_PLAYING
	};

	enum CHARACTERCODE
	{
		CODE_SJIS = 0,
		CODE_UNICODE,
		CODE_UTF8
	};

#pragma pack( push, 1 )
	struct VOXCOMMENT
	{
		char **user_comments;
		int   *comment_lengths;
		int    comments;
		char  *vendor;
	};
#pragma pack( pop )

public:
	virtual ~ATOM_Vox (void) {}
	virtual bool Load( const char *pFileName ) = 0;
	virtual bool Play() = 0;
	virtual bool Pause() = 0;
	virtual bool Release() = 0;
	virtual bool SetLoop( int Count ) = 0;
	virtual bool CheckDevice() = 0;
	virtual bool SetVolume( float Volume ) = 0;
	virtual float GetVolume() = 0;
	virtual bool Fade( float StartVolume, float EndVolume, int FadeTime ) = 0;

	virtual bool Delete() = 0;

	virtual float GetTotalTime() = 0;
	virtual float GetCurrentTime() = 0;
	virtual bool Seek( float Time ) = 0;
	virtual VOXSTATE GetStatus() = 0;
	virtual VOXCOMMENT* GetComment() = 0;

	virtual char* ParseComment( char *pFieldName, CHARACTERCODE CharacterCode = CODE_SJIS ) = 0;
};

int  ATOM_VoxLoad( char *pFileName );
bool ATOM_VoxPlay( int ID );
bool ATOM_VoxPause( int ID );
bool ATOM_VoxRelease( int ID );
bool ATOM_VoxSetLoop( int ID, int Count );
bool ATOM_VoxCheckDevice( int ID );
bool ATOM_VoxSetVolume( int ID, float Volume );
int  ATOM_VoxGetVolume( int ID );
bool ATOM_VoxFade( int ID, float StartVolume, float EndVolume, int FadeTime );
bool ATOM_VoxDelete( int ID );
int  ATOM_VoxGetTotalTime( int ID );
int  ATOM_VoxGetCurrentTime( int ID );
bool ATOM_VoxSeek( int ID, int Time );
int  ATOM_VoxGetStatus( int ID );
ATOM_Vox::VOXCOMMENT* ATOM_VoxGetComment( int ID );
char* ATOM_VoxParseComment( int ID, char *pFieldName, ATOM_Vox::CHARACTERCODE CharacterCode = ATOM_Vox::CODE_SJIS );

