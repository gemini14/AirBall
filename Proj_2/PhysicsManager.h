#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H


#include <memory>

#include <boost/noncopyable.hpp>
#include <irrlicht.h>

#include "EnumsConstants.h"


namespace Tuatara
{
	struct Physics_Manager;

	class PhysicsManager : boost::noncopyable
	{
	private:

		std::shared_ptr<Physics_Manager> physics;
		
	public:

		PhysicsManager();
		~PhysicsManager();

		void CreateBlock( const float& x, const float& y, const float& z);
		void CreateBall( const float& entryX, const float& entryY, const float& entryZ );
		void CreatePhantom( const float& x, const float& y, const float& z, const Direction& dir,
			const int& strength, bool isVent = true );

		bool StepSimulation( float timeDelta = 0 );

		void ApplyImpulseToBall( Direction dir, const float& x = 0, const float& y = 0, const float& z = 0 );
		irr::core::vector3df GetBallPosition() const;
		bool GetBallRotation( irr::core::vector3df& rotationVector ) const;
		irr::core::vector3df GetBallVelocity() const;
	};		
}

#endif