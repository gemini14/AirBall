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
	PhysicsManager::PhysicsManager()
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
	}

	PhysicsManager::~PhysicsManager()
	{
		world->removeReference();
		vdb->removeReference();

		// context is deleted only after finished using VDB
		context->removeReference();

		delete jobQueue;
		threadPool->removeReference();

		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void PhysicsManager::StepSimulation( float timeDelta )
	{
		//hkStopwatch stopWatch;
		//stopWatch.start();
		//hkReal lastTime = stopWatch.getElapsedSeconds();

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
	}

	void PhysicsManager::CreateWorld( /*Game *game*/ )
	{
		// create the ground box (so ball doesn't fall down through...err...nothing)
		hkVector4 ground( 50.f, 2.f, 50.f );
		hkpConvexShape *shape = new hkpBoxShape( ground, 0 );

		hkpRigidBodyCinfo ci;
		ci.m_shape = shape;
		ci.m_motionType = hkpMotion::MOTION_FIXED;
		ci.m_position = hkVector4( 0.f, -2.f, 0.f );
		ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;

		world->addEntity( new hkpRigidBody( ci ) )->removeReference();

		shape->removeReference();

		// add the six faces
		hkVector4 buildingBlockSize( 0.5f, 0.5f, 0.5f );
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
						hkpConvexShape *buildingBlock = new hkpBoxShape( buildingBlockSize, 0 );

						hkpRigidBodyCinfo ci;
						ci.m_shape = buildingBlock;
						ci.m_motionType = hkpMotion::MOTION_FIXED;
						ci.m_position = hkVector4( static_cast<float>(x),
							static_cast<float>(cubeLevel), static_cast<float>(z) );
						ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
						ci.m_friction = .8f;

						world->addEntity( new hkpRigidBody( ci ) )->removeReference();

						buildingBlock->removeReference();
					}
				}
			}
		}		
	}

	void PhysicsManager::CreateBall( /*Game *game*/ )
	{
		// radius of 0.5 for a diameter of 1
		hkReal radius = .5f;
		hkReal sphereMass = 1.0f;

		hkpRigidBodyCinfo info;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties( radius, sphereMass, massProperties );

		info.m_mass = massProperties.m_mass;
		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_shape = new hkpSphereShape( radius );
		info.m_friction = 1.f;
		info.m_position = hkVector4( levelSize / 2.f, 1.f, levelSize / 2.f );//1.5f, 2.f, 1.5f );

		info.m_motionType = hkpMotion::MOTION_DYNAMIC;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

		hkpRigidBody *sphereRigidBody = new hkpRigidBody( info );
		ball = sphereRigidBody;

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
		hkReal modifier = 1.f;

		switch( dir )
		{
		case FORWARD:
			impulse.setXYZ( hkVector4( x / modifier, 0, z / modifier ) );
			break;

		case LEFT:
			impulse.setXYZ( hkVector4( x / modifier, 0, z / modifier ) );
			break;

		case BACKWARD:
			impulse.setXYZ( hkVector4( x / modifier, 0, z / modifier ) );
			break;

		case RIGHT:
			impulse.setXYZ( hkVector4( x / modifier, 0, z / modifier ) );
			break;
		
		case UP:
			impulse.setXYZ( hkVector4( 0, 2.f, 0 ) );
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