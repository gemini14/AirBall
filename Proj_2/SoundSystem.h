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
		void CreateVentSound( const float& x, const float& y, const float& z );
		
		void StartPlayingLoopingSounds();
		void PlayJetSound();
		void PausePlayback();
		void ResumePlayback();
		
		void Update( const float& posX, const float& posY, const float& posZ, 
			 const float& velocityX, const float& velocityY, const float& velocityZ, 
			  const float& forwardX, const float& forwardY, const float& forwardZ );
		
	};
}

#endif