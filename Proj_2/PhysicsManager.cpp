#include "PhysicsManager.h"

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
	PhysicsManager::PhysicsManager() : levelComplete( false )
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
		//worldInfo.m_collisionTolerance = 0.01f;

		world = new hkpWorld( worldInfo );

		// register all collision agents
		hkpAgentRegisterUtil::registerAllAgents( world->getCollisionDispatcher() );

		// register all modules we will run multi-threaded with job queue
		world->registerWithJobQueue( jobQueue );


		// now initialize the visual debugger
		hkArray<hkProcessContext*> contexts;

		// register physics specific visual debugger processes
		context = new hkpPhysicsContext();
		//all physics viewers
		hkpPhysicsContext::registerAllPhysicsProcesses();
		//add physics world so viewers can see it
		context->addWorld( world );
		contexts.pushBack( context );

		world->setMultithreadedAccessChecking( hkpWorld::MT_ACCESS_CHECKING_DISABLED );

		vdb = new hkVisualDebugger( contexts );
		vdb->serve();

		world->addWorldPostSimulationListener( this );
	}

	PhysicsManager::~PhysicsManager()
	{
		world->removeWorldPostSimulationListener( this );

		BOOST_FOREACH( Vent *v, vents )
		{
			delete v;
		}

		world->removeReference();
		vdb->removeReference();

		// context is deleted only after finished using VDB
		context->removeReference();

		delete jobQueue;
		threadPool->removeReference();

		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void PhysicsManager::postSimulationCallback( hkpWorld* world )
	{
		BOOST_FOREACH( Vent *v, vents )
		{
			auto impulse = [=]()->hkVector4
			{
				float x( 0.f ), y( 0.f ), z( 0.f );
				switch( v->direction )
				{
				case FORWARD:
					z += 0.1f;
					break;
				case BACKWARD:
					z -= 0.1f;
					break;
				case LEFT:
					x -= 0.1f;
					break;
				case RIGHT:
					x += 0.1f;
					break;
				case UP:
					y += 0.1f;
					break;
				case DOWN:
					y -= 0.1f;
					break;
				default:
					printf( "postSimulationCallback: bad direction in vent structure.\n" );
					return hkVector4( 0, 0, 0);
				}

				return hkVector4( x, y, z );
			};

			for( int i = 0; i < v->phantom->getOverlappingCollidables().getSize(); ++i )
			{
				hkpCollidable* c = v->phantom->getOverlappingCollidables()[i];
				if ( c->getType() == hkpWorldObject::BROAD_PHASE_ENTITY )
				{

					hkpRigidBody* rigidBody = hkpGetRigidBody( v->phantom->getOverlappingCollidables()[i] );
					if ( rigidBody )
					{
						rigidBody->applyLinearImpulse( impulse() );
					}
				}
			}
		}

		// level completion check (bit of code duplication here, will look into it later)
		/*for( int i = 0; i < exit->getOverlappingCollidables().getSize(); ++i )
		{
			hkpCollidable* c = exit->getOverlappingCollidables()[i];
			if ( c->getType() == hkpWorldObject::BROAD_PHASE_ENTITY )
			{
				hkpRigidBody* rigidBody = hkpGetRigidBody( exit->getOverlappingCollidables()[i] );
				if ( rigidBody )
				{
					levelComplete = true;
				}
			}
		}*/
	}

	void PhysicsManager::CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir, 
		const int& strength, bool isVent )
	{
		hkAabb info;
		info.m_min = hkVector4( x - 0.5f, y - 0.5f, z - 0.5f );

		auto maxCalc = [=]()->hkVector4
		{
			float max_x( x ), max_y( y ), max_z( z );
			switch( dir )
			{
			case FORWARD:
				max_x += 1.f;
				max_y += 1.f;
				max_z += static_cast<float>(strength);
				break;
			case BACKWARD:
				max_x += 1.f;
				max_y += 1.f;
				max_z -= static_cast<float>(strength);
				break;
			case LEFT:
				max_x += static_cast<float>(strength);
				max_y += 1.f;
				max_z += 1.f;
				break;
			case RIGHT:
				max_x -= static_cast<float>(strength);
				max_y += 1.f;
				max_z += 1.f;
				break;
			case UP:
				max_x += 1.f;
				max_y += static_cast<float>(strength);
				max_z += 1.f;
				break;
			case DOWN:
				max_x += 1.f;
				max_y -= static_cast<float>(strength);
				max_z += 1.f;
				break;
			default:
				printf( "CreateVent: bad direction sent.\n" );
				return hkVector4( 0.f, 0.f, 0.f);
			}

			return hkVector4( max_x - 0.5f, max_y - 0.5f, max_z - 0.5f);
		};

		info.m_max = maxCalc();

		if( isVent )
		{
			Vent *vent = new Vent;

			vent->direction = dir;
			vent->strength = strength;
			vent->phantom = new hkpAabbPhantom( info );

			world->addPhantom( vent->phantom );
			vents.push_back( vent );
		}
		/*else
		{
			exit = new hkpAabbPhantom( info );
			world->addPhantom( exit );
		}*/
	}

	bool PhysicsManager::StepSimulation( float timeDelta )
	{
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

		// step the visual debugger after synchronizing the timer data
		context->syncTimers( threadPool );
		vdb->step();

		// clear accumulated timer data in this thread and all slave threads
		hkMonitorStream::getInstance().reset();
		threadPool->clearTimerData();

		return levelComplete;
	}

	void PhysicsManager::CreateBlock( const float& x, const float& y, const float& z)
	{
		hkVector4 buildingBlockSize( 0.5f, 0.5f, 0.5f );
		hkpConvexShape *buildingBlock = new hkpBoxShape( buildingBlockSize, 0 );

		hkpRigidBodyCinfo ci;
		ci.m_shape = buildingBlock;
		ci.m_motionType = hkpMotion::MOTION_FIXED;
		ci.m_position = hkVector4( x, y, z );
		ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
		ci.m_friction = 100.0f;

		world->addEntity( new hkpRigidBody( ci ) )->removeReference();

		buildingBlock->removeReference();

	}

	void PhysicsManager::CreateBall( const float& entryX, const float& entryY, const float& entryZ )
	{
		// radius of 0.5 for a diameter of 1 -- .25 for .5 -- .125 for .25
		hkReal radius = .125f;
		hkReal sphereMass = 5.0f;
		hkReal MaxVel = 0.5f;

		hkpRigidBodyCinfo info;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties( radius, sphereMass, massProperties );

		info.m_mass = massProperties.m_mass;
		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_shape = new hkpSphereShape( radius );
		info.m_friction = 3.0f;
		info.m_linearDamping = 0.05f;
		info.m_position = hkVector4( entryX, entryY, entryZ );

		info.m_motionType = hkpMotion::MOTION_DYNAMIC;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

		hkpRigidBody *sphereRigidBody = new hkpRigidBody( info );
		ball = sphereRigidBody;

		ball->setMaxLinearVelocity(MaxVel);

		world->addEntity( sphereRigidBody );
		sphereRigidBody->removeReference();
		info.m_shape->removeReference();
	}

	static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
	{
		printf("%s", msg);
	}

	void PhysicsManager::ApplyImpulseToBall( Direction dir, const float& x, const float& y, const float& z )
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
			impulse.setXYZ( hkVector4( 0, 7.5f, 0 ) );
			break;

		default:
			printf( "ApplyImpulseToBall: Bad direction sent.\n" );
			break;
		}

		ball->applyLinearImpulse( impulse );
	}

	irr::core::vector3df PhysicsManager::GetBallPosition()
	{
		hkVector4 ballPos = ball->getPosition();
		return irr::core::vector3df( ballPos.getSimdAt( 0 ), ballPos.getSimdAt( 1 ), ballPos.getSimdAt( 2 ) );
	}

	bool PhysicsManager::GetBallRotation( irr::core::vector3df& rotationVector )
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
}