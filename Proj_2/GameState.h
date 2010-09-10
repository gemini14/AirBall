#ifndef GAMESTATE_H
#define GAMESTATE_H


#include <boost/noncopyable.hpp>
#include <fstream>
#include <irrlicht.h>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>

#include "Game.h"
#include "GameStateEventReceiver.h"
#include "Level.h"
#include "State.h"


namespace Tuatara
{

	class Game;
	class GameStateEventReceiver;
	class Level;

	// main GameState .. state lol
	class GameState : public State<Game>, boost::noncopyable
	{
		GameState();
		~GameState();

		int currentLevel;
		int numberOfLevels;
		bool paused;

		std::shared_ptr<GameStateEventReceiver> gameEventRcvr;
		Level *level;

		irr::scene::ICameraSceneNode *camera;
		void ResetGameState( Game *game );

	public:

		static GameState* Instance();

		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );

		void Pause( bool pause );

		friend class GameStateEventReceiver;

	};

}

#endif