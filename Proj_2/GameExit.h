#ifndef GAMEEXIT_H
#define GAMEEXIT_H


#include <boost/noncopyable.hpp>

#include "Game.h"
#include "State.h"


namespace Tuatara
{

	class Game;

	// dummy state for signalling program termination
	class GameExit : public State<Game>, boost::noncopyable
	{

	private:

		GameExit();
		
	public:

		static GameExit* Instance();

		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );

	};

}

#endif