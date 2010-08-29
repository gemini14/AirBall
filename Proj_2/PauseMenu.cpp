#include "PauseMenu.h"
#include "MainMenu.h"


namespace Tuatara
{
	// Constructor:
	PauseMenu::PauseMenu() 
		: ScreenBase()
	{
		// Create buttons:
		returnButton = new GUIButton(PAUSEMENU_BUTTON_RETURN, L"Return to Game", this);
		returnButton->location = irr::core::rect<irr::s32>( 100, 100, 300, 200 );
		
		mainMenuButton = new GUIButton(PAUSEMENU_BUTTON_MAINMENU, L"Main Menu", this);
		mainMenuButton->location = irr::core::rect<irr::s32>( 100, 400, 300, 500 );

		// Add them to list (base class then knows about them)
		buttonList[PAUSEMENU_BUTTON_RETURN] = returnButton;
		buttonList[PAUSEMENU_BUTTON_MAINMENU] = mainMenuButton;

		// path to image for background:
		backgroundPath = "pauseBackground.jpg";

	}

	// return the one and only instance of the pause menu:
	PauseMenu* PauseMenu::Instance()
	{
		static PauseMenu instance;
		return &instance;
	}

	// Button event (call back from GUIButton):
	bool PauseMenu::OnClick(int ID)
	{
		switch (ID)
		{
			case PAUSEMENU_BUTTON_RETURN:
				// return button = move to game state
				game->stateMachine->RevertToPreviousState(); //ChangeState( GameState::Instance() );
				return true;

			case PAUSEMENU_BUTTON_MAINMENU:
				// main menu button = move back to main menu
				game->stateMachine->ChangeState( MainMenu::Instance() );
				return true;

			default:
				return false;
		}
	}

	bool PauseMenu::OnKeyEvent( irr::EKEY_CODE code )
	{
		using namespace irr;

		switch (code)
		{
			case KEY_ESCAPE: // exit game
				game->stateMachine->RevertToPreviousState();
				return true;

			default:
				return false;
		}
	}


	// Destructor:
	PauseMenu::~PauseMenu()
	{
		// clean up buttons:
		delete returnButton;
		delete mainMenuButton;
	}

}