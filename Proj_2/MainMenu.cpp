#include "EnumsConstants.h"
#include "GameExit.h"
#include "GameState.h"
#include "GUIButton.h"
#include "LightSoundSystem.h"
#include "MainMenu.h"


namespace Tuatara
{
	MainMenu::MainMenu() : ScreenBase(), soundSystem ( new LightSoundSystem( "sound/menuBackMusic.ogg" ) )
	{
		// Create buttons:
		newGameButton = new GUIButton(MAINMENU_BUTTON_NEWGAME, L"New Game", this);
		newGameButton->location = irr::core::rect<irr::s32>( 800, 500, 1000, 600 );
		
		exitButton = new GUIButton(MAINMENU_BUTTON_EXIT, L"Exit Game", this);
		exitButton->location = irr::core::rect<irr::s32>( 800, 650, 1000, 750 );

		// Add them to list (base class then knows about them)
		buttonList[MAINMENU_BUTTON_NEWGAME] = newGameButton;
		buttonList[MAINMENU_BUTTON_EXIT] = exitButton;

		backgroundPath = "menuBackground.png";

	}

	// return the one and only instance of the pause menu:
	MainMenu* MainMenu::Instance()
	{
		static MainMenu instance;
		return &instance;
	}

	void MainMenu::Enter( Game *game )
	{
		soundSystem->StartPlayback( true );
		soundSystem->AdjustVolume( 0.1f );
		ScreenBase::Enter( game );
	}

	void MainMenu::Execute( Game *game )
	{
		soundSystem->Update();
		ScreenBase::Execute( game );
	}

	void MainMenu::Exit( Game *game )
	{
		soundSystem->StopPlayback();
		ScreenBase::Exit( game );
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
			// exit game
			case KEY_ESCAPE: 
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

	void MainMenu::Pause( bool pause )
	{
		if( pause )
		{
			soundSystem->PausePlayback();
		}
		else
		{
			soundSystem->ResumePlayback();
		}
	}

	MainMenu::~MainMenu()
	{
		// clean up buttons:
		delete newGameButton;
		delete exitButton;
	}
}