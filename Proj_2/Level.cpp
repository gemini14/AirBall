#include "Level.h"


#include <algorithm>
#include <fstream>
#include <map>

#include <sstream>
#include <vector>

#include <boost/foreach.hpp>

#include "PhysicsManager.h"
#include "SoundSystem.h"
#include "VentParticles.h"


namespace Tuatara
{
	class PhysicsManager;

	// private implementation
	struct Level_ : boost::noncopyable
	{
		// Definitions ////
		struct VentInfo
		{
			float x;
			float y;
			float z;
			Direction direction;
			int strength;
			std::shared_ptr<VentParticles> particle;
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

		typedef std::map<std::string, std::string> SoundFilenameMap;

		// Variables
		VentVector vents;
		BuildingBlockMap levelBlocks;
		SoundFilenameMap soundFilenameMap;

		float entryX, entryY, entryZ;
		float exitX, exitY, exitZ;

		irr::scene::ILightSceneNode* light1;
		irr::scene::ILightSceneNode* light2;
		irr::scene::IAnimatedMeshSceneNode *ball;

		std::shared_ptr<SoundSystem> soundSystem;

		std::shared_ptr<PhysicsManager> physics;


		// Functions
		Level_();
		~Level_();

		bool InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
			irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex );
		bool StepSimulation( float timeDelta );

		bool LoadLevelData( irr::io::IFileSystem *fileSystem, const std::string& levelFile );

		void CreateBall( irr::scene::ISceneManager *smgr, irr::video::ITexture *ballTex );
		void CreateLights( irr::scene::ISceneManager* smgr);
		void CreateExit( irr::video::ITexture *exitTex );
		void CreatePhysicsBlocks();
		void CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall );
		void CreateVents( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
			irr::video::ITexture *ventTex );

		void AddPairToSoundFilenameMap( const std::string& key, const std::string& value );
		void ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z );
		Direction CalcDirection( const float& x, const float& y, const float& z );
		BuildingBlockMap::iterator FindBlock( const float& x, const float& y, const float& z );
		void RemoveBlock( float x, float y, float z );

		void Pause( bool pause );
	};


	Level_::Level_() : physics( new PhysicsManager ), ball( nullptr ), soundSystem( new SoundSystem ), light1( nullptr ), light2( nullptr )
	{
	}

	Level_::~Level_()
	{
		// memory cleanup
		BOOST_FOREACH( VentInfo *v, vents )
		{
			delete v;
		}

		std::for_each( levelBlocks.begin(), levelBlocks.end(), []( std::pair<NodePos, irr::scene::IMeshSceneNode*> p )
		{
			p.second->remove();
		});

		if( ball == nullptr )
		{
			ball->remove();
		}

		if ( light1 != nullptr )
		{
			light1->remove();
			light1 = nullptr;
		}
		if ( light2 != nullptr )
		{
			light2->remove();
			light2 = nullptr;
		}
	}

	bool Level_::InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
		irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
		irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex )
	{
		using namespace irr;
		using namespace std;

		// make sure level data was read in correctly
		// note that this part can be extended to read in other info like music location
		if( !LoadLevelData( fileSystem, levelFile ) )
		{
			return false;
		}

		CreateBall( smgr, ballTex );
		CreateLights( smgr );
		CreateRenderBlocks( smgr, wall );
		CreateVents( smgr, ventFXTex, ventTex );
		CreateExit( exitTex );
		// creating physics blocks is last because the exit has to be removed
		CreatePhysicsBlocks();

		if( !soundSystem->SoundSystemInitOK() )
		{
			return false;
		}

		soundSystem->CreateSounds( soundFilenameMap );
		soundSystem->StartPlayingLoopingSounds();

		return true;
	}

	bool Level_::StepSimulation( float timeDelta )
	{
		bool levelComplete = physics->StepSimulation( timeDelta );
		// set ball position:
		ball->setPosition( physics->GetBallPosition() );

		// set ball rotation (if there is a valid rotation)
		static irr::core::vector3df rotation;
		if( physics->GetBallRotation( rotation ) ) 
		{
			ball->setRotation( rotation );
		}

		soundSystem->Update();

		return levelComplete;
	}

	bool Level_::LoadLevelData( irr::io::IFileSystem *fileSystem, const std::string& levelFile )
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
				// get the entry position (where the ball starts) coordinates
				if( name == "entry\0" )
				{
					entryX = levelReader->getAttributeValueAsFloat( "x" );
					entryY = levelReader->getAttributeValueAsFloat( "y" );
					entryZ = levelReader->getAttributeValueAsFloat( "z" );
				}
				// get the exit position coordinates
				else if( name == "exit\0" )
				{
					exitX = levelReader->getAttributeValueAsFloat( "x" );
					exitY = levelReader->getAttributeValueAsFloat( "y" );
					exitZ = levelReader->getAttributeValueAsFloat( "z" );
				}
				// get the vent coordinates and attributes and add it to the vector
				else if( name == "vent\0" )
				{
					VentInfo *v = new VentInfo;
					v->x = levelReader->getAttributeValueAsFloat( "x" );
					v->y = levelReader->getAttributeValueAsFloat( "y" );
					v->z = levelReader->getAttributeValueAsFloat( "z" );
					v->strength = levelReader->getAttributeValueAsInt( "strength" );
					v->direction = CalcDirection( v->x, v->y, v->z );
					vents.push_back( v );
				}
				else if( name == "bgmusic\0" )
				{
					AddPairToSoundFilenameMap( "bgmusic", levelReader->getAttributeValueSafe( "file" ) );
				}
				else if( name == "collision\0" )
				{
					AddPairToSoundFilenameMap( "collision", levelReader->getAttributeValueSafe( "file" ) );
				}
				else if( name == "vent\0" )
				{
					AddPairToSoundFilenameMap( "vent", levelReader->getAttributeValueSafe( "file" ) );
				}
				else if( name == "jet\0" )
				{
					AddPairToSoundFilenameMap( "jet", levelReader->getAttributeValueSafe( "file" ) );
				}
			}
		}

		delete levelReader;
		return true;
	}

	void Level_::CreateBall( irr::scene::ISceneManager *smgr, irr::video::ITexture *ballTex )
	{
		using namespace irr;

		// create the scene node for the ball and set its properties
		scene::IAnimatedMesh* mesh;
		mesh = smgr->getMesh("ball250.x");
		ball = smgr->addAnimatedMeshSceneNode(mesh);
		ball->setPosition(core::vector3df( entryX, entryY, entryZ ));
		ball->addShadowVolumeSceneNode();
		smgr->setShadowColor(video::SColor(150,0,0,0));
		ball->setMaterialFlag( video::EMF_LIGHTING, true );
		ball->setMaterialTexture( 0, ballTex );
        // if we need to resize the ball, we can scall it with these two lines:
		//ball->setScale(core::vector3df(4,4,4));
        //ball->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);


		// create the ball's physical counterpart
		physics->CreateBall( entryX, entryY, entryZ );
	}

	void Level_::CreateLights( irr::scene::ISceneManager* smgr)
	{

		light1 = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, (float)levelSize, (float)levelSize / 2), irr::video::SColorf(255, 255, 255), (float)levelSize * 2);
		light2 = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, 0, (float)levelSize / 2), irr::video::SColorf(128, 128, 128), (float)levelSize * 2);
	}

	void Level_::CreateExit( irr::video::ITexture *exitTex )
	{
		// set the texture for the exit block so that the player can find it
		FindBlock( exitX, exitY, exitZ )->second->setMaterialTexture( 0, exitTex );
		physics->CreatePhantom( exitX, exitY, exitZ, CalcDirection( exitX, exitY, exitZ ), 1, false );
	}

	void Level_::CreatePhysicsBlocks()
	{
		// iterate through all the blocks and create their physical counterparts (skip the exit or the player 
		// can't get out)
		std::for_each( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)
		{
			if( pair.first.x == exitX && pair.first.y == exitY && pair.first.z == exitZ )
			{
				return;
			}
			physics->CreateBlock( pair.first.x, pair.first.y, pair.first.z );
		});
	}

	void Level_::CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall )
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
						// store the x,y,z positions
						nodePos.x = static_cast<float>(x);
						nodePos.y = static_cast<float>(cubeLevel);
						nodePos.z = static_cast<float>(z);

						// create the node in the scene and set its properties
						scene::IMeshSceneNode *node = smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, true );
						node->setMaterialTexture( 0, wall );

						// store the NodePos and ISceneNode
						levelBlocks.insert( make_pair( nodePos, node ) );
					}
				}
			}
		}
	}

	void Level_::CreateVents( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
		irr::video::ITexture *ventTex )
	{
		using namespace irr::core;

		// this lambda is used to create the normal vector to send to the particle system based on the
		// vent's direction
		auto normalGenerator = []( Direction d )->irr::core::vector3df
		{
			irr::core::vector3df normal;
			switch( d )
			{
			case FORWARD:
				normal.Z = 1.f;
				break;
			case BACKWARD:
				normal.Z = -1.f;
				break;
			case LEFT:
				normal.X = -1.f;
				break;
			case RIGHT:
				normal.X = 1.f;
				break;
			case UP:
				normal.Y = 1.f;
				break;
			case DOWN:
				normal.Y = -1.f;
				break;
			default:
				printf( "CreateVents: bad direction sent to normalGenerator lambda.\n" );
			}

			return normal.normalize();
		};

		BOOST_FOREACH( VentInfo *v, vents )
		{
			// for each vent, set the correct texture, create its physical counterpart, and add the
			// particle system
			FindBlock( v->x, v->y, v->z )->second->setMaterialTexture( 0, ventTex );
			physics->CreatePhantom( v->x, v->y, v->z, v->direction, v->strength );
			v->particle.reset( new VentParticles(smgr, particleTex, vector3df( v->x, v->y, v->z ), 
				normalGenerator( v->direction ), static_cast<float>(v->strength) ) );
		}
	}

	void Level_::AddPairToSoundFilenameMap( const std::string& key, const std::string& value )
	{
		using namespace std;

		if( !value.length() == 0 )
		{
			soundFilenameMap.insert( make_pair<string, string>( key, value ) );
		}
	}
	
	void Level_::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		physics->ApplyImpulseToBall( dir, x, y, z );
	}

	
	Direction Level_::CalcDirection( const float& x, const float& y, const float& z )
	{
		// calculate the direction of the vent based on it's position on the cube's wall
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

	Level_::BuildingBlockMap::iterator Level_::FindBlock( const float& x, const float& y, const float& z )
	{
		// return the iterator to the block entry that matches the x, y, z coordinates, if any
		return std::find_if( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)->bool
		{
			return pair.first.x == x && pair.first.y == y && pair.first.z == z;
		});
	}	

	void Level_::RemoveBlock( float x, float y, float z )
	{
		// remove block (and its entry in the map) if it matches the x, y, z coordinates
		auto iter = std::find_if( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)
		{
			return pair.first.x == x && pair.first.y == y && pair.first.z == z;
		});

		iter->second->remove();
		levelBlocks.erase( iter );
	}

	void Level_::Pause( bool pause )
	{
		if( pause )
		{
			soundSystem->PausePlayback();
		}
		else
		{
			soundSystem->ResumePlayback();
		}
	}

	//public implementation
	Level::Level() : level_( new Level_ )
	{
	}

	Level::~Level()
	{		
	}

	bool Level::InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
		irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
		irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex )
	{
		return level_->InitLevel( smgr, fileSystem, levelFile, wall, ballTex, exitTex, ventTex, ventFXTex );
	}

	void Level::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		level_->ApplyImpulseToBall( dir, x, y, z );
	}

	bool Level::StepSimulation( float timeDelta )
	{
		return level_->StepSimulation( timeDelta );
	}

	void Level::Pause( bool pause )
	{
		level_->Pause( pause );
	}
}