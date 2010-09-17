#include "BallContactListener.h"

#include <math.h>

#include "EnumsConstants.h"
#include "SoundSystem.h"

namespace Tuatara
{
	BallContactListener::BallContactListener( SoundSystem *sound ) : soundSystem( sound )
	{
	}

	void BallContactListener::contactPointCallback( const hkpContactPointEvent& event )
	{
		using namespace std;

		hkpRigidBody *ball = event.getBody( event.m_source );
		hkVector4 position( ball->getPosition() ), velocity( ball->getLinearVelocity() );
		
		if( ( ( position(1) < (BLOCK_HALF_EXTENT + BALL_RADIUS + 0.01f) && 
			position(1) > (BLOCK_HALF_EXTENT + BALL_RADIUS - 0.05f) ) && abs( velocity(1) ) > 0.3 )
			||
			( position(1) > (BLOCK_HALF_EXTENT + BALL_RADIUS + 0.01f) ) )
		{
			soundSystem->PlayCollisionSound();

			std::cout << "A collision was detected! X: " << ball->getPosition()(0) << " Y: " << 
				ball->getPosition()(1) << " Z: " << ball->getPosition()(2) << " Y-Velocity: " 
				<< ball->getLinearVelocity()(1) << std::endl;
		}
	}
}