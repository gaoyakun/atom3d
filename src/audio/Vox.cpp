#include <ATOM_dbghlp.h>
#include "VorbisDriver.h"

struct VOXDATA{
	ATOM_VorbisDriver* pVorbisDriver;
	int ID;
};

ATOM_VECTOR<VOXDATA> ATOM_VoxDataVector;

int ATOM_VoxLoad( char *pFileName )
{
	ATOM_STACK_TRACE(ATOM_VoxLoad);

	static int IDCounter = 0;
	VOXDATA ATOM_VoxData;
	ATOM_VoxData.pVorbisDriver = ATOM_NEW(ATOM_VorbisDriver);
	if( !ATOM_VoxData.pVorbisDriver->Load( pFileName ) ){
		ATOM_DELETE(ATOM_VoxData.pVorbisDriver);
		return -1;
	}
	ATOM_VoxData.ID = IDCounter;
	ATOM_VoxDataVector.push_back( ATOM_VoxData );
	++IDCounter;

	return ATOM_VoxData.ID;
}

bool ATOM_VoxPlay( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxPlay);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->Play();
		}
	}

	return false;
}

bool ATOM_VoxPause( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxPause);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->Pause();
		}
	}

	return false;
}

bool ATOM_VoxRelease( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxRelease);

	ATOM_VECTOR<VOXDATA>::iterator it;
	for( it = ATOM_VoxDataVector.begin(); it != ATOM_VoxDataVector.end(); it++ ){
		if( (*it).ID == ID ){
			bool Result = (*it).pVorbisDriver->Release();
			if( !Result ) return false;
			Result = (*it).pVorbisDriver->Delete(); 
			ATOM_VoxDataVector.erase( it );
			if( !Result ) return false;

			return true;
		}
	}

	return false;
}

bool ATOM_VoxSetLoop( int ID, int Count )
{
	ATOM_STACK_TRACE(ATOM_VoxSetLoop);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->SetLoop( Count );
		}
	}

	return false;
}

bool ATOM_VoxCheckDevice( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxCheckDevice);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->CheckDevice();
		}
	}

	return false;
}

bool ATOM_VoxSetVolume( int ID, int Volume )
{
	ATOM_STACK_TRACE(ATOM_VoxSetVolume);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->SetVolume( (float)Volume / 10000 );
		}
	}

	return false;
}

int ATOM_VoxGetVolume( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxGetVolume);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return (int)( ATOM_VoxDataVector[i].pVorbisDriver->GetVolume() * 10000 );
		}
	}

	return -1;
}

bool ATOM_VoxFade( int ID, int StartVolume, int EndVolume, int FadeTime )
{
	ATOM_STACK_TRACE(ATOM_VoxFade);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->Fade( (float)StartVolume / 10000, (float)EndVolume / 10000, FadeTime );
		}
	}

	return false;
}

bool ATOM_VoxDelete( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxDelete);

	ATOM_VECTOR<VOXDATA>::iterator it;
	for( it = ATOM_VoxDataVector.begin(); it != ATOM_VoxDataVector.end(); it++ ){
		if( (*it).ID == ID ){
			return true;
		}
	}

	return false;
}

int ATOM_VoxGetTotalTime( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxGetTotalTime);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return (int)ATOM_VoxDataVector[i].pVorbisDriver->GetTotalTime();
		}
	}

	return -1;
}

int ATOM_VoxGetCurrentTime( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxGetCurrentTime);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID )
		{			
			return (int)ATOM_VoxDataVector[i].pVorbisDriver->GetCurrentTime();
		}
	}

	return -1;
}

bool ATOM_VoxSeek( int ID, int Time )
{
	ATOM_STACK_TRACE(ATOM_VoxSeek);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return (int)ATOM_VoxDataVector[i].pVorbisDriver->Seek( (float)Time );
		}
	}

	return false;
}

int ATOM_VoxGetStatus( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxGetStatus);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			ATOM_Vox::VOXSTATE State = ATOM_VoxDataVector[i].pVorbisDriver->GetStatus();
			if( State == ATOM_Vox::STATE_PAUSE	) return 0;
			if( State == ATOM_Vox::STATE_PLAYING	) return 1;
		}
	}

	return -1;
}

ATOM_Vox::VOXCOMMENT* ATOM_VoxGetComment( int ID )
{
	ATOM_STACK_TRACE(ATOM_VoxGetComment);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->GetComment();
		}
	}

	return NULL;
}

char* ATOM_VoxParseComment( int ID, char *pFieldName, ATOM_Vox::CHARACTERCODE CharacterCode )
{
	ATOM_STACK_TRACE(ATOM_VoxParseComment);

	for( unsigned int i = 0; i < ATOM_VoxDataVector.size(); i++ ){
		if( ATOM_VoxDataVector[i].ID == ID ){
			return ATOM_VoxDataVector[i].pVorbisDriver->ParseComment( pFieldName, CharacterCode );
		}
	}

	return NULL;
}
