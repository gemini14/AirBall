#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H


// other includes
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <stdio.h>
#include <vector>

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
		hkVisualDebugger *vdb;
		hkpPhysicsContext *context;

		hkpRigidBody *ball;
		hkpAabbPhantom *exit;

		VentVector vents;

		bool levelComplete;

		void AdjustAabbForExit( hkAabb &info, const Direction& dir );
		
	public:

		PhysicsManager();
		~PhysicsManager();

		//void CreateWorld();
		void CreateBlock( const float& x, const float& y, const float& z);
		void CreateBall( const float& entryX, const float& entryY, const float& entryZ );
		void CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir,
			const int& strength, bool isVent = true );

		bool StepSimulation( float timeDelta = 0 );

		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0 );
		irr::core::vector3df GetBallPosition() const;
		bool GetBallRotation( irr::core::vector3df& rotationVector ) const;

		virtual void postSimulationCallback( hkpWorld* world );
	};

	static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit);
}

#endif