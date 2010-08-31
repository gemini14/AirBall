#include "Level.h"

namespace Tuatara
{
	Level::Level() : physics( new PhysicsManager )
	{
	}

	Level::~Level()
	{
		BOOST_FOREACH( VentInfo *v, vents )
		{
			delete v;
		}
	}

	bool Level::StepSimulation( float timeDelta )
	{
		bool levelComplete = physics->StepSimulation( timeDelta );
		// set ball position:
		ball->setPosition( physics->GetBallPosition() );

		// set ball rotation:
		static irr::core::vector3df rotation;
		if( physics->GetBallRotation( rotation ) ) 
		{
			ball->setRotation( rotation );
		}

		return levelComplete;
	}

	bool Level::LoadLevelData( irr::io::IFileSystem *fileSystem, std::string& levelFile )
	{
		using namespace irr;
		using namespace std;

		auto *levelReader = fileSystem->createXMLReaderUTF8( levelFile.c_str() );
		if( levelReader == nullptr )
		{
			return false;
		}

		while( levelReader->read() )
		{
			if( levelReader->getNodeType() == io::EXN_ELEMENT )
			{
				string name( levelReader->getNodeName() );
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
					VentInfo *v = new VentInfo;
					v->x = levelReader->getAttributeValueAsFloat( "x" );
					v->y = levelReader->getAttributeValueAsFloat( "y" );
					v->z = levelReader->getAttributeValueAsFloat( "z" );
					v->strength = levelReader->getAttributeValueAsInt( "strength" );
					v->direction = CalcDirection( v->x, v->y, v->z );
					/*if( v->z == 0 )
					{
						v->direction = FORWARD;
					}
					else if( v->z == levelSize )
					{
						v->direction = BACKWARD;
					}
					else if( v->x == 0 && ( v->z != 0 && v->z != levelSize ) )
					{
						v->direction == RIGHT;
					}
					else if( v->x == levelSize && ( v->z != 0 && v->z != levelSize ) )
					{
						v->direction = LEFT;
					}
					else if( v->y == levelSize )
					{
						v->direction = DOWN;
					}
					else if( v->y == 0 )
					{
						v->direction = UP;
					}*/
					vents.push_back( v );
				}
			}
		}

		delete levelReader;
		return true;
	}

	Direction Level::CalcDirection( const float& x, const float& y, const float& z )
	{
		if( z == 0 )
		{
			return FORWARD;
		}
		else if( z == levelSize )
		{
			return BACKWARD;
		}
		else if( x == 0 && ( z != 0 && z != levelSize ) )
		{
			return RIGHT;
		}
		else if( x == levelSize && ( z != 0 && z != levelSize ) )
		{
			return LEFT;
		}
		else if( y == levelSize )
		{
			return DOWN;
		}
		else if( y == 0 )
		{
			return UP;
		}

		return NONE;
	}

	void Level::CreateVents()
	{
		BOOST_FOREACH( VentInfo *v, vents )
		{
			RemoveBlock( v->x, v->y, v->z );
			physics->CreatePhantom( v->x, v->y, v->z, v->direction, v->strength );
		}
	}

	void Level::CreateExit()
	{
		RemoveBlock( exitX, exitY, exitZ );
		//physics->CreatePhantom( exitX, exitY, exitZ, CalcDirection( exitX, exitY, exitZ ), 1, false );
	}

	void Level::CreateBall( irr::scene::ISceneManager *smgr, irr::video::ITexture *ballTex )
	{
		using namespace irr;

		ball = smgr->addSphereSceneNode( /*0.5*/0.125f, 64, 0, -1, core::vector3df( entryX, entryY, entryZ ) );
		ball->setMaterialFlag( video::EMF_LIGHTING, false );
		ball->setMaterialTexture( 0, ballTex );
		
		physics->CreateBall( entryX, entryY, entryZ );
	}

	void Level::CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall )
	{
		using namespace irr;
		using namespace std;

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
						NodePos nodePos;
						nodePos.x = static_cast<float>(x);
						nodePos.y = static_cast<float>(cubeLevel);
						nodePos.z = static_cast<float>(z);

						scene::IMeshSceneNode *node = smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, false );
						node->setMaterialTexture( 0, wall );

						levelBlocks.insert( make_pair( nodePos, node ) );
					}
				}
			}
		}
	}

	void Level::RemoveBlock( float x, float y, float z )
	{
		auto iter = std::find_if( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)
		{
			return pair.first.x == x && pair.first.y == y && pair.first.z == z;
		});

		iter->second->remove();
		levelBlocks.erase( iter );
	}

	void Level::CreatePhysicsBlocks()
	{
		std::for_each( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)
		{
			physics->CreateBlock( pair.first.x, pair.first.y, pair.first.z );
		});
	}

	bool Level::InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex )
	{
		using namespace irr;
		using namespace std;

		if( !LoadLevelData( fileSystem, levelFile ) )
		{
			return false;
		}

		CreateBall( smgr, ballTex );
		CreateRenderBlocks( smgr, wall );
		CreateVents();
		CreateExit();
		CreatePhysicsBlocks();

		return true;
	}
}