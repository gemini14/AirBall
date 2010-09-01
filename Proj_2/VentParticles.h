#ifndef VENTPARTICLES_H
#define VENTPARTICLES_H

#include <irrlicht.h>
#include <string>

#include "EnumsConstants.h"
//#include "Game.h"

namespace Tuatara
{
	class VentParticles
	{
		irr::scene::IParticleSystemSceneNode* particleSystem;

	public:
		
		VentParticles( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
			irr::core::vector3df position, irr::core::vector3df normalDirection, irr::f32 distance );
		~VentParticles();
	};
}

#endif