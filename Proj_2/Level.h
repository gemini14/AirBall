#ifndef LEVEL_H
#define LEVEL_H


#include <memory>
#include <string>

#include <boost/noncopyable.hpp>
#include <irrlicht.h>

#include "EnumsConstants.h"


namespace Tuatara
{
	class Game;
	struct Level_;

	class Level : boost::noncopyable
	{
	private:

		std::shared_ptr<Level_> level_;

	public:

		Level();
		~Level();

		void ApplyDirectionToCamera( Direction dir );
		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0);
		bool InitLevel( Game *game, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
			irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex, irr::video::ITexture *transTex );
		void Pause( bool pause );
		void PlayJetSound();
		bool IsJetPlaying();
		bool StepSimulation( float timeDelta = 0 );
	};

}

#endif