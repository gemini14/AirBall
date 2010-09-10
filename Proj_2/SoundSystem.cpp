#include "SoundSystem.h"

#include <iostream>
#include <string>
#include <vector>

#include <fmod.hpp>
#include <fmod_errors.h>


namespace Tuatara
{

	struct FMOD_System
	{
		typedef std::map<std::string, std::string> SoundFilenameMap;

		FMOD::System *system;
		bool init;
		FMOD_RESULT result;

		FMOD::Sound *bgmusic;

		FMOD_System();
		~FMOD_System();
		
		void ErrorCheck( const FMOD_RESULT& result );
		void DisplayError( const FMOD_RESULT& result );
		
		bool SoundSystemInitOK() const;
		void CreateSounds( const SoundFilenameMap& soundFilenames );
		
		void StartPlayingLoopingSounds();
		void PausePlayback();
		void ResumePlayback();

		void Update();
	};

	///// private implementation /////

	FMOD_System::FMOD_System() : init( false ), system( nullptr ), bgmusic( nullptr )
	{
		ErrorCheck( FMOD::System_Create( &system ) );

		result = system->init( 500, FMOD_INIT_NORMAL, nullptr );
		if( result != FMOD_OK )
		{
			DisplayError( result );
		}
		else
		{
			init = true;
		}
	}

	FMOD_System::~FMOD_System()
	{
		if( init )
		{
			bgmusic->release();

			system->release();
		}
	}

	void FMOD_System::ErrorCheck( const FMOD_RESULT& result )
	{
		if( result != FMOD_OK )
		{
			DisplayError( result );
		}
	}

	void FMOD_System::DisplayError( const FMOD_RESULT& result )
	{
		std::cout << "FMOD error: " << FMOD_ErrorString( result ) << "\n";
	}

	bool FMOD_System::SoundSystemInitOK() const
	{
		return init;
	}

	void FMOD_System::CreateSounds( const SoundFilenameMap& soundFilenames )
	{
		auto isPresent = [=]( const std::string& key ) -> bool
		{
			return ( soundFilenames.find( key ) != soundFilenames.end() ) ? true : false;
		};
		
		if( isPresent( "bgmusic" ) )
		{
			ErrorCheck( system->createSound( soundFilenames.at("bgmusic").c_str(), FMOD_LOOP_NORMAL, nullptr, &bgmusic ) );
			bgmusic->setLoopCount( -1 );
			bgmusic->setDefaults( 44100, 0.5f, 0.f, 128 );
		}
	}

	void FMOD_System::StartPlayingLoopingSounds()
	{
		if( bgmusic != nullptr )
		{
			system->playSound( FMOD_CHANNEL_FREE, bgmusic, false, nullptr );
		}
	}

	void FMOD_System::PausePlayback()
	{
		FMOD::ChannelGroup *masterGroup;
		system->getMasterChannelGroup( &masterGroup );
		masterGroup->setPaused( true );
	}

	void FMOD_System::ResumePlayback()
	{
		FMOD::ChannelGroup *masterGroup;
		system->getMasterChannelGroup( &masterGroup );
		masterGroup->setPaused( false );
	}

	void FMOD_System::Update()
	{
		system->update();
	}


	///// public implementation /////
	
	SoundSystem::SoundSystem() : system( new FMOD_System )
	{		
	}

	SoundSystem::~SoundSystem()
	{
	}

	bool SoundSystem::SoundSystemInitOK() const
	{
		return system->SoundSystemInitOK();
	}

	void SoundSystem::CreateSounds( const SoundFilenameMap& soundFilenameMap )
	{
		system->CreateSounds( soundFilenameMap );
	}

	void SoundSystem::StartPlayingLoopingSounds()
	{
		system->StartPlayingLoopingSounds();
	}

	void SoundSystem::PausePlayback()
	{
		system->PausePlayback();
	}

	void SoundSystem::ResumePlayback()
	{
		system->ResumePlayback();
	}

	void SoundSystem::Update()
	{
		system->Update();
	}
}