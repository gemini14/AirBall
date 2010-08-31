#include "GameState.h"


namespace Tuatara
{

	GameState::GameState() : gameEventRcvr( new GameStateEventReceiver ), currentLevel( 1 )
	{
		using namespace std;

		ifstream config( "levels//config.txt" );
		string property, value;
		config >> property >> value;
		if( property == "[NumberOfLevels]" )
		{
			stringstream temp( value );
			temp >> numberOfLevels;
		}
		else
		{
			numberOfLevels = 0;
			currentLevel = 0;
		}
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

		pausedOrExited = false;

		// set event receiver on entry (replaces any current receiver)
		gameEventRcvr->SetGame( game, this );
		game->manager->device->setEventReceiver( gameEventRcvr.get() );

		camera = game->manager->smgr->addCameraSceneNode( 0, 
			core::vector3df( 1.f, levelSize - 1.f, 1.f ), 
			core::vector3df( levelSize - 1, 1.f, levelSize - 1 ) );

		level = new Level;
		
		auto levelFilenameGenerator = []( int currentLevel )->std::string
		{
			std::stringstream filename;
			filename << "level_" << currentLevel << ".xml";
			return filename.str();
		};
		bool levelCreatedOK = level->InitLevel( game->manager->smgr, game->manager->device->getFileSystem(),
			levelFilenameGenerator( currentLevel ), game->manager->driver->getTexture( "media/bubbleWater.jpg" ),
			game->manager->driver->getTexture( "media/tuatara.jpg" ) );

		if( !levelCreatedOK )
		{
			game->stateMachine->ChangeState( MainMenu::Instance() );
		}
	}

	void GameState::Execute( Game *game )
	{
		using namespace irr;

		static u32 then = game->manager->device->getTimer()->getTime();
		auto calcTimeDelta = [&]( irr::f32 then, irr::f32 now ) -> irr::f32
				{
					irr::f32 delta = static_cast<irr::f32>( now - then ) / 1000.f;
					then = now;
					return delta; 
				};

		//u32 now = game->manager->device->getTimer()->getTime();
		//game->frameDelta = calcTimeDelta( then, game->manager->device->getTimer()->getTime() );

		level->StepSimulation();

		game->manager->driver->beginScene( true, true, video::SColor(255, 100, 101, 140)/*255, 100, 100, 100)*/ );

		game->manager->smgr->drawAll();

		game->manager->driver->endScene();
	}

	void GameState::Exit( Game *game )
	{
		delete level;
	}

}