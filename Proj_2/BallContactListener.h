#ifndef BALL_CONTACT_LISTENER_H
#define BALL_CONTACT_LISTENER_H


#include <iostream>

#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>


namespace Tuatara
{
	class SoundSystem;

	class BallContactListener : public hkReferencedObject, public hkpContactListener
	{
	
		SoundSystem *soundSystem;

	public:

		BallContactListener( SoundSystem *sound );

		virtual void contactPointCallback( const hkpContactPointEvent& event );
	};

}

#endif