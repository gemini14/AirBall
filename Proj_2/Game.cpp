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
		// set the window rect to the current window size
		windowRect.addInternalPoint( windowWidth, windowHeight );

		// add archives to Irr device's file system
		/*
		// this is for debugging and development, not final release
		manager->device->getFileSystem()->addFileArchive( "art\\" );
		manager->device->getFileSystem()->addFileArchive( "levels\\" );
		*/
		manager->device->getFileSystem()->addFileArchive( "art.zip" );
		manager->device->getFileSystem()->addFileArchive( "levels.zip" );

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