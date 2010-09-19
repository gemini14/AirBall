#include "GameStateEventReceiver.h"
#include "PauseMenu.h"

namespace Tuatara
{

	GameStateEventReceiver::GameStateEventReceiver()
	{
	}

	void GameStateEventReceiver::SetGame( Game *g, GameState *gState )
	{
		game = g;
		gameState = gState;
	}

	bool GameStateEventReceiver::OnEvent( const irr::SEvent& event)
	{
		using namespace irr;


		if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			switch( event.KeyInput.Key )
			{
			case KEY_ESCAPE:
				game->stateMachine->ChangeState( MainMenu::Instance() );
				return true;

			case KEY_LEFT:
				gameState->level->ApplyDirectionToCamera( LEFT );
				return true;

			case KEY_RIGHT:
				gameState->level->ApplyDirectionToCamera( RIGHT );
				return true;

			case KEY_UP:
				gameState->level->ApplyDirectionToCamera( UP );
				return true;

			case KEY_DOWN:
				gameState->level->ApplyDirectionToCamera( DOWN );
				return true;

			case KEY_KEY_W:
				{
					gameState->level->ApplyImpulseToBall( FORWARD );
					if (!gameState->level->IsJetPlaying())
						gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_A:
				{
					gameState->level->ApplyImpulseToBall( LEFT );
					if (!gameState->level->IsJetPlaying())
						gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_S:
				{
					gameState->level->ApplyImpulseToBall( BACKWARD );
					if (!gameState->level->IsJetPlaying())
						gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_D:
				{
					gameState->level->ApplyImpulseToBall( RIGHT );
					if (!gameState->level->IsJetPlaying())
						gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_Q:
				if( !event.KeyInput.PressedDown )
				{
					static bool okToExecute = true;
					static u32 then;
					static auto Q_Action = [&]()
					{
						gameState->level->ApplyImpulseToBall( UP, 0, 5.5f, 0 );
						gameState->level->PlayJetSound();

						then = game->manager->device->getTimer()->getTime();
						okToExecute = !okToExecute;
					};

					if( okToExecute )
					{
						Q_Action();
					}
					else
					{
						u32 now = game->manager->device->getTimer()->getTime();
						if( (now - then) / 1000.f >= 1.5f )
						{
							Q_Action();
						}
					}
				}
				return true;
			default:
				return false;
			}
		}

		return false;
	}

}