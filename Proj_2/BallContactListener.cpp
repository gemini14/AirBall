#include "BallContactListener.h"

#include <algorithm>
#include <bitset>
#include <math.h>

#include "EnumsConstants.h"
#include "SoundSystem.h"

namespace Tuatara
{
	BallContactListener::BallContactListener( SoundSystem *sound ) : soundSystem( sound )
	{
	}

	bool BallContactListener::IsOnWall( const float& x, const float& y, const float& z, const hkVector4& velocity  )
	{
		static const float maximum = levelSize - BLOCK_HALF_EXTENT * 3 - BALL_RADIUS;
		static const float minimum = BLOCK_HALF_EXTENT + BALL_RADIUS;
		const float positionBuffer = 0.02f;

		auto maximumCheck = [=]( float val ) -> bool
		{
			if( val >= maximum - positionBuffer )
			{
				return true;
			}
			return false;
		};

		auto minimumCheck = [=]( float val ) -> bool
		{
			if( val <= minimum + positionBuffer )
			{
				return true;
			}
			return false;
		};


		// left wall
		if( minimumCheck( x ) )
		{
			return true;
		}
		// right wall
		if( maximumCheck( x ) )
		{
			return true;
		}
		// top wall
		if( maximumCheck( y ) )
		{
			return true;
		}
		// bottom wall
		if( minimumCheck( y ) && velocity(1) < -0.5f )
		{
			return true;
		}
		// front wall
		if( minimumCheck( z ) )
		{
			return true;
		}
		// back wall
		if( maximumCheck( z ) )
		{
			return true;
		}

		return false;
	}

	void BallContactListener::contactPointCallback( const hkpContactPointEvent& event )
	{
		using namespace std;

		if( abs( event.getSeparatingVelocity() ) >= 0.75f )
		{
			auto ball = event.getBody( event.m_source );
			hkVector4 position( ball->getPosition() );
			if( IsOnWall( position(0), position(1), position(2), ball->getLinearVelocity() ) )
			{
				// left in for the moment in case of testing later on for refinement
				/*std::cout << event.getSeparatingVelocity() << " " << position(0) << " " 
					<< position(1) << " " << position(2) << std::endl;*/
				soundSystem->PlayCollisionSound();
			}
		}
	}
}