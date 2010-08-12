#include "Game.h"


namespace Tuatara
{

	Game::Game( Tuatara::IrrManager *manager )
		: manager( manager )
	{
		// set the current window width and height (this could be pulled out later into a function if needed)
		windowWidth = manager->driver->getScreenSize().Width;
		windowHeight = manager->driver->getScreenSize().Height;
		windowRect.addInternalPoint( windowWidth, windowHeight );

		// add zip archive of media files to Irr device's file system
		manager->device->getFileSystem()->addFileArchive( "media.zip", true, true );

		// create the FSM
		stateMachine = new StateMachine<Game>( this );
		stateMachine->ChangeState( MainMenu::Instance() );
	}

	Game::~Game()
	{
		delete stateMachine;
		delete manager;
	}

	void Game::Update()
	{
		if( stateMachine != nullptr )
		{
			// run the FSM
			stateMachine->Update();
		}
	}

}