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
		worldInfo.m_gravity = hkVector4( 0, -4.9f, 0 );
		worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS;
		//worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
		// flag objects that fall out of the world to be automatically removed
		worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
		worldInfo.m_collisionTolerance = 0.1f;

		world = new hkpWorld( worldInfo );

		// when multithreaded and in debug mode, sdk makes sure only one thread is changing the world at once
		// each thread must call markForRead / markForWrite before it modifies the world
		//world->markForWrite();

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
		// we have finished modifying the world, release write marker
		//world->unmarkForWrite();


		vdb = new hkVisualDebugger( contexts );
		vdb->serve();
	}

	PhysicsManager::~PhysicsManager()
	{
		world->markForWrite();
		world->removeReference();

		vdb->removeReference();

		// context is deleted only after finished using VDB
		context->removeReference();

		delete jobQueue;
		threadPool->removeReference();

		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void PhysicsManager::StepSimulation()
	{
		hkStopwatch stopWatch;
		stopWatch.start();
		hkReal lastTime = stopWatch.getElapsedSeconds();

		static hkReal timestep = 1.f / 60.f;

		// get read/write access to the world (for debug build only)
		//world->markForWrite();

		// step the world
		//world->stepMultithreaded( jobQueue, threadPool, timestep );
		world->stepDeltaTime( timestep );

		// step the visual debugger after synchronizing the timer data
		context->syncTimers( threadPool );
		vdb->step();

		// clear accumulated timer data in this thread and all slave threads
		hkMonitorStream::getInstance().reset();
		threadPool->clearTimerData();

		// pause until actual time has passed
		//while( stopWatch.getElapsedSeconds() < (lastTime + timestep) );
		/*{
			lastTime += timestep;
		}*/

		//world->unmarkForWrite();
	}

	void PhysicsManager::CreateWorld( Game *game )
	{
		//world->markForWrite();

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

		//world->unmarkForWrite();
	}

	void PhysicsManager::CreateBall( Game *game )
	{
		//world->markForWrite();

		hkReal radius = 1.f;
		hkReal sphereMass = 1.0f;

		hkVector4 relPos( 0.0f, radius + 0.0f, 0.0f );

		hkpRigidBodyCinfo info;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties( radius, sphereMass, massProperties );

		info.m_mass = massProperties.m_mass;
		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_shape = new hkpSphereShape( radius );
		
		hkVector4 posy( 0.f, 10.f, 0.f );
		info.m_position.setAdd4( posy, relPos );
		info.m_motionType = hkpMotion::MOTION_DYNAMIC;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

		hkpRigidBody *sphereRigidBody = new hkpRigidBody( info );
		ball = sphereRigidBody;

		world->addEntity( sphereRigidBody );
		sphereRigidBody->removeReference();
		info.m_shape->removeReference();

		//hkVector4 vel( 0.0f, 0.0f, 0.0f );
		//sphereRigidBody->setLinearVelocity( vel );

		//world->unmarkForWrite();
	}

	static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
	{
		printf("%s", msg);
	}

	void PhysicsManager::ApplyImpulseToBall( Direction dir )
	{
		hkVector4 impulse( 0, ball->getMass() * 5, 0 );
		ball->applyLinearImpulse( impulse );
	}
}