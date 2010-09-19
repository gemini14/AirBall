#include "VentParticles.h"

namespace Tuatara
{

	VentParticles::VentParticles( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
			irr::core::vector3df position, irr::core::vector3df normalDirection, irr::f32 distance )
			: particleSystem( nullptr )
	{
		// SMOKE EFFECT:
		particleSystem = smgr->addParticleSystemSceneNode(false, 0, -1, 
			position, 
			irr::core::vector3df(0, 0, 0), // rotation
			irr::core::vector3df(1.0f, 1.0f, 1.0f)); //scale

		irr::f32 gravity = -0.01f; // a good speed, I think.
		irr::f32 initialVelocity = 0.0188f / sqrt(8.0f / distance); // g=-0.01, iV=0.02 makes it go 8 units high. Dividing by sqrt(8/distance) scales to proper distance...
		initialVelocity *= 1.f+(distance*0.028f); // bugfix, because irr particles don't seem to conform to physics...
		irr::f32 maxTime = (initialVelocity / -gravity) * 450; // seems to work out--particles get to end, and just as they turn around, they disappear.

		irr::core::vector3df initialDirection = normalDirection * initialVelocity;

		irr::scene::IParticleSphereEmitter* em = particleSystem->createSphereEmitter(
			irr::core::vector3df(0, 0, 0),		// no offset from position
			irr::f32(0.3),						// radius--essentially, vent size
			initialDirection,					// set's initial velocity for each direction...
			500,								//particles per second (range)
			550,
			irr::video::SColor(0xFF1F1F1F),		// particle color (range)
			irr::video::SColor(0xFF1F1F1F),
			maxTime * 0.75, //distance * 75,						// lifetime of particles (range)
			maxTime, //distance * 100,
			2,									// variation on angle of emission
			irr::core::dimension2df(0.2f,0.2f), // particle size (range)
			irr::core::dimension2df(0.25f,0.25f));

		particleSystem->setEmitter(em);
		em->drop();

		irr::scene::IParticleAffector* paf = particleSystem->createGravityAffector(normalDirection * gravity);
		particleSystem->addAffector(paf);
		paf->drop();

		paf = particleSystem->createFadeOutParticleAffector(irr::video::SColor(0, 0, 0, 0), distance * 5);
		particleSystem->addAffector(paf);
		paf->drop();

		paf = particleSystem->createScaleParticleAffector(irr::core::dimension2df(0.4f, 0.4f));
		particleSystem->addAffector(paf);
		paf->drop();

		particleSystem->setPosition(position);
		particleSystem->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		particleSystem->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
		particleSystem->setMaterialTexture(0, particleTex);
		particleSystem->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
		// END SMOKE
	}


	VentParticles::~VentParticles()
	{
		if (particleSystem != nullptr)
		{
			particleSystem->remove();
		}
	}
}