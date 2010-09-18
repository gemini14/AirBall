#ifndef LIGHTSOUNDSYSTEM_H
#define LIGHTSOUNDSYSTEM_H

#include <fmod.hpp>
#include <fmod_errors.h>

#include <boost/noncopyable.hpp>

namespace Tuatara
{
	// VERY lightweight class, only intended to play and control one sound
	class LightSoundSystem : boost::noncopyable
	{
	private:

		FMOD::System *system;
		FMOD::Sound *sound;
		FMOD::Channel *soundChannel;

		void DisplayError( const FMOD_RESULT& result );
		void ErrorCheck( const FMOD_RESULT& result );

	public:

		LightSoundSystem( const char *soundFilename );
		~LightSoundSystem();

		void AdjustVolume( const float& volume );
		void PausePlayback();
		void ResumePlayback();
		void StartPlayback( bool startPaused );
		void StopPlayback();
		void Update();
	};
}

#endif