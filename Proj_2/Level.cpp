#include "Level.h"


#include <algorithm>
#include <fstream>
#include <map>

#include <sstream>
#include <vector>

#include <boost/foreach.hpp>

#include "EnumsConstants.h"
#include "Game.h"
#include "HelpText.h"
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
		typedef std::multimap<NodePos, irr::scene::IMeshSceneNode*, NodePosCriterion> BuildingBlockMap;
		typedef std::map<std::string, std::string> SoundFilenameMap;

		// Variables
		BuildingBlockMap levelBlocks;
		BuildingBlockMap levelTransBlocks;
		SoundFilenameMap soundFilenameMap;
		VentVector vents;

		irr::scene::ISceneNode *TopWall, *BottomWall, *LeftWall, *RightWall, *FrontWall, *BackWall;
		irr::scene::ICameraSceneNode *camera;
		irr::scene::ILightSceneNode* lightDir;
		irr::scene::ILightSceneNode* lightAmb;
		irr::scene::IAnimatedMeshSceneNode *ball;

		std::shared_ptr<HelpText> helpText;
		std::shared_ptr<SoundSystem> soundSystem;
		std::shared_ptr<PhysicsManager> physics;

		float entryX, entryY, entryZ;
		float exitX, exitY, exitZ;
		float cameraRotation;

		// Functions
		Level_();
		~Level_();

		void AddPairToSoundFilenameMap( const std::string& key, const std::string& value );
		void AdjustWallVisibility( const irr::core::vector3df& cameraPos );
		void ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z );
		void ApplyDirectionToCamera( Direction dir );
		Direction CalcDirection( const float& x, const float& y, const float& z ) const;
		void CreateBall( irr::scene::ISceneManager *smgr, irr::video::ITexture *ballTex );
		void CreateCamera( irr::scene::ISceneManager *smgr );
		void CreateExit( irr::video::ITexture *exitTex );
		void CreateLights( irr::scene::ISceneManager* smgr );
		void CreatePhysicsBlocks();
		void CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall, irr::video::ITexture *transTex );
		void CreateVents( irr::scene::ISceneManager *smgr, irr::video::ITexture *particleTex,
			irr::video::ITexture *ventTex );
		void CreateVentSounds();
		void CreateWallNodes( irr::scene::ISceneManager* smgr );
		BuildingBlockMap::iterator FindBlock( const float& x, const float& y, const float& z );
		const irr::core::vector3df GetCameraLookVector() const;
		const irr::core::vector3df GetCameraUpVector() const;
		const irr::core::vector3df GetNewCameraPosition( const irr::core::vector3df& currentPos,
			const Direction& dir );
		irr::scene::ISceneNode* GetParentWall( const float& x, const float& y, const float& z );
		bool InitLevel( Game *game, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
			irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
			irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex, irr::video::ITexture *transTex );
		bool LoadLevelData( irr::io::IFileSystem *fileSystem, const std::string& levelFile, irr::gui::IGUIEnvironment *guienv );
		void Pause( bool pause );
		void PlayJetSound();
		void RemoveBlock( float x, float y, float z );
		bool StepSimulation( float timeDelta );
	};


	Level_::Level_() 
		: physics( new PhysicsManager ),
		ball( nullptr ),
		soundSystem( new SoundSystem ),
		lightDir( nullptr ),
		lightAmb( nullptr ),
		camera( nullptr ),
		TopWall( nullptr ),
		BottomWall( nullptr ),
		LeftWall( nullptr ),
		RightWall( nullptr ),
		FrontWall( nullptr ),
		BackWall( nullptr ),
		cameraRotation( 0.0f )
	{
	}

	Level_::~Level_()
	{
		// to prevent user from hearing sounds as level is deleted
		soundSystem->PausePlayback();

		// clean up vent information
		BOOST_FOREACH( VentInfo *v, vents )
		{
			delete v;
		}

		// clean up level blocks
		std::for_each( levelBlocks.begin(), levelBlocks.end(), []( std::pair<NodePos, irr::scene::IMeshSceneNode*> p )
		{
			p.second->remove();
		});

		// clean up transparent level blocks
		std::for_each( levelTransBlocks.begin(), levelTransBlocks.end(), []( std::pair<NodePos, irr::scene::IMeshSceneNode*> p )
		{
			p.second->remove();
		});

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

	void Level_::AddPairToSoundFilenameMap( const std::string& key, const std::string& value )
	{
		using namespace std;

		// if a filename was indeed supplied, store it (to eventually be sent to the sound system for
		// sound creation)
		if( !value.length() == 0 )
		{
			soundFilenameMap.insert( make_pair<string, string>( key, value ) );
		}
	}

	void Level_::AdjustWallVisibility( const irr::core::vector3df& cameraPos )
	{
		// axisValue is the desired camera position vector component, zeroAxisValue is true if testing for the <0 cases
		// and false otherwise, wall is the scene node whose visibility is being adjusted
		auto setVisibility = [&]( float axisValue, bool zeroAxisValue, irr::scene::ISceneNode *wall )
		{
			if( (zeroAxisValue && axisValue < 0) || (!zeroAxisValue && axisValue > levelSize) )
			{
				wall->setVisible( false );
			}
			else
			{
				wall->setVisible( true );
			}
		};

		setVisibility( cameraPos.X, true, LeftWall );
		setVisibility( cameraPos.Y, true, BottomWall );
		setVisibility( cameraPos.Z, true, FrontWall );
		setVisibility( cameraPos.X, false, RightWall );
		setVisibility( cameraPos.Y, false, TopWall );
		setVisibility( cameraPos.Z, false, BackWall );
	}

	void Level_::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		if( dir == UP )
		{
			physics->ApplyImpulseToBall( dir, x, y, z );
		}
		else
		{
			switch( dir )
			{
			case FORWARD:
				{
					auto forwardVec = irr::core::vector3df( 0.f, 0.f, IMPULSE_VALUE );
					forwardVec.rotateXZBy( cameraRotation );
					physics->ApplyImpulseToBall( dir, forwardVec.X, 0, forwardVec.Z );
				}
				break;
			case BACKWARD:
				{
					auto backwardVec = irr::core::vector3df( 0.f, 0.f, -IMPULSE_VALUE );
					backwardVec.rotateXZBy( cameraRotation );
					physics->ApplyImpulseToBall( dir, backwardVec.X, 0, backwardVec.Z );
				}
				break;
			case LEFT:
				{
					auto leftVec = irr::core::vector3df( -IMPULSE_VALUE, 0.f, 0.f );
					leftVec.rotateXZBy( cameraRotation );
					physics->ApplyImpulseToBall( dir, leftVec.X, 0, leftVec.Z );
				}
				break;
			case RIGHT:
				{
					auto rightVec = irr::core::vector3df( IMPULSE_VALUE, 0.f, 0.f );
					rightVec.rotateXZBy( cameraRotation );
					physics->ApplyImpulseToBall( dir, rightVec.X, 0, rightVec.Z );
				}
				break;
			default:
#if defined(_DEBUG) | defined(DEBUG)
				printf( "Bad direction sent to ApplyImpulseToBall in Level_\n" );
#endif
			}
		}		
	}

	void Level_::ApplyDirectionToCamera( Direction dir )
	{
		using namespace irr::core;

		// adjust camera position based on direction
		camera->setPosition( GetNewCameraPosition( camera->getPosition(), dir ) );

		AdjustWallVisibility( camera->getPosition() );
	}

	Direction Level_::CalcDirection( const float& x, const float& y, const float& z ) const
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
		physics->CreateBall( entryX, entryY, entryZ, soundSystem.get() );
	}

	void Level_::CreateCamera( irr::scene::ISceneManager* smgr )
	{
		// add camera
		camera = smgr->addCameraSceneNode( 0, 
			irr::core::vector3df( (float)levelSize / 2, (float)levelSize / 2, -8.f ), 
			irr::core::vector3df( (float)levelSize / 2, (float)levelSize / 2, levelSize - 1 ) );
	}

	void Level_::CreateExit( irr::video::ITexture *exitTex )
	{
		// set the texture for the exit block so that the player can find it
		FindBlock( exitX, exitY, exitZ )->second->setMaterialTexture( 0, exitTex );
		physics->CreatePhantom( exitX, exitY, exitZ, CalcDirection( exitX, exitY, exitZ ), 1, false );
	}

	void Level_::CreateLights( irr::scene::ISceneManager* smgr )
	{
		// Create directional light, for shadow:
		lightDir = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, (float)levelSize * 5, (float)levelSize / 2), irr::video::SColorf(255, 255, 255), (float)levelSize * 5);

		// Create an ambient light, so everything's not quite so dark...
		lightAmb = smgr->addLightSceneNode(0, irr::core::vector3df((float)levelSize / 2, 1, (float)levelSize / 2), irr::video::SColorf(128, 128, 128), (float)levelSize * 2);
		lightAmb->enableCastShadow(false);
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

	void Level_::CreateRenderBlocks( irr::scene::ISceneManager *smgr, irr::video::ITexture *wall, irr::video::ITexture *transTex )
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
						scene::ISceneNode* parent;
						parent = GetParentWall( static_cast<float>(x), static_cast<float>(cubeLevel), static_cast<float>(z) );
						scene::IMeshSceneNode *node = smgr->addCubeSceneNode( BLOCK_HALF_EXTENT * 2, parent, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						node->setMaterialFlag( video::EMF_LIGHTING, false );
						node->setMaterialTexture( 0, wall );

						// store the NodePos and ISceneNode
						levelBlocks.insert( make_pair( nodePos, node ) );

						// create transparent node in scene
						scene::IMeshSceneNode *transNode = smgr->addCubeSceneNode( 1.f, 0, -1, 
							core::vector3df( static_cast<float>(x), static_cast<float>(cubeLevel),
							static_cast<float>(z) ) );
						transNode->setMaterialFlag( video::EMF_LIGHTING, true );
						transNode->setMaterialTexture( 0, transTex );
						transNode->setMaterialType( video::EMT_TRANSPARENT_ALPHA_CHANNEL );

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
#if defined(_DEBUG) | defined(DEBUG)
				printf( "CreateVents: bad direction sent to normalGenerator lambda.\n" );
#endif
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
		// go through all the vents and create a sound for it in 3D space
		std::for_each( vents.begin(), vents.end(), [&]( VentInfo *v )
		{
			soundSystem->CreateVentSound( v->x, v->y, v->z );
		});
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

	Level_::BuildingBlockMap::iterator Level_::FindBlock( const float& x, const float& y, const float& z )
	{
		// return the iterator to the block entry that matches the x, y, z coordinates, if any
		return std::find_if( levelBlocks.begin(), levelBlocks.end(), 
			[=](std::pair<NodePos, irr::scene::IMeshSceneNode*> pair)->bool
		{
			return pair.first.x == x && pair.first.y == y && pair.first.z == z;
		});
	}	

	const irr::core::vector3df Level_::GetCameraLookVector() const
	{
		auto target( camera->getTarget() );
		return target.normalize();
	}

	const irr::core::vector3df Level_::GetCameraUpVector() const
	{
		auto up( camera->getUpVector() );
		return up.normalize();
	}

	const irr::core::vector3df Level_::GetNewCameraPosition( const irr::core::vector3df& currentPos, 
		const Direction& dir )
	{
		using namespace irr::core;
		auto pos = currentPos;

		// rotate or tilt up/down based on direction
		switch (dir)
		{
		case RIGHT:
			pos.rotateXZBy( 10, vector3df( static_cast<float>(levelSize / 2), pos.Y, static_cast<float>(levelSize / 2) ) );
			cameraRotation += 10.f;
			break;
		case LEFT:
			pos.rotateXZBy( -10, vector3df( static_cast<float>(levelSize / 2), pos.Y, static_cast<float>(levelSize / 2) ) );
			cameraRotation -= 10.f;
			break;
		case UP:
			if (pos.Y < levelSize + 3 )
			{
				++pos.Y;
			}
			break;
		case DOWN:
			if (pos.Y > -3 )
			{
				--pos.Y;
			}
			break;
		default:

#if defined(_DEBUG) | defined(DEBUG)
			printf( "Bad camera direction sent to ApplyDirectionToCamera.\n" );
#endif
		}

		return pos;
	}

	irr::scene::ISceneNode* Level_::GetParentWall( const float& x, const float& y, const float& z )
	{
		irr::scene::ISceneNode* parent = nullptr;
		if( z == 0 )
		{
			parent = FrontWall;
		}
		else if( x == levelSize )
		{
			parent = RightWall;
		}
		else if( x == 0 )
		{
			parent = LeftWall;
		}
		else if( z == levelSize )
		{
			parent = BackWall;
		}
		else if( y == levelSize )
		{
			parent = TopWall;
		}
		else if( y == 0 )
		{
			parent = BottomWall;
		}
		return parent;
	}

	bool Level_::InitLevel( Game *game, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
		irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
		irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex, irr::video::ITexture *transTex )
	{
		using namespace irr;
		using namespace std;

		if( !soundSystem->SoundSystemInitOK() )
		{
			return false;
		}

		// make sure level data was read in correctly
		// note that this part can be extended to read in other info like music location
		if( !LoadLevelData( fileSystem, levelFile, game->manager->guienv ) )
		{
			return false;
		}

		game->manager->font->ChangeFontSize( 12 );
		auto smgr = game->manager->smgr;

		CreateCamera( smgr );
		CreateWallNodes( smgr );
		CreateBall( smgr, ballTex );
		CreateLights( smgr );
		CreateRenderBlocks( smgr, wall, transTex );
		CreateVents( smgr, ventFXTex, ventTex );
		CreateExit( exitTex );
		// creating physics blocks is last because the exit has to be removed
		CreatePhysicsBlocks();

		soundSystem->CreateSounds( soundFilenameMap );
		CreateVentSounds();
		soundSystem->StartPlayingLoopingSounds();

		return true;
	}

	bool Level_::LoadLevelData( irr::io::IFileSystem *fileSystem, const std::string& levelFile,
		irr::gui::IGUIEnvironment *guienv )
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
				else if( name == "helpText\0" )
				{
					std::string text = levelReader->getAttributeValue( "text" );
					if( text.length() > 0 )
					{
						helpText.reset( new HelpText( guienv, 5, 5, 375, 85, text ) );
					}
				}
			}
		}

		delete levelReader;
		return true;
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

		vector3df soundSysVelocity = ( position - lastpos ) / ( 1.f / 60.f );
		vector3df ballVelocity = physics->GetBallVelocity().normalize();
		
		soundSystem->Update( position.X, position.Y, position.Z,			// position
			soundSysVelocity.X, soundSysVelocity.Y, soundSysVelocity.Z,		// velocity
			ballVelocity.X, ballVelocity.Y, ballVelocity.Z );				// forward vector

		lastpos = position;

		return levelComplete;
	}



	// public implementation
	Level::Level() : level_( new Level_ )
	{
	}

	Level::~Level()
	{		
	}

	void Level::ApplyDirectionToCamera( Direction dir )
	{
		level_->ApplyDirectionToCamera( dir );
	}

	void Level::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		level_->ApplyImpulseToBall( dir, x, y, z );
	}

	bool Level::InitLevel( Game *game, irr::io::IFileSystem *fileSystem, const std::string& levelFile, 
		irr::video::ITexture *wall, irr::video::ITexture *ballTex, irr::video::ITexture *exitTex,
		irr::video::ITexture *ventTex, irr::video::ITexture *ventFXTex, irr::video::ITexture *transTex )
	{
		return level_->InitLevel( game, fileSystem, levelFile, wall, ballTex, exitTex, ventTex, ventFXTex, transTex );
	}

	void Level::Pause( bool pause )
	{
		level_->Pause( pause );
	}

	void Level::PlayJetSound()
	{
		level_->PlayJetSound();
	}

	bool Level::StepSimulation( float timeDelta )
	{
		return level_->StepSimulation( timeDelta );
	}
}