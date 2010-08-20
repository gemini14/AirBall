#include "MainMenu.h"


namespace Tuatara
{

	MainMenu::MainMenu() : menuBackground( nullptr ), menuEventRcvr( new MainMenuEventReceiver )
	{
	}

	MainMenu* MainMenu::Instance()
	{
		static MainMenu instance;
		return &instance;
	}

	void MainMenu::Enter(Game *game)
	{
		using namespace irr;

		// set event receiver on entry (replaces any current receiver)
		menuEventRcvr->SetGame( game );
		game->manager->device->setEventReceiver( menuEventRcvr.get() );

		// if menu background texture hasn't been loaded, load it
		if( menuBackground == nullptr )
		{
			menuBackground = game->manager->driver->getTexture( "menuBackground.jpg" );
		}

		// add new game button to gui environment, set its image, scale it, and turn on alpha
		// button ID is found in EnumsConstants.h
		newGameButton = game->manager->guienv->addButton( 
			core::rect<s32>( game->GetWinWidth() / 3 - 50, game->GetWinHeight() * 0.85,
			game->GetWinWidth() / 3 + 50, game->GetWinHeight() * 0.95 ), 0, MAINMENU_BUTTON_NEWGAME );
		newGameButton->setImage( game->manager->driver->getTexture( "newGameButton.png" ) );
		newGameButton->setScaleImage( true );
		newGameButton->setUseAlphaChannel( true );
		
		// add exit button to gui environment, set its image, scale it, and turn on alpha
		// button ID is found in EnumsConstants.h
		exitButton = game->manager->guienv->addButton( 
			core::rect<s32>( (game->GetWinWidth() * 2) / 3 - 50, game->GetWinHeight() * 0.85,
			(game->GetWinWidth() * 2) / 3 + 50, game->GetWinHeight() * 0.95 ), 0, MAINMENU_BUTTON_EXIT );
		exitButton->setImage( game->manager->driver->getTexture( "exitButton.png" ) );
		exitButton->setScaleImage( true );
		exitButton->setUseAlphaChannel( true );

		game->manager->font->ChangeFontSize( 72 );
		gameTitle = game->manager->guienv->addStaticText( L"Air Ball", 
			core::rect<s32>( game->GetWinWidth() / 2 - 150, 0, game->GetWinWidth() / 2 + 150, 100), 
			false, true, 0, MAINMENU_TITLE );
		gameTitle->setAlignment( gui::EGUIA_CENTER, gui::EGUIA_CENTER, gui::EGUIA_CENTER, gui::EGUIA_CENTER );
	}

	void MainMenu::Execute(Game *game)
	{
		using namespace irr;

		// make sure device is still operational
		if( game->manager->device->run() && game->manager->driver )
		{
			if( game->manager->device->isWindowActive() )
			{
				game->manager->driver->beginScene();

				// draw the background image
				game->manager->driver->draw2DImage( menuBackground, game->GetWindowRect(),
					core::rect<s32>( 0, 0, menuBackground->getOriginalSize().Width,
					menuBackground->getOriginalSize().Height) );

				// draw the GUI
				game->manager->guienv->drawAll();

				game->manager->driver->endScene();
			}
			else
			{
				// if window isn't active, yield resources to OS
				game->manager->device->yield();
			}
		}
	}

	void MainMenu::Exit(Game *game)
	{
		// clear buttons from GUI before changing state
		newGameButton->remove();
		exitButton->remove();
		gameTitle->remove();
	}

	MainMenu::~MainMenu()
	{
	}

}