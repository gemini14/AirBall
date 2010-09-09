#include "SoundSystem.h"


namespace Tuatara
{
	SoundSystem::SoundSystem() : system( nullptr ), init( false )
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

	void SoundSystem::DisplayError( FMOD_RESULT result, const std::string& functionName )
	{
		std::cout << "FMOD error in " << functionName << ": " << FMOD_ErrorString( result ) << "\n";
	}

	SoundSystem::~SoundSystem()
	{
		if( system != nullptr )
		{
			system->release();
		}
	}
}