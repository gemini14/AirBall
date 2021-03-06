#ifndef GAMESTATE_EVENTRECEIVER_H
#define GAMESTATE_EVENTRECEIVER_H


#include <irrlicht.h>

#include "EnumsConstants.h"
#include "Game.h"
#include "GameState.h"
#include "MainMenu.h"
#include "StateMachine.h"


namespace Tuatara
{

	class Game;
	class GameState;

	class GameStateEventReceiver : public irr::IEventReceiver
	{
	private:

		Game *game;
		GameState *gameState;

	public:

		GameStateEventReceiver();

		// the reason this can't go in the constructor is that the GameState state doesn't have access to the
		// Game pointer originally
		void SetGame( Game *g, GameState *gState );

		// event callback
		virtual bool OnEvent( const irr::SEvent& event);

	};

}

#endif