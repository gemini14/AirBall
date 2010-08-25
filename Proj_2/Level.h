#ifndef LEVEL_H
#define LEVEL_H


#include <algorithm>
#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>
#include <map>
#include <string>
#include <sstream>

#include "PhysicsManager.h"


namespace Tuatara
{
	class PhysicsManager;

	class Level : boost::noncopyable
	{
	private:

		typedef std::map<std::string, irr::scene::IMeshSceneNode*> BuildingBlockMap;

		irr::scene::IMeshSceneNode *ball;
		BuildingBlockMap levelBlocks;

	public:
		std::shared_ptr<PhysicsManager> physics;
		Level();
		~Level();

		void InitLevel( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall, irr::video::ITexture *ballTex );
		void StepSimulation( float timeDelta = 0 );
	};

}

#endif