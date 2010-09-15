#include "LightSoundSystem.h"

#include <iostream>


namespace Tuatara
{

	void LightSoundSystem::DisplayError( const FMOD_RESULT& result )
	{
#if defined(_DEBUG) | defined(DEBUG)
		std::cout << "FMOD error: " << FMOD_ErrorString( result ) << "\n";
#endif
	}

	void LightSoundSystem::ErrorCheck( const FMOD_RESULT& result )
	{
		if( result != FMOD_OK )
		{
			DisplayError( result );
		}
	}

	LightSoundSystem::LightSoundSystem( const char *soundFilename ) 
		: system( nullptr), sound( nullptr ), soundChannel( nullptr )
	{
		ErrorCheck( FMOD::System_Create( &system ) );
		FMOD_RESULT result = system->init( 1, FMOD_INIT_NORMAL, nullptr );
		if( result == FMOD_OK )
		{
			ErrorCheck( system->createStream( soundFilename, FMOD_LOOP_NORMAL | FMOD_CREATESTREAM, nullptr, &sound ) );
		}
	}

	LightSoundSystem::~LightSoundSystem()
	{
		if( sound != nullptr )
		{
			system->release();
		}
		if( system != nullptr )
		{
			system->release();
		}		
	}

	void LightSoundSystem::AdjustVolume( const float& volume )
	{
		if( soundChannel != nullptr && volume <= 1.0f && volume >= 0.0f )
		{
			soundChannel->setVolume( volume );
		}
	}

	void LightSoundSystem::PausePlayback()
	{
		if( soundChannel != nullptr )
		{
			soundChannel->setPaused( true );
		}
	}
	
	void LightSoundSystem::ResumePlayback()
	{
		if( soundChannel != nullptr )
		{
			soundChannel->setPaused( false );
		}
	}

	void LightSoundSystem::StartPlayback( bool startPaused )
	{
		ErrorCheck( system->playSound( FMOD_CHANNEL_FREE, sound, startPaused, &soundChannel ) );
	}

	void LightSoundSystem::StopPlayback()
	{
		if( soundChannel != nullptr )
		{
			soundChannel->stop();
		}
	}

	void LightSoundSystem::Update()
	{
		if( system != nullptr )
		{
			system->update();
		}
	}

}