#include "PauseMenuEventReceiver.h"

namespace Tuatara
{
	PauseMenuEventReceiver::PauseMenuEventReceiver(void)
	{
	}

	void PauseMenuEventReceiver::SetGame( Game *g )
	{
		game = g;
	}

	bool PauseMenuEventReceiver::OnEvent( const irr::SEvent& event)
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
					case PAUSEMENU_BUTTON_RETURN:
						// return button = move to game state
						game->stateMachine->ChangeState( GameState::Instance() );
						return true;

					case PAUSEMENU_BUTTON_MAINMENU:
						// main menu button = move back to main menu
						game->stateMachine->ChangeState( MainMenu::Instance() );
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