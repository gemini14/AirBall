#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H


// other includes
#include <boost/noncopyable.hpp>
#include <stdio.h>

#include "EnumsConstants.h"
#include "Game.h"

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


namespace Tuatara
{
	class PhysicsManager : boost::noncopyable, hkpWorldPostSimulationListener
	{
	private:

		hkJobThreadPool *threadPool;
		hkJobQueue *jobQueue;

		hkpWorld *world;
		hkVisualDebugger *vdb;
		hkpPhysicsContext *context;

		hkpRigidBody *ball;
		hkpAabbPhantom *m_phantom;
	public:

		PhysicsManager();
		~PhysicsManager();

		void CreateWorld();
		void CreateBall();

		void StepSimulation( float timeDelta = 0 );

		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0 );
		irr::core::vector3df GetBallPosition();
		bool GetBallRotation( irr::core::vector3df& rotationVector );

		virtual void postSimulationCallback( hkpWorld* world );
	};

	static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit);
}

#endif