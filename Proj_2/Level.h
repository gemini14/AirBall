#ifndef LEVEL_H
#define LEVEL_H


#include <algorithm>
#include <boost/noncopyable.hpp>
#include <fstream>
#include <irrlicht.h>
#include <memory>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "PhysicsManager.h"


namespace Tuatara
{
	class PhysicsManager;

	class Level : boost::noncopyable
	{
	private:
		struct VentInfo
		{
			int face;
			float x;
			float y;
			int strength;
		};

		typedef std::vector<VentInfo> VentVector;
		typedef std::map<std::string, irr::scene::IMeshSceneNode*> BuildingBlockMap;

		VentVector vents;
		float entryX, entryY, entryZ;
		float exitX, exitY, exitZ;

		irr::scene::IMeshSceneNode *ball;
		BuildingBlockMap levelBlocks;

	public:
		std::shared_ptr<PhysicsManager> physics;
		Level();
		~Level();

		bool InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex );
		void StepSimulation( float timeDelta = 0 );
	};

}

#endif