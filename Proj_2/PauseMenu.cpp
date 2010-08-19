#include "PauseMenu.h"


namespace Tuatara
{

	PauseMenu::PauseMenu() : menuBackground( nullptr ), menuEventRcvr( new PauseMenuEventReceiver )
	{
	}

	PauseMenu* PauseMenu::Instance()
	{
		static PauseMenu instance;
		return &instance;
	}

	void PauseMenu::Enter(Game *game)
	{
		using namespace irr;

		// set event receiver on entry (replaces any current receiver)
		menuEventRcvr->SetGame( game );
		game->manager->device->setEventReceiver( menuEventRcvr.get() );

		// if menu background texture hasn't been loaded, load it
		if( menuBackground == nullptr )
		{
			menuBackground = game->manager->driver->getTexture( "pauseBackground.jpg" );
		}

		// add new game button to gui environment, set its image, scale it, and turn on alpha
		// button ID is found in EnumsConstants.h
		returnButton = game->manager->guienv->addButton( 
			core::rect<s32>( game->GetWinWidth() / 4 - 50, game->GetWinHeight() * 0.25,
			game->GetWinWidth() / 4 + 50, game->GetWinHeight() * 0.25 + 100 ), 0, PAUSEMENU_BUTTON_RETURN );
		returnButton->setImage( game->manager->driver->getTexture( "returnToGameButton.png" ) );
		returnButton->setScaleImage( true );
		returnButton->setUseAlphaChannel( true );
		
		// add exit button to gui environment, set its image, scale it, and turn on alpha
		// button ID is found in EnumsConstants.h
		mainMenuButton = game->manager->guienv->addButton( 
			core::rect<s32>( game->GetWinWidth() / 4 - 50, game->GetWinHeight() * 0.75,
			game->GetWinWidth() / 4 + 50, game->GetWinHeight() * 0.75 + 100 ), 0, PAUSEMENU_BUTTON_MAINMENU );
		mainMenuButton->setImage( game->manager->driver->getTexture( "mainMenuButton.png" ) );
		mainMenuButton->setScaleImage( true );
		mainMenuButton->setUseAlphaChannel( true );
	}

	void PauseMenu::Execute(Game *game)
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
					core::rect<s32>( 0, 0, menuBackground->getOriginalSize().Width, menuBackground->getOriginalSize().Height) );

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

	void PauseMenu::Exit(Game *game)
	{
		// clear buttons from GUI before changing state
		returnButton->remove();
		mainMenuButton->remove();
	}

	PauseMenu::~PauseMenu()
	{
	}

}
