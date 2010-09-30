#include "PhysicsManager.h"

#include <stdio.h>
#include <vector>

#include <boost/foreach.hpp>

#include "BallContactListener.h"
#include "EnumsConstants.h"
#include "Game.h"

// Havok directive for addtional debugging stuff
#ifdef _DEBUG
#define HK_DEBUG
#endif

///// Havok includes /////
// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>

// Dynamics includes
#include <Physics/Collide/hkpCollide.h>										
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>					

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/World/Listener/hkpWorldPostSimulationListener.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/Phantom/hkpAabbPhantom.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>	

// Keycode
#include <Common/Base/keycode.cxx>

#if !defined USING_HAVOK_PHYSICS
#error Physics is needed to build this demo. It is included in the common package for reference only.
#endif

// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>


namespace Tuatara
{
	struct Physics_Manager : boost::noncopyable, hkpWorldPostSimulationListener
	{
		// variables

		struct Vent
		{
			hkpAabbPhantom *phantom;
			Direction direction;
			int strength;
		};

		typedef std::vector<Vent*> VentVector;

		hkJobThreadPool *threadPool;
		hkJobQueue *jobQueue;

		hkpWorld *world;
		
		// Havok debugger variables
		#if defined(_DEBUG) | defined(DEBUG)
		hkVisualDebugger *vdb;
		hkpPhysicsContext *context;
		#endif

		hkpRigidBody *ball;
		BallContactListener *ballListener;
		hkpAabbPhantom *exit;

		VentVector vents;

		bool ballInVent;
		bool levelComplete;

		// functions
		Physics_Manager();
		~Physics_Manager();

		void AdjustAabbForExit( hkAabb &info, const Direction& dir );

		void CreateBlock( const float& x, const float& y, const float& z);
		void CreateBall( const float& entryX, const float& entryY, const float& entryZ, SoundSystem *sound );
		void CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir,
			const int& strength, bool isVent = true );

		bool StepSimulation( float timeDelta );

		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0 );
		irr::core::vector3df GetBallPosition() const;
		bool GetBallRotation( irr::core::vector3df& rotationVector ) const;
		irr::core::vector3df GetBallVelocity() const;

		virtual void postSimulationCallback( hkpWorld* world );
	};

	static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
	{
		#if defined(_DEBUG) | defined(DEBUG)
		printf("%s", msg);
		#endif
	}


	///// private implementation /////

	Physics_Manager::Physics_Manager() : levelComplete( false ), ballInVent( false )
	{
		// initialize base system and memory
		hkMemoryRouter *memoryRouter = hkMemoryInitUtil::initDefault();
		hkBaseSystem::init( memoryRouter, errorReport );

		// initialize multithreading classes
		// we will cap the number of threads used
		int totalNumThreadsUsed;

		// get number of physical threads
		hkHardwareInfo hwInfo;
		hkGetHardwareInfo( hwInfo );
		totalNumThreadsUsed = hwInfo.m_numThreads;

		// use one less that that since we must use one for the simulation
		hkCpuJobThreadPoolCinfo threadPoolCinfo;
		threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
		threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

		// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
		// Here we only use it for physics.
		hkJobQueueCinfo info;
		info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
		jobQueue = new hkJobQueue( info );

		// enable monitors for thread
		// monitor have been enabled for thread pool threads already
		hkMonitorStream::getInstance().resize( 200000 );

		// this hold global simulation parameters (gravity, etc.)
		// gravity (most important attribute for this app) defaults to -9.8 m/s^2
		hkpWorldCinfo worldInfo;
		worldInfo.m_gravity = hkVector4( 0, -1.f, 0 );
		worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS;
		//worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

		// flag objects that fall out of the world to be automatically removed
		worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
		worldInfo.m_collisionTolerance = 0.01f;

		world = new hkpWorld( worldInfo );

		// register all collision agents
		hkpAgentRegisterUtil::registerAllAgents( world->getCollisionDispatcher() );

		// register all modules we will run multi-threaded with job queue
		world->registerWithJobQueue( jobQueue );

		#if defined(_DEBUG) | defined(DEBUG)
		// now initialize the visual debugger
		hkArray<hkProcessContext*> contexts;

		// register physics specific visual debugger processes
		context = new hkpPhysicsContext();
		//all physics viewers
		hkpPhysicsContext::registerAllPhysicsProcesses();
		//add physics world so viewers can see it
		context->addWorld( world );
		contexts.pushBack( context );

		// we're single threading, so disable checks
		world->setMultithreadedAccessChecking( hkpWorld::MT_ACCESS_CHECKING_DISABLED );

		// init the debugger
		vdb = new hkVisualDebugger( contexts );
		vdb->serve();
		#endif

		world->addWorldPostSimulationListener( this );
	}

	Physics_Manager::~Physics_Manager()
	{
		world->removeWorldPostSimulationListener( this );

		// memory cleanup
		BOOST_FOREACH( Vent *v, vents )
		{
			v->phantom->removeReference();
			delete v;
		}

		exit->removeReference();
		ball->removeContactListener( ballListener );
		ball->removeReference();

		world->removeReference();
		
		#if defined(_DEBUG) | defined(DEBUG)
		vdb->removeReference();
		// context is deleted only after finished using VDB
		context->removeReference();
		#endif

		delete jobQueue;
		threadPool->removeReference();

		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void Physics_Manager::AdjustAabbForExit( hkAabb &info, const Direction& dir )
	{
		hkVector4 min( info.m_min ), max( info.m_max );
		const float ADJUSTMENT_VALUE = 1.5f;
		switch( dir )
		{
		case FORWARD:
			{
				info.m_min = hkVector4( min(0), min(1), min(2) - ADJUSTMENT_VALUE );
				info.m_max = hkVector4( max(0), max(1), max(2) - ADJUSTMENT_VALUE );
			}
			break;
		case BACKWARD:
			{
				info.m_min = hkVector4( min(0), min(1), min(2) + ADJUSTMENT_VALUE );
				info.m_max = hkVector4( max(0), max(1), max(2) + ADJUSTMENT_VALUE );
			}
			break;
		case LEFT:
			{
				info.m_min = hkVector4( min(0) + ADJUSTMENT_VALUE, min(1), min(2) );
				info.m_max = hkVector4( max(0) + ADJUSTMENT_VALUE, max(1), max(2) );
			}
			break;
		case RIGHT:
			{
				info.m_min = hkVector4( min(0) - ADJUSTMENT_VALUE, min(1), min(2) );
				info.m_max = hkVector4( max(0) - ADJUSTMENT_VALUE, max(1), max(2) );
			}
			break;
		case UP:
			{
				info.m_min = hkVector4( min(0), min(1) - ADJUSTMENT_VALUE, min(2) );
				info.m_max = hkVector4( max(0), max(1) - ADJUSTMENT_VALUE, max(2) );
			}
			break;
		case DOWN:
			{
				info.m_min = hkVector4( min(0), min(1) + ADJUSTMENT_VALUE, min(2) );
				info.m_max = hkVector4( max(0), max(1) + ADJUSTMENT_VALUE, max(2) );
			}
			break;
		default:
			#if defined(_DEBUG) | defined(DEBUG)
			printf( "AdjustAabbForExit: bad direction sent.\n" );
			#endif
			break;
		}
	}

	void Physics_Manager::CreateBlock( const float& x, const float& y, const float& z)
	{
		hkVector4 buildingBlockSize( BLOCK_HALF_EXTENT, BLOCK_HALF_EXTENT, BLOCK_HALF_EXTENT );
		hkpConvexShape *buildingBlock = new hkpBoxShape( buildingBlockSize, 0 );

		hkpRigidBodyCinfo ci;
		ci.m_shape = buildingBlock;
		ci.m_motionType = hkpMotion::MOTION_FIXED;
		ci.m_position = hkVector4( x, y, z );
		// blocks will not move, so keep them fixed, but collidable
		ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
		ci.m_friction = 0.25f;

		// add the block and remove the reference
		world->addEntity( new hkpRigidBody( ci ) )->removeReference();

		buildingBlock->removeReference();
	}

	void Physics_Manager::CreateBall( const float& entryX, const float& entryY, const float& entryZ, SoundSystem *sound )
	{
		hkReal radius = BALL_RADIUS;
		hkReal sphereMass = 3.f;
		hkReal maxVelocity = 2.f;

		hkpRigidBodyCinfo info;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties( radius, sphereMass, massProperties );

		info.m_mass = massProperties.m_mass;
		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_shape = new hkpSphereShape( radius );
		info.m_friction = 0.85f;
		info.m_allowedPenetrationDepth = 0.f;
		info.m_position = hkVector4( entryX, entryY, entryZ );

		info.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
		info.m_contactPointCallbackDelay = 0;

		ball = new hkpRigidBody( info );

		ball->setMaxLinearVelocity(maxVelocity);

		world->addEntity( ball );
		ballListener = new BallContactListener( sound );
		ball->addContactListener( ballListener );
		info.m_shape->removeReference();
	}

	void Physics_Manager::CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir, 
		const int& strength, bool isVent )
	{
		hkAabb info;
		info.m_min = hkVector4( x - 0.5f, y - 0.5f, z - 0.5f );

		// return m_max point of the box for the phantom based on the direction of the vent (max points
		// will always point toward center of level)
		auto maxCalc = [&]()->hkVector4
		{
			float max_x( x ), max_y( y ), max_z( z );
			switch( dir )
			{
			case FORWARD:
				{
					max_x += 1.f;
					max_y += 1.f;
					max_z += static_cast<float>(strength) + 1.f;

					info.m_min = hkVector4( x - 0.5f, y - 0.5f, z + 0.5f );
				}
				break;
			case BACKWARD:
				{
					max_x += 1.f;
					max_y += 1.f;
					max_z -= static_cast<float>(strength);

					// flip z values between m_min and m_max or Havok complains
					float temp_z = info.m_min(2) + 0.5f;
					info.m_min = hkVector4( info.m_min(0), info.m_min(1), max_z - 0.5f );
					max_z = temp_z;
				}
				break;
			case LEFT:
				{
					max_x -= static_cast<float>(strength) + 1.f;
					max_y += 1.f;
					max_z += 1.f;

					// flip x values between m_min and m_max or Havok complains
					float temp_x = info.m_min(0) + 0.5f;
					info.m_min = hkVector4( max_x + 0.5f, info.m_min(1), info.m_min(2) );
					max_x = temp_x;
				}
				break;
			case RIGHT:
				{
					max_x += static_cast<float>(strength) + 1.f;
					max_y += 1.f;
					max_z += 1.f;

					info.m_min = hkVector4( x + 0.5f, y - 0.5f, z - 0.5f );
				}
				break;
			case UP:
				{
					max_x += 1.f;
					max_y += static_cast<float>(strength) + 1;
					max_z += 1.f;

					info.m_min = hkVector4( x - 0.5f, y + 0.5f, z - 0.5f );
				}
				break;
			case DOWN:
				{
					max_x += 1.f;
					max_y -= static_cast<float>(strength);
					max_z += 1.f;

					// flip y values between m_min and m_max or Havok complains
					float temp_y = info.m_min(1) + 0.5f;
					info.m_min = hkVector4( info.m_min(0), max_y - 0.5f, info.m_min(2) );
					max_y = temp_y;
				}
				break;
			default:
				#if defined(_DEBUG) | defined(DEBUG)
				printf( "CreateVent: bad direction sent.\n" );
				#endif
				return hkVector4( 0.f, 0.f, 0.f);
			}

			// remove 0.5f because position of center of blocks is on the axes, but the extents of the cubes themselves lie
			// on .5 values on the axes
			return hkVector4( max_x - 0.5f, max_y - 0.5f, max_z - 0.5f);
		};

		info.m_max = maxCalc();

		if( isVent )
		{
			Vent *vent = new Vent;

			// store information for vent
			vent->direction = dir;
			vent->strength = strength;
			vent->phantom = new hkpAabbPhantom( info );

			// add to world and vector
			world->addPhantom( vent->phantom );
			vents.push_back( vent );
		}
		else
		{
			AdjustAabbForExit( info, dir );
			exit = new hkpAabbPhantom( info );
			world->addPhantom( exit );
		}
	}

	bool Physics_Manager::StepSimulation( float timeDelta )
	{
		// use the timeDelta if given, otherwise use 1/60th of a second
		static hkReal timestep;
		if( timeDelta == 0 )
		{
			timestep = 1.f / 60.f;
		}
		else
		{
			timestep = timeDelta;
		}

		// step the world
		world->stepDeltaTime( timestep );

		#if defined(_DEBUG) | defined(DEBUG)
		// step the visual debugger after synchronizing the timer data
		context->syncTimers( threadPool );
		vdb->step();
		#endif

		// clear accumulated timer data in this thread and all slave threads
		hkMonitorStream::getInstance().reset();
		threadPool->clearTimerData();

		return levelComplete;
	}

	void Physics_Manager::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		hkVector4 impulse;

		switch( dir )
		{
		case FORWARD:
			impulse.setXYZ( hkVector4( x, 0, z ) );
			break;

		case LEFT:
			impulse.setXYZ( hkVector4( x, 0, z ) );
			break;

		case BACKWARD:
			impulse.setXYZ( hkVector4( x, 0, z ) );
			break;

		case RIGHT:
			impulse.setXYZ( hkVector4( x, 0, z ) );
			break;

		case UP:
			if( ball->getPosition()(1) <= BLOCK_HALF_EXTENT + BALL_RADIUS + 0.02f || ballInVent )
			{
				impulse.setXYZ( hkVector4( 0, y, 0 ) );
				break;
			}
			else
			{
				return;
			}

		default:
			#if defined(_DEBUG) | defined(DEBUG)
			printf( "ApplyImpulseToBall: Bad direction sent.\n" );
			#endif
			break;
		}

		ball->applyLinearImpulse( impulse );
	}

	irr::core::vector3df Physics_Manager::GetBallPosition() const
	{
		hkVector4 ballPos = ball->getPosition();
		return irr::core::vector3df( ballPos( 0 ), ballPos( 1 ), ballPos( 2 ) );
	}

	bool Physics_Manager::GetBallRotation( irr::core::vector3df& rotationVector ) const
	{
		hkQuaternion ballRotation = ball->getRotation();
		if( !ballRotation.hasValidAxis() )
		{
			return false;
		}

		static hkVector4 originalAxis;
		ballRotation.getAxis( originalAxis );
		irr::core::vector3df axis( originalAxis( 0 ), originalAxis( 1 ), originalAxis( 2 ) );
		axis.normalize();

		static irr::core::quaternion q;
		// axis must be normalized (see above)
		q.fromAngleAxis( ballRotation.getAngle(), axis );
		q.toEuler( rotationVector );
		// this must be in degrees for the setRotation function
		rotationVector = rotationVector * irr::core::RADTODEG;

		return true;
	}

	irr::core::vector3df Physics_Manager::GetBallVelocity() const
	{
		hkVector4 vel = ball->getLinearVelocity();
		return irr::core::vector3df( vel(0), vel(1), vel(2) );
	}

	void Physics_Manager::postSimulationCallback( hkpWorld* world )
	{
		bool isInVent = false;
		BOOST_FOREACH( Vent *v, vents )
		{
			// calculate impulse vector based on vent direction
			auto impulse = [=]()->hkVector4
			{
				float x( 0.f ), y( 0.f ), z( 0.f );
				float impulseValue = 0.05f;
				switch( v->direction )
				{
				case FORWARD:
					z += impulseValue;
					break;
				case BACKWARD:
					z -= impulseValue;
					break;
				case LEFT:
					x -= impulseValue;
					break;
				case RIGHT:
					x += impulseValue;
					break;
				case UP:
					y += impulseValue;
					break;
				case DOWN:
					y -= impulseValue;
					break;
				default:
					#if defined(_DEBUG) | defined(DEBUG)
					printf( "postSimulationCallback: bad direction in vent structure.\n" );
					#endif
					return hkVector4( 0, 0, 0);
				}

				return hkVector4( x, y, z );
			};

			v->phantom->ensureDeterministicOrder();

			// iterate through all overlapping bodies
			for( int i = 0; i < v->phantom->getOverlappingCollidables().getSize(); ++i )
			{
				hkpCollidable* c = v->phantom->getOverlappingCollidables()[i];
				// if collidable is the ball (which it will always be, only one dynamic object in game)
				if ( c->getType() == hkpWorldObject::BROAD_PHASE_ENTITY )
				{
					// get the body
					hkpRigidBody* rigidBody = hkpGetRigidBody( v->phantom->getOverlappingCollidables()[i] );
					// add apply the impulse to it
					if ( rigidBody && rigidBody == ball )
					{
						isInVent = true;
						if( v->direction == UP )
						{
							float proportion = rigidBody->getPosition()(1) / v->strength;
							hkVector4 impulseVec = impulse();
							impulseVec.mul4( proportion );
							rigidBody->applyLinearImpulse( impulseVec );
						}
						else
						{
							rigidBody->applyLinearImpulse( impulse() );
						}
					}
				}
			}
		}

		exit->ensureDeterministicOrder();
		// level completion check
		for( int i = 0; i < exit->getOverlappingCollidables().getSize(); ++i )
		{
			hkpCollidable* c = exit->getOverlappingCollidables()[i];
			if ( c->getType() == hkpWorldObject::BROAD_PHASE_ENTITY )
			{
				hkpRigidBody* rigidBody = hkpGetRigidBody( exit->getOverlappingCollidables()[i] );
				if ( rigidBody && rigidBody == ball)
				{
					levelComplete = true;
				}
			}
		}

		ballInVent = isInVent;
	}



	///// public implementation /////

	PhysicsManager::PhysicsManager() : physics( new Physics_Manager )
	{		
	}

	PhysicsManager::~PhysicsManager()
	{		
	}

	void PhysicsManager::CreateBlock( const float& x, const float& y, const float& z)
	{
		physics->CreateBlock( x, y, z );
	}

	void PhysicsManager::CreateBall( const float& entryX, const float& entryY, const float& entryZ, SoundSystem *sound )
	{
		physics->CreateBall( entryX, entryY, entryZ, sound );
	}

	void PhysicsManager::CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir, 
		const int& strength, bool isVent )
	{
		physics->CreatePhantom( x, y, z, dir, strength, isVent );
	}

	bool PhysicsManager::StepSimulation( float timeDelta )
	{
		return physics->StepSimulation( timeDelta );
	}

	void PhysicsManager::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
	{
		physics->ApplyImpulseToBall( dir, x, y, z );
	}

	irr::core::vector3df PhysicsManager::GetBallPosition() const
	{
		return physics->GetBallPosition();
	}

	bool PhysicsManager::GetBallRotation( irr::core::vector3df& rotationVector ) const
	{
		return physics->GetBallRotation( rotationVector );
	}

	irr::core::vector3df PhysicsManager::GetBallVelocity() const
	{
		return physics->GetBallVelocity();
	}
}