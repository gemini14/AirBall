#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <memory>

#include <boost/noncopyable.hpp>

namespace Tuatara
{
	struct FMOD_System;

	class SoundSystem : boost::noncopyable
	{
	private:

		std::shared_ptr<FMOD_System> system;

	public:

		SoundSystem();
		~SoundSystem();

		bool SoundSystemInitOK() const;
		
	};
}

#endif