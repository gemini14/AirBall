#include "MainMenuEventReceiver.h"

namespace Tuatara
{

	MainMenuEventReceiver::MainMenuEventReceiver()
	{
	}

	void MainMenuEventReceiver::SetGame( Game *g )
	{
		game = g;
	}

	bool MainMenuEventReceiver::OnEvent( const irr::SEvent& event)
	{
		using namespace irr;


		if( event.EventType == EET_GUI_EVENT )
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch( event.GUIEvent.EventType )
			{
			case gui::EGET_BUTTON_CLICKED:
				// gui button was clicked
				switch( id )
				{
					case MAINMENU_BUTTON_NEWGAME:
						// new game button = move to game state
						game->stateMachine->ChangeState( GameState::Instance() );
						return true;

					case MAINMENU_BUTTON_EXIT:
						// exit button = change to dummy exit state (thus ending the program)
						game->stateMachine->ChangeState( GameExit::Instance() );
						return true;
				default:
					return false;
				}
				break;

			default:
				return false;
			}
		}

		return false;
	}

}