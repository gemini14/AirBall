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

		// Handle mouse input
		if ( event.EventType == irr::EET_MOUSE_INPUT_EVENT )
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_LEFT_UP: // only case currently handled...
				gameState->level->HandleMouseClick( event.MouseInput.X, event.MouseInput.Y );
				break;

			// could add more:
			//case EMIE_LMOUSE_PRESSED_DOWN:
			//	break;

			//case EMIE_MOUSE_MOVED:
			//	break;

			default:
				// We won't use the wheel
				break;
			}

		}
		// handle keyboard input:
		else if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			switch( event.KeyInput.Key )
			{
			#ifdef _DEBUG
			case KEY_RETURN:
				if ( event.KeyInput.Control )
					gameState->level->EndLevel(true);
				return true;
			#endif
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
					gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_A:
				{
					gameState->level->ApplyImpulseToBall( LEFT );
					gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_S:
				{
					gameState->level->ApplyImpulseToBall( BACKWARD );
					gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_D:
				{
					gameState->level->ApplyImpulseToBall( RIGHT );
					gameState->level->PlayJetSound();
					return true;
				}
			case KEY_KEY_Q:
				{
					static bool keyWasReleased = true;
					
					if( event.KeyInput.PressedDown && keyWasReleased )
					{
						gameState->level->ApplyImpulseToBall( UP, 0, 5.5f, 0 );
						gameState->level->PlayJetSound();
						keyWasReleased = false;
					}
					else if( !event.KeyInput.PressedDown )
					{
						keyWasReleased = true;
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