#include "Level.h"

namespace Tuatara
{
	Level::Level() : physics( new PhysicsManager )
	{
	}

	Level::~Level()
	{
		using namespace std;

		/*ball->drop();

		for_each( levelBlocks.begin(), levelBlocks.end(), [&](std::pair<std::string, irr::scene::IMeshSceneNode*> iter)
		{
			iter.second->drop();
		});*/
	}

	void Level::StepSimulation( float timeDelta )
	{
		physics->StepSimulation( timeDelta );
		ball->setPosition( physics->GetBallPosition() );
	}

	void Level::InitLevel( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall, irr::video::ITexture *ballTex )
	{
		using namespace irr;
		using namespace std;

		physics->CreateWorld();
		physics->CreateBall();

		scene::IMeshSceneNode * node = smgr->addSphereSceneNode( 0.5f, 64, 0, -1, physics->GetBallPosition() );
		node->setMaterialFlag( video::EMF_LIGHTING, false );
		node->setMaterialTexture( 0, ballTex );
		ball = node;

		for( int cubeLevel = 0; cubeLevel <= levelSize; ++cubeLevel )
		{
			for( int x = 0; x <= levelSize; ++x )
			{
				for( int z = 0; z <= levelSize; ++z )
				{
					if( // bottom or top level
						( cubeLevel == 0 || cubeLevel == levelSize ) 
						|| 
						// or one of the edges of the mid levels
						( cubeLevel != 0 && cubeLevel != levelSize && 
						// (this part of the test weeds out blocks in the center portion of the cube)
						( ( z == 0 || z == levelSize ) || ( z > 0 && z < levelSize && ( x == 0 || x == levelSize ) ) )
						) )
					{
						stringstream key;
						key << x << cubeLevel << z;
						
						scene::IMeshSceneNode *node = smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, false );
						node->setMaterialTexture( 0, wall );
						
						levelBlocks.insert( make_pair( key.str(), node ) );
					}
				}
			}
		}
	}
}