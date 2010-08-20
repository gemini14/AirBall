#include "GameStateEventReceiver.h"

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

	const irr::core::vector3df GameStateEventReceiver::GetNewCameraPosition( const irr::core::vector3df& currentPos, 
		const irr::core::vector3df& target, const Direction& dir )
	{
		irr::core::vector3df newPos = currentPos;

		switch( dir )
		{
		case UP:
			break;
		
		case DOWN:
			
			break;
		
		case LEFT:
			newPos.rotateXZBy( -10.f, target );
			break;
		
		case RIGHT:
			newPos.rotateXZBy( 10.f, target );
			break;

		default:
			return irr::core::vector3df();
		}

		return newPos;
	}

	bool GameStateEventReceiver::OnEvent( const irr::SEvent& event)
	{
		using namespace irr;


		if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			switch( event.KeyInput.Key )
			{
			case KEY_ESCAPE:
				// ESC = go to pause menu
				game->stateMachine->ChangeState( PauseMenu::Instance() );
				return true;
			case KEY_LEFT:
				gameState->camera->setTarget( GetNewCameraPosition( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), LEFT ) );
				break;
			case KEY_RIGHT:
				gameState->camera->setTarget( GetNewCameraPosition( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), RIGHT ) );
				break;
			case KEY_UP:
				gameState->camera->setTarget( GetNewCameraPosition( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), UP ) );
				break;
			case KEY_DOWN:
				gameState->camera->setTarget( GetNewCameraPosition( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), DOWN ) );
				break;

			case KEY_KEY_W:
				// this only applies an upward impulse at the moment...more to come
				gameState->level->physics->ApplyImpulseToBall( LEFT );
				break;
			default:
				return false;
			}
		}

		return false;
	}

}