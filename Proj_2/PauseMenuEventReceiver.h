#pragma once
#include <irrlicht.h>
#include "Game.h"
#include "GameExit.h"
#include "GameState.h"
#include "MainMenu.h"
#include "StateMachine.h"

namespace Tuatara
{
	class Game;

	class PauseMenuEventReceiver :
		public irr::IEventReceiver
	{
	private:

		Game *game;

	public:

		PauseMenuEventReceiver();

		// the reason this can't go in the constructor is that the MainMenu state doesn't have access to the
		// Game pointer originally
		void SetGame( Game *g );

		// event callback
		virtual bool OnEvent( const irr::SEvent& event);
	};
}

