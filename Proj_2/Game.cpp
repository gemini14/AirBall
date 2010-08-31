#include "Game.h"
#include "MainMenu.h"


namespace Tuatara
{

	Game::Game( irr::IrrlichtDevice *device, irr::video::IVideoDriver *driver, irr::scene::ISceneManager *smgr,
			irr::gui::IGUIEnvironment *guienv )
			: manager( std::make_shared<IrrManager>( device, driver, smgr, guienv ) )
	{
		// set the current window width and height (this could be pulled out later into a function if needed)
		windowWidth = manager->driver->getScreenSize().Width;
		windowHeight = manager->driver->getScreenSize().Height;
		windowRect.addInternalPoint( windowWidth, windowHeight );

		// add zip archive of media files to Irr device's file system - this will be used at the end
		// to speed up asset loading and to save space
		//manager->device->getFileSystem()->addFileArchive( "media.zip", true, true );
		manager->device->getFileSystem()->addFileArchive( "media\\" );
		manager->device->getFileSystem()->addFileArchive( "levels\\" );

		// create the FSM
		stateMachine = std::make_shared<StateMachine<Game>>( this );
		stateMachine->ChangeState( MainMenu::Instance() );
	}

	Game::~Game()
	{
	}

	void Game::Update()
	{
		if( stateMachine.get() != nullptr )
		{
			// run the FSM
			stateMachine->Update();
		}
	}

}