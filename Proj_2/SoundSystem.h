#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <map>
#include <memory>

#include <boost/noncopyable.hpp>

namespace Tuatara
{
	typedef std::map<std::string, std::string> SoundFilenameMap;

	struct FMOD_System;

	class SoundSystem : boost::noncopyable
	{
	private:

		std::shared_ptr<FMOD_System> system;

	public:

		SoundSystem();
		~SoundSystem();

		bool SoundSystemInitOK() const;
		void CreateSounds( const SoundFilenameMap& soundFilenameMap );
		
		void StartPlayingLoopingSounds();
		void PausePlayback();
		void ResumePlayback();
		
		void Update();
		
	};
}

#endif