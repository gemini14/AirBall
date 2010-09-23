#include "SoundSystem.h"

#include <algorithm>
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
		FMOD_VECTOR ballPosition;

		FMOD::Sound *bgmusic;
		FMOD::Sound *ventSound;

		FMOD::Sound *collision;
		FMOD::Channel *collisionChannel;

		FMOD::Sound *jet;
		FMOD::Channel *jetChannel;

		FMOD::Sound *roll;
		FMOD::Channel *rollChannel;

		struct VentPoint
		{
			float x, y, z;
		};
		std::vector<VentPoint> vents;

		FMOD_System();
		~FMOD_System();

		void ErrorCheck( const FMOD_RESULT& result );
		void DisplayError( const FMOD_RESULT& result );

		bool SoundSystemInitOK() const;
		void CreateSounds( const SoundFilenameMap& soundFilenames );
		void CreateVentSound( const float& x, const float& y, const float& z );

		void StartPlayingLoopingSounds();
		bool IsPlaying( FMOD::Channel *channel );
		void PlayCollisionSound();
		void PlayJetSound();
		void PlayRollSound();
		void PausePlayback();
		void ResumePlayback();

		void Update( const float& posX, const float& posY, const float& posZ, 
			const float& velocityX, const float& velocityY, const float& velocityZ, 
			const float& forwardX, const float& forwardY, const float& forwardZ );
	};

	///// private implementation /////

	FMOD_System::FMOD_System() : init( false ), system( nullptr ), bgmusic( nullptr ), collision( nullptr ),
		jet( nullptr ), ventSound( nullptr ), collisionChannel( nullptr ), jetChannel( nullptr ), roll( nullptr ),
		rollChannel( nullptr )
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
		auto safeDelete = []( FMOD::Sound* sound )
		{
			if( sound != nullptr )
			{
				sound->release();
			}
		};

		if( init )
		{
			safeDelete( bgmusic );
			safeDelete( jet );
			safeDelete( ventSound );
			safeDelete( roll );

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
#if defined(_DEBUG) | defined(DEBUG)
		std::cout << "FMOD error: " << FMOD_ErrorString( result ) << "\n";
#endif
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
			ErrorCheck( system->createStream( soundFilenames.at("bgmusic").c_str(), FMOD_LOOP_NORMAL | FMOD_CREATESTREAM,
				nullptr, &bgmusic ) );
			bgmusic->setDefaults( 44100, 0.5f, 0.f, 128 );
		}
		if( isPresent( "jet" ) )
		{
			ErrorCheck( system->createSound( soundFilenames.at( "jet" ).c_str(), FMOD_3D, nullptr, &jet ) );
			jet->setDefaults( 44100, 1.f, 0.f, 128 );
		}
		if( isPresent( "vent" ) )
		{
			ErrorCheck( system->createSound( soundFilenames.at( "vent" ).c_str(), FMOD_3D | FMOD_LOOP_NORMAL,
				nullptr, &ventSound ) );
			ventSound->setDefaults( 44100, 0.3f, 0.f, 128 );
		}
		if( isPresent( "collision" ) )
		{
			ErrorCheck( system->createSound( soundFilenames.at( "collision" ).c_str(), FMOD_3D, nullptr, &collision ) );
			collision->setDefaults( 44100, 0.9f, 0.f, 128 );
		}
		if( isPresent( "roll" ) )
		{
			ErrorCheck( system->createSound( soundFilenames.at( "roll" ).c_str(), FMOD_3D, nullptr, &roll ) );
			roll->setDefaults( 44100, 2.f, 0.f, 128 );
		}
	}

	void FMOD_System::CreateVentSound( const float& x, const float& y, const float& z )
	{
		VentPoint v = { x, y, z };
		vents.push_back( v );
	}

	void FMOD_System::StartPlayingLoopingSounds()
	{
		if( bgmusic != nullptr )
		{
			system->playSound( FMOD_CHANNEL_FREE, bgmusic, false, nullptr );
		}

		if( ventSound != nullptr )
		{
			std::for_each( vents.begin(), vents.end(), [&]( VentPoint v )
			{
				FMOD::Channel *vent_channel;
				FMOD_System::system->playSound( FMOD_CHANNEL_FREE, ventSound, true, &vent_channel );
				FMOD_VECTOR position = { v.x, v.y, v.z };
				vent_channel->set3DAttributes( &position, nullptr );
				vent_channel->setPaused( false );
			});
		}
	}

	bool FMOD_System::IsPlaying( FMOD::Channel *channel )
	{
		bool playing;
		if( channel != nullptr )
		{
			channel->isPlaying( &playing );
			return playing;
		}

		return false;
	}

	void FMOD_System::PlayCollisionSound()
	{
		if( collision != nullptr )
		{
			if( !IsPlaying( collisionChannel ) )
			{
				system->playSound( FMOD_CHANNEL_FREE, collision, true, &collisionChannel );
				collisionChannel->set3DAttributes( &ballPosition, nullptr );
				collisionChannel->setPaused( false );
			}
		}
	}

	void FMOD_System::PlayJetSound()
	{
		if( jet != nullptr )
		{
			if( !IsPlaying( jetChannel ) )
			{
				system->playSound( FMOD_CHANNEL_FREE, jet, true, &jetChannel );
				jetChannel->set3DAttributes( &ballPosition, nullptr );
				jetChannel->setPaused( false );
			}
		}
	}

	void FMOD_System::PlayRollSound()
	{
		if( roll != nullptr )
		{
			if( !IsPlaying( rollChannel ) && !IsPlaying( collisionChannel ) )
			{
				system->playSound( FMOD_CHANNEL_FREE, roll, true, &rollChannel );
				rollChannel->set3DAttributes( &ballPosition, nullptr );
				rollChannel->setPaused( false );
			}
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

	void FMOD_System::Update( const float& posX, const float& posY, const float& posZ, 
		const float& forwardX, const float& forwardY, const float& forwardZ, 
		const float& ballPosX, const float& ballPosY, const float& ballPosZ )
	{
		FMOD_VECTOR position = { posX, posY, posZ };
		FMOD_VECTOR forward = { forwardX, forwardY, forwardZ };
		system->set3DListenerAttributes( 0, &position, nullptr, &forward, nullptr );
		ballPosition.x = ballPosX;
		ballPosition.y = ballPosY;
		ballPosition.z = ballPosZ;
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

	void SoundSystem::CreateVentSound( const float& x, const float& y, const float& z )
	{
		system->CreateVentSound( x, y, z );
	}

	void SoundSystem::StartPlayingLoopingSounds()
	{
		system->StartPlayingLoopingSounds();
	}

	void SoundSystem::PlayCollisionSound()
	{
		system->PlayCollisionSound();
	}

	void SoundSystem::PlayJetSound()
	{
		system->PlayJetSound();
	}

	void SoundSystem::PlayRollSound()
	{
		system->PlayRollSound();
	}

	void SoundSystem::PausePlayback()
	{
		system->PausePlayback();
	}

	void SoundSystem::ResumePlayback()
	{
		system->ResumePlayback();
	}

	void SoundSystem::Update( const float& posX, const float& posY, const float& posZ, 
		const float& forwardX, const float& forwardY, const float& forwardZ, 
		const float& ballPosX, const float& ballPosY, const float& ballPosZ )
	{
		system->Update( posX, posY, posZ, forwardX, forwardY, forwardZ, ballPosX, ballPosY, ballPosZ );
	}
}