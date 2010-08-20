#include "GameState.h"


namespace Tuatara
{

	GameState::GameState() : gameEventRcvr( new GameStateEventReceiver ), level( new Level )
	{
	}

	GameState::~GameState()
	{
	}

	GameState* GameState::Instance()
	{
		static GameState instance;
		return &instance;
	}

	void GameState::Enter( Game *game )
	{
		using namespace irr;

		// set event receiver on entry (replaces any current receiver)
		gameEventRcvr->SetGame( game, this );
		game->manager->device->setEventReceiver( gameEventRcvr.get() );

		camera = game->manager->smgr->addCameraSceneNode( 0, core::vector3df( 0, 0, -100 ), core::vector3df( 0, 0, 0 ) );

		level->physics->CreateWorld( game );
		level->physics->CreateBall( game );
	}

	void GameState::Execute( Game *game )
	{
		using namespace irr;

		// make sure device is still operational
		if( game->manager->device->run() && game->manager->driver )
		{
			if( game->manager->device->isWindowActive() )
			{

				level->physics->StepSimulation();
				game->manager->driver->beginScene( true, true, video::SColor(255, 100, 100, 100) );

				game->manager->driver->endScene();
			}
			else
			{
				// if window isn't active, yield resources to OS
				game->manager->device->yield();
			}
		}
	}

	void GameState::Exit( Game *game )
	{
	}

}