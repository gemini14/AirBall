#ifndef LEVEL_H
#define LEVEL_H


#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <fstream>
#include <irrlicht.h>
#include <memory>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "PhysicsManager.h"
#include "VentParticles.h"


namespace Tuatara
{
	class PhysicsManager;

	class Level : boost::noncopyable
	{
	private:
		// Definitions ////
		struct VentInfo
		{
			float x;
			float y;
			float z;
			Direction direction;
			int strength;
		};

		typedef std::vector<VentInfo*> VentVector;

		struct NodePos
		{
			float x;
			float y;
			float z;
		};

		struct NodePosCriterion
		{
			bool operator()( const NodePos& lhs, const NodePos& rhs ) const
			{
				return lhs.x < rhs.x;
			}
		};
		typedef std::multimap<NodePos, irr::scene::IMeshSceneNode*, NodePosCriterion> BuildingBlockMap;
		// End Definitions ////

		// Declarations
		VentVector vents;
		BuildingBlockMap levelBlocks;

		float entryX, entryY, entryZ;
		float exitX, exitY, exitZ;

		irr::scene::IMeshSceneNode *ball;

		bool LoadLevelData( irr::io::IFileSystem *fileSystem, std::string& levelFile );
		void CreateBall( irr::scene::ISceneManager *smgr, irr::video::ITexture *ballTex );
		void CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall );
		Direction CalcDirection( const float& x, const float& y, const float& z );
		void CreatePhysicsBlocks();
		void RemoveBlock( float x, float y, float z );
		void CreateVents();
		void CreateExit();

	public:
		std::shared_ptr<PhysicsManager> physics;
		Level();
		~Level();

		bool InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex );
		bool StepSimulation( float timeDelta = 0 );
	};

}

#endif