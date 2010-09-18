#pragma once


#include <string>

#include <irrlicht.h>

#include "EnumsConstants.h"


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