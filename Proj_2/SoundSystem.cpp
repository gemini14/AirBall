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
		FMOD::System *system;
		bool init;

		FMOD_System();
		~FMOD_System();
		void DisplayError( FMOD_RESULT& result, const std::string& functionName );
		bool SoundSystemInitOK() const;
	};

	///// private implementation /////

	FMOD_System::FMOD_System() : init( false )
	{
		FMOD_RESULT result;
		result = FMOD::System_Create( &system );

		if( result != FMOD_OK )
		{
			DisplayError( result, "SoundSystem::Constructor" );
		}
		else
		{
			result = system->init( 500, FMOD_INIT_NORMAL, nullptr );
			if( result != FMOD_OK )
			{
				DisplayError( result, "SoundSystem::Constructor" );
			}
			else
			{
				init = true;
			}
		}
	}

	FMOD_System::~FMOD_System()
	{
		if( system != nullptr )
		{
			system->release();
		}
	}

	void FMOD_System::DisplayError( FMOD_RESULT& result, const std::string& functionName )
	{
		std::cout << "FMOD error in " << functionName << ": " << FMOD_ErrorString( result ) << "\n";
	}

	bool FMOD_System::SoundSystemInitOK() const
	{
		return init;
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
}