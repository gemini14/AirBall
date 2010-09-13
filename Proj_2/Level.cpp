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
		BuildingBlockMap levelTransBlocks;
		SoundFilenameMap soundFilenameMap;
		irr::scene::ISceneNode *TopWall, *BottomWall, *LeftWall, *RightWall, *FrontWall, *BackWall;

		float entryX, entryY, entryZ;
		float exitX, exitY, exitZ;

		irr::scene::ICameraSceneNode *camera;
		irr::scene::ILightSceneNode* lightDir;
		irr::scene::ILightSceneNode* lightAmb;
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
		void CreateCamera( irr::scene::ISceneManager *smgr );
		void CreateLights( irr::scene::ISceneManager* smgr );
		void CreateWallNodes( irr::scene::ISceneManager* smgr );
		void CreateExit( irr::video::ITexture *exitTex );
		void CreatePhysicsBlocks();
		void CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall );
		void CreateVents( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
			irr::video::ITexture *ventTex );
		void CreateVentSounds();

		void AddPairToSoundFilenameMap( const std::string& key, const std::string& value );
		void ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z );
		void ApplyDirectionToCamera( Direction dir );
		Direction CalcDirection( const float& x, const float& y, const float& z );
		BuildingBlockMap::iterator FindBlock( const float& x, const float& y, const float& z );
		void RemoveBlock( float x, float y, float z );
		irr::scene::ISceneNode* GetParentWall( float x, float y, float z);

		void Pause( bool pause );
		void PlayJetSound();

		const irr::core::vector3df GetNewCameraTarget( const irr::core::vector3df& currentPos,
			const irr::core::vector3df& target, const Direction& dir );
		const irr::core::vector3df GetCameraLookVector();
		const irr::core::vector3df GetCameraUpVector();
	};


	Level_::Level_() : physics( new PhysicsManager ), ball( nullptr ), soundSystem( new SoundSystem ), lightDir( nullptr ), lightAmb( nullptr ),
		camera( nullptr ), TopWall( nullptr ), BottomWall( nullptr ), LeftWall( nullptr ), RightWall( nullptr ), FrontWall( nullptr ), BackWall( nullptr )
	{
	}

	Level_::~Level_()
	{
		// to prevent user from hearing sounds as level is deleted
		soundSystem->PausePlayback();

		// memory cleanup
		BOOST_FOREACH( VentInfo *v, vents )
		{
			delete v;
		}

		std::for_each( levelBlocks.begin(), levelBlocks.end(), []( std::pair<NodePos, irr::scene::IMeshSceneNode*> p )
		{
			p.second->remove();
		});
		levelBlocks.clear();

		std::for_each( levelTransBlocks.begin(), levelTransBlocks.end(), []( std::pair<NodePos, irr::scene::IMeshSceneNode*> p )
		{
			p.second->remove();
		});
		levelTransBlocks.clear();

		auto safeDelete = []( irr::scene::ISceneNode *node )
		{
			if( node != nullptr )
			{
				node->remove();
			}
		};

		safeDelete( ball );
		safeDelete( lightDir );
		safeDelete( lightAmb );
		safeDelete( TopWall );
		safeDelete( BottomWall );
		safeDelete( LeftWall );
		safeDelete( RightWall );
		safeDelete( FrontWall );
		safeDelete( BackWall );
		safeDelete( camera );
	}

	bool Level_::InitLevel( irr::scene::ISceneManager *smgr, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
		irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
		irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex )
	{
		using namespace irr;
		using namespace std;

		if( !soundSystem->SoundSystemInitOK() )
		{
			return false;
		}

		// make sure level data was read in correctly
		// note that this part can be extended to read in other info like music location
		if( !LoadLevelData( fileSystem, levelFile ) )
		{
			return false;
		}

		CreateCamera( smgr );
		CreateWallNodes( smgr );
		CreateBall( smgr, ballTex );
		CreateLights( smgr );
		CreateRenderBlocks( smgr, wall );
		CreateVents( smgr, ventFXTex, ventTex );
		CreateExit( exitTex );
		// creating physics blocks is last because the exit has to be removed
		CreatePhysicsBlocks();

		soundSystem->CreateSounds( soundFilenameMap );
		CreateVentSounds();
		soundSystem->StartPlayingLoopingSounds();

		return true;
	}

	bool Level_::StepSimulation( float timeDelta )
	{
		using namespace irr::core;
		bool levelComplete = physics->StepSimulation( timeDelta );
		
		static vector3df lastpos = ball->getPosition();
		// set ball position:
		vector3df position = physics->GetBallPosition();
		ball->setPosition( position );

		// set ball rotation (if there is a valid rotation)
		static vector3df rotation;
		if( physics->GetBallRotation( rotation ) ) 
		{
			ball->setRotation( rotation );
		}

		camera->setTarget( position );

		vector3df soundSysVelocity = ( position - lastpos ) / ( 1.f / 60.f );
		vector3df ballVelocity = physics->GetBallVelocity().normalize();
		
		soundSystem->Update( position.X, position.Y, position.Z,			// position
			soundSysVelocity.X, soundSysVelocity.Y, soundSysVelocity.Z,		// velocity
			ballVelocity.X, ballVelocity.Y, ballVelocity.Z );				// forward vector

		lastpos = position;

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
				else if( name == "collisionSound\0" )
				{
					AddPairToSoundFilenameMap( "collision", levelReader->getAttributeValueSafe( "file" ) );
				}
				else if( name == "ventSound\0" )
				{
					AddPairToSoundFilenameMap( "vent", levelReader->getAttributeValueSafe( "file" ) );
				}
				else if( name == "jetSound\0" )
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
        // if we need to resize the ball, we can scale it with these two lines:
		//ball->setScale(core::vector3df(4,4,4));
        //ball->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);


		// create the ball's physical counterpart
		physics->CreateBall( entryX, entryY, entryZ );
	}

	void Level_::CreateLights( irr::scene::ISceneManager* smgr )
	{
		// Create directional light, for shadow:
		// TODO: Get this to work... it's not right now, because the position part doesn't seem to be working...
		//lightDir = smgr->addLightSceneNode(0, irr::core::vector3df(0, 1, 0), irr::video::SColorf(1, 1, 1), levelSize * 2);
		//lightDir->setLightType( irr::video::ELT_DIRECTIONAL );
		//lightDir->setPosition(irr::core::vector3df(0, 1, 0));
		lightDir = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, (float)levelSize * 5, (float)levelSize / 2), irr::video::SColorf(255, 255, 255), (float)levelSize * 5);

		// Create an ambient light, so everything's not quite so dark...
		lightAmb = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, 1, (float)levelSize / 2), irr::video::SColorf(128, 128, 128), (float)levelSize * 2);
		lightAmb->enableCastShadow(false);
	}

	void Level_::CreateCamera( irr::scene::ISceneManager* smgr )
	{
		// add camera
		camera = smgr->addCameraSceneNode( 0, 
			irr::core::vector3df( (float)levelSize / 2, (float)levelSize / 2, -8.f ), 
			irr::core::vector3df( (float)levelSize / 2, (float)levelSize / 2, levelSize - 1 ) );
	}

	void Level_::CreateWallNodes( irr::scene::ISceneManager* smgr )
	{
		// These are used as parents, so we can selectively turn them on/off:
		TopWall = smgr->addEmptySceneNode();
		BottomWall = smgr->addEmptySceneNode();
		LeftWall = smgr->addEmptySceneNode();
		RightWall = smgr->addEmptySceneNode();
		FrontWall = smgr->addEmptySceneNode();
		BackWall = smgr->addEmptySceneNode();
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
						scene::ISceneNode* parent;
						parent = GetParentWall(x, cubeLevel, z);

						NodePos nodePos;
						// store the x,y,z positions
						nodePos.x = static_cast<float>(x);
						nodePos.y = static_cast<float>(cubeLevel);
						nodePos.z = static_cast<float>(z);

						// create the node in the scene and set its properties
						scene::IMeshSceneNode *node = smgr->addCubeSceneNode( 1.f, parent, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, false );
						node->setMaterialTexture( 0, wall );

						// store the NodePos and ISceneNode
						levelBlocks.insert( make_pair( nodePos, node ) );

						scene::IMeshSceneNode *transNode = smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						scene::IMesh *mesh = transNode->getMesh();
						scene::IMeshManipulator* man = smgr->getMeshManipulator();
						man->setVertexColorAlpha(mesh, 0.8f);
						transNode->setMaterialFlag( video::EMF_LIGHTING, false );
						//transNode->setMaterialFlag( video::EMF_WIREFRAME, true );
						transNode->setMaterialTexture( 0, wall );
						transNode->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

						levelTransBlocks.insert( make_pair( nodePos, transNode ) );
					}
				}
			}
		}
		FrontWall->setVisible(false);
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

	void Level_::CreateVentSounds()
	{
		std::for_each( vents.begin(), vents.end(), [&]( VentInfo *v )
		{
			soundSystem->CreateVentSound( v->x, v->y, v->z );
		});
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

	void Level_::ApplyDirectionToCamera( Direction dir )
	{
		// OLD Functionality:
		//camera->setTarget( GetNewCameraTarget( camera->getAbsolutePosition(),
		//	camera->getTarget(), dir ) );

		// New idea:
		irr::core::vector3df pos = camera->getPosition();
		switch (dir)
		{
		case RIGHT:
			pos.rotateXZBy( 10, irr::core::vector3df( (float)levelSize / 2, pos.Y, (float)levelSize / 2 ) );
			break;
		case LEFT:
			pos.rotateXZBy( -10, irr::core::vector3df( (float)levelSize / 2, pos.Y, (float)levelSize / 2 ) );
			break;
		case UP:
			if (pos.Y < levelSize + 3 ) pos.Y  = pos.Y + 1;
			break;
		case DOWN:
			if (pos.Y > -3 ) pos.Y  = pos.Y - 1;
			break;
		}
		camera->setPosition( pos );

		if ( pos.X < 0 ) LeftWall->setVisible(false); else LeftWall->setVisible(true);
		if ( pos.Y < 0 ) BottomWall->setVisible(false); else BottomWall->setVisible(true);
		if ( pos.Z < 0 ) FrontWall->setVisible(false); else FrontWall->setVisible(true);
		if ( pos.X > levelSize ) RightWall->setVisible(false); else RightWall->setVisible(true);
		if ( pos.Y > levelSize ) TopWall->setVisible(false); else TopWall->setVisible(true);
		if ( pos.Z > levelSize ) BackWall->setVisible(false); else BackWall->setVisible(true);
	}

	const irr::core::vector3df Level_::GetNewCameraTarget( const irr::core::vector3df& currentPos, 
		const irr::core::vector3df& target, const Direction& dir )
	{
		using namespace irr;
		auto newPos = currentPos;
		auto newTarget = target;

		switch( dir )
		{
		case UP:
			if( newTarget.Y < levelSize - 2 )
			{
				newTarget.Y += 0.5f;
			}
			return newTarget;

		case DOWN:
			if( newTarget.Y > -2 )
			{
				newTarget.Y -= 0.5f;
			}
			return newTarget;

		case LEFT:
			newTarget.rotateXZBy( 5.f, currentPos );
			if( newTarget.X < 3 )
			{
				newTarget.X = 2;
			}
			return newTarget;

		case RIGHT:
			newTarget.rotateXZBy( -5.f, currentPos );
			if( newTarget.Z < 3 )
			{
				newTarget.Z = 2;
			}
			return newTarget;

		default:
			return core::vector3df();
		}

		return core::vector3df();
	}

	const irr::core::vector3df Level_::GetCameraLookVector()
	{
		irr::core::vector3df look( camera->getTarget() );
		return look.normalize();
	}

	const irr::core::vector3df Level_::GetCameraUpVector()
	{
		irr::core::vector3df up( camera->getUpVector() );
		return up.normalize();
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

	irr::scene::ISceneNode* Level_::GetParentWall(float x, float y, float z)
	{
		irr::scene::ISceneNode* parent = 0;
		if (z == 0)
		{
			parent = FrontWall;
		}
		else if (x == levelSize)
		{
			parent = RightWall;
		}
		else if (x == 0)
		{
			parent = LeftWall;
		}
		else if (z == levelSize)
		{
			parent = BackWall;
		}
		else if (y == levelSize)
		{
			parent = TopWall;
		}
		else if (y == 0)
		{
			parent = BottomWall;
		}
		return parent;
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

	void Level_::PlayJetSound()
	{
		soundSystem->PlayJetSound();
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

	void Level::ApplyDirectionToCamera( Direction dir )
	{
		level_->ApplyDirectionToCamera( dir );
	}

	bool Level::StepSimulation( float timeDelta )
	{
		return level_->StepSimulation( timeDelta );
	}

	void Level::Pause( bool pause )
	{
		level_->Pause( pause );
	}

	void Level::PlayJetSound()
	{
		level_->PlayJetSound();
	}
}