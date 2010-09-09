#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H


#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <string>
#include <vector>


namespace Tuatara
{
	class SoundSystem
	{
	private:

		FMOD::System *system;

		bool init;

		void DisplayError( FMOD_RESULT result, const std::string& functionName );

	public:

		SoundSystem();
		~SoundSystem();

		bool SoundSystemInitOK() const { return init; }
		
	};
}

#endif