#include "GameState.h"


namespace Tuatara
{

	GameState::GameState() : gameEventRcvr( new GameStateEventReceiver ), currentLevel( 0 )
	{
		using namespace std;

		// get the total number of levels from the config.txt file (invalid file will return user to menu)
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

		paused = false;

		// set event receiver on entry (replaces any current receiver)
		gameEventRcvr->SetGame( game, this );
		game->manager->device->setEventReceiver( gameEventRcvr.get() );

		level = new Level;
		
		// returns the correct filename for the level file based on which level the player is on
		auto levelFilenameGenerator = []( int currentLevel )->std::string
		{
			std::stringstream filename;
			filename << "level_" << currentLevel << ".xml";
			return filename.str();
		};

		bool levelCreatedOK = level->InitLevel( game->manager->smgr, game->manager->device->getFileSystem(),
			levelFilenameGenerator( currentLevel ), game->manager->driver->getTexture( "tile.png" ),
			game->manager->driver->getTexture( "ball.png" ), game->manager->driver->getTexture( "exit.png" ),
			game->manager->driver->getTexture( "tileVent.png" ), 
			game->manager->driver->getTexture( "ventSmoke.png" ), game->manager->driver->getTexture( "tileTransparent.png" ) );

		if( !levelCreatedOK )
		{
			ResetGameState( game );
		}
	}

	void GameState::Execute( Game *game )
	{
		using namespace irr;

		bool levelComplete;

		levelComplete = level->StepSimulation();

		game->manager->driver->beginScene( true, true, video::SColor(255, 100, 101, 140) );
			// draw 3D Scene:
			game->manager->smgr->drawAll();
			// draw GUI (if there is any):
			game->manager->device->getGUIEnvironment()->drawAll();
		game->manager->driver->endScene();

		if( levelComplete )
		{
			// if the player finished the level, advance to the next level if the player hasn't completed
			// all the levels
			if( ++currentLevel != numberOfLevels )
			{
				Exit( game );
				Enter( game );
			}
			// otherwise go back to the main menu
			else
			{
				ResetGameState( game );
			}
		}
	}

	void GameState::Exit( Game *game )
	{
		delete level;
	}

	void GameState::ResetGameState( Game *game )
	{
		currentLevel = 0;
		game->stateMachine->ChangeState( MainMenu::Instance() );
	}

	void GameState::Pause( bool pause )
	{
		paused = pause;
		if( paused )
		{
			level->Pause( paused );
		}
		else
		{
			level->Pause( paused );
		}
	}
}