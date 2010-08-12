#ifndef MAINMENU_EVENTRECEIVER_H
#define MAINMENU_EVENTRECEIVER_H


#include <irrlicht.h>

#include "Game.h"
#include "GameExit.h"
#include "GameState.h"
#include "StateMachine.h"


namespace Tuatara
{

	class Game;

	class MainMenuEventReceiver : public irr::IEventReceiver
	{
	private:

		Game *game;

	public:

		MainMenuEventReceiver();

		// the reason this can't go in the constructor is that the MainMenu state doesn't have access to the
		// Game pointer originally
		void SetGame( Game *g );

		// event callback
		virtual bool OnEvent( const irr::SEvent& event);

	};

}

#endif