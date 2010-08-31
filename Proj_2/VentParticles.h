#pragma once

#include <irrlicht.h>
#include <string>

#include "EnumsConstants.h"
#include "Game.h"

namespace Tuatara
{
	class VentParticles
	{
		irr::scene::IParticleSystemSceneNode* particleSystem;


	public:
		VentParticles(void);
		~VentParticles(void);

		void Start (Game& game, irr::core::vector3df position, irr::core::vector3df normalDirection, irr::f32 distance);
	};
}

