#include "GameStateEventReceiver.h"

namespace Tuatara
{

	GameStateEventReceiver::GameStateEventReceiver()
	{
	}

	void GameStateEventReceiver::SetGame( Game *g )
	{
		game = g;
	}

	bool GameStateEventReceiver::OnEvent( const irr::SEvent& event)
	{
		using namespace irr;


		if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			switch( event.KeyInput.Key )
			{
			case KEY_ESCAPE:
				// ESC = go to pause menu
				game->stateMachine->ChangeState( PauseMenu::Instance() );
				return true;

			default:
				return false;
			}
		}

		return false;
	}

}