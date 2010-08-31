#include "Level.h"

namespace Tuatara
{
	Level::Level() : physics( new PhysicsManager )
	{
	}

	Level::~Level()
	{
	}

	void Level::StepSimulation( float timeDelta )
	{
		physics->StepSimulation( timeDelta );
		// set ball position:
		ball->setPosition( physics->GetBallPosition() );

		// set ball rotation:
		static irr::core::vector3df rotation;
		if( physics->GetBallRotation( rotation ) ) 
		{
			ball->setRotation( rotation );
		}
	}

	bool Level::InitLevel( Game &game, irr::io::IFileSystem *fileSystem, std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex )
	{
		using namespace irr;
		using namespace std;

		physics->CreateWorld();
		physics->CreateBall();

		scene::IMeshSceneNode * node = game.manager->smgr->addSphereSceneNode( 0.5f, 64, 0, -1, physics->GetBallPosition() );
		node->setMaterialFlag( video::EMF_LIGHTING, false );
		node->setMaterialTexture( 0, ballTex );
		ball = node;

		// TODO: Temporarily put in 5 vents. This should change to work through level loading...
		for (int i = 0; i < 5; i++)
		{
			ventRenderers[0].Start(game, irr::core::vector3df( levelSize - 1.5f - (float)i, 1.f, levelSize - 1.5f ), irr::core::vector3df(0.0f,1.0f,0.0f), (float)i + 1.f);
		}

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

						scene::IMeshSceneNode *node = game.manager->smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, false );
						node->setMaterialTexture( 0, wall );

						levelBlocks.insert( make_pair( key.str(), node ) );
					}
				}
			}
		}

		auto *levelReader = fileSystem->createXMLReaderUTF8( levelFile.c_str() );
		if( levelReader == nullptr )
		{
			return false;
		}

		while( levelReader->read() )
		{
			if( levelReader->getNodeType() == io::EXN_ELEMENT )
			{
				std::string name( levelReader->getNodeName() );
				if( name == "entry\0" )
				{
					entryX = levelReader->getAttributeValueAsFloat( "x" );
					entryY = levelReader->getAttributeValueAsFloat( "y" );
					entryZ = levelReader->getAttributeValueAsFloat( "z" );
				}
				else if( name == "exit\0" )
				{
					exitX = levelReader->getAttributeValueAsFloat( "x" );
					exitY = levelReader->getAttributeValueAsFloat( "y" );
					exitZ = levelReader->getAttributeValueAsFloat( "z" );
				}
				else if( name == "vent\0" )
				{
					VentInfo v;
					v.face = levelReader->getAttributeValueAsInt( "face" );
					v.strength = levelReader->getAttributeValueAsInt( "strength" );
					v.x = levelReader->getAttributeValueAsFloat( "x" );
					v.y = levelReader->getAttributeValueAsFloat( "y" );
					vents.push_back( v );
				}
			}
		}

		delete levelReader;
		return true;
	}
}