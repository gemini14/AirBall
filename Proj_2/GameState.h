#ifndef GAMESTATE_H
#define GAMESTATE_H


#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "Game.h"
#include "GameStateEventReceiver.h"
#include "State.h"


namespace Tuatara
{

	class Game;
	class GameStateEventReceiver;

	// main GameState .. state lol
	class GameState : public State<Game>, boost::noncopyable
	{
		GameState();
		~GameState();
		
		std::shared_ptr<GameStateEventReceiver> gameEventRcvr;

	public:

		static GameState* Instance();

		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );

	};

}

#endif