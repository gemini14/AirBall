#pragma once

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

		bool InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
			irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex, irr::video::ITexture *transTex,
			irr::video::ITexture *ventTransTex, irr::video::ITexture *exitTransTex );
		void HandleMouseClick( irr::s32 x, irr::s32 y );
		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0);
		void ApplyDirectionToCamera( Direction dir );
		void Pause( bool pause );
		bool StepSimulation( float timeDelta = 0 );
		void PlayJetSound();
		void EndLevel(bool inWin);
	};

}