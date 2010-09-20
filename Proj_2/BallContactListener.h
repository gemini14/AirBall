#ifndef BALL_CONTACT_LISTENER_H
#define BALL_CONTACT_LISTENER_H


#include <iostream>

#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>

#include "EnumsConstants.h"


namespace Tuatara
{
	class SoundSystem;

	class BallContactListener : public hkReferencedObject, public hkpContactListener
	{
	
		float maximum;
		float minimum;
		float positionBuffer;

		SoundSystem *soundSystem;

		bool IsOnWall( const float& x, const float& y, const float& z, const hkVector4& velocity );

	public:

		BallContactListener( SoundSystem *sound );

		virtual void contactPointCallback( const hkpContactPointEvent& event );
	};

}

#endif