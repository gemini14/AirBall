#include "MainMenu.h"
#include "GameState.h"
#include "GameExit.h"

namespace Tuatara
{
	// Constructor:
	MainMenu::MainMenu() 
		: ScreenBase()
	{
		// Create buttons:
		newGameButton = new GUIButton(MAINMENU_BUTTON_NEWGAME, L"New Game", this);
		newGameButton->location = irr::core::rect<irr::s32>( 200, 600, 400, 700 );
		
		exitButton = new GUIButton(MAINMENU_BUTTON_EXIT, L"Exit Game", this);
		exitButton->location = irr::core::rect<irr::s32>( 624, 600, 824, 700 );

		// Add them to list (base class then knows about them)
		buttonList[MAINMENU_BUTTON_NEWGAME] = newGameButton;
		buttonList[MAINMENU_BUTTON_EXIT] = exitButton;

		backgroundPath = "menuBackground.jpg";

	}

	// return the one and only instance of the pause menu:
	MainMenu* MainMenu::Instance()
	{
		static MainMenu instance;
		return &instance;
	}

	// Button event (call back from GUIButton):
	bool MainMenu::OnClick(int ID)
	{
		switch (ID)
		{
			case MAINMENU_BUTTON_NEWGAME:
				// new game button = move to game state
				game->stateMachine->ChangeState( GameState::Instance() );
				return true;

			case MAINMENU_BUTTON_EXIT:
				// exit button = exit game
				game->stateMachine->ChangeState( GameExit::Instance() );
				return true;

			default:
				return false;
		}
	}

	bool MainMenu::OnKeyEvent( irr::EKEY_CODE code )
	{
		using namespace irr;

		switch (code)
		{
			case KEY_ESCAPE: // exit game
				game->stateMachine->ChangeState( GameExit::Instance() );
				return true;

			case KEY_F2:
				// new game button = move to game state
				game->stateMachine->ChangeState( GameState::Instance() );
				return true;

			default:
				return false;
		}
	}

	// Destructor:
	MainMenu::~MainMenu()
	{
		// clean up buttons:
		delete newGameButton;
		delete exitButton;
	}
}