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

	const irr::core::vector3df GameStateEventReceiver::GetNewCameraTarget( const irr::core::vector3df& currentPos, 
		const irr::core::vector3df& target, const Direction& dir )
	{
		using namespace irr;
		auto newPos = currentPos;
		auto newTarget = target;

		switch( dir )
		{
		case UP:
			if( newTarget.Y < levelSize - 2 )
			{
				newTarget.Y += 0.5f;
			}
			return newTarget;

		case DOWN:
			if( newTarget.Y > -2 )
			{
				newTarget.Y -= 0.5f;
			}
			return newTarget;

		case LEFT:
			newTarget.rotateXZBy( 5.f, currentPos );
			if( newTarget.X < 3 )
			{
				newTarget.X = 2;
			}
			return newTarget;

		case RIGHT:
			newTarget.rotateXZBy( -5.f, currentPos );
			if( newTarget.Z < 3 )
			{
				newTarget.Z = 2;
			}
			return newTarget;

		default:
			return core::vector3df();
		}

		return core::vector3df();
	}

	const irr::core::vector3df GameStateEventReceiver::GetCameraLookVector()
	{
		irr::core::vector3df look( gameState->camera->getTarget() );
		return look.normalize();
	}

	const irr::core::vector3df GameStateEventReceiver::GetCameraUpVector()
	{
		irr::core::vector3df up( gameState->camera->getUpVector() );
		return up.normalize();
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
				gameState->pausedOrExited = true;
				return true;

			case KEY_LEFT:
				gameState->camera->setTarget( GetNewCameraTarget( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), LEFT ) );
				return true;

			case KEY_RIGHT:
				gameState->camera->setTarget( GetNewCameraTarget( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), RIGHT ) );
				return true;

			case KEY_UP:
				gameState->camera->setTarget( GetNewCameraTarget( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), UP ) );
				return true;

			case KEY_DOWN:
				gameState->camera->setTarget( GetNewCameraTarget( gameState->camera->getAbsolutePosition(),
					gameState->camera->getTarget(), DOWN ) );
				return true;

			case KEY_KEY_W:
				gameState->level->physics->ApplyImpulseToBall( FORWARD, 0.f, 0.f, 0.6f);
				return true;

			case KEY_KEY_A:
				gameState->level->physics->ApplyImpulseToBall( LEFT, -0.6f, 0.f, 0.f);
				return true;

			case KEY_KEY_S:
				gameState->level->physics->ApplyImpulseToBall( BACKWARD, 0.f, 0.f, -0.6f);
				return true;

			case KEY_KEY_D:
				gameState->level->physics->ApplyImpulseToBall( RIGHT, 0.6f, 0.f, 0.f);
				return true;

			case KEY_KEY_Q:
				{
					static bool okToExecute = true;
					static u32 then;
					static auto Q_Action = [&]()
					{
						gameState->level->physics->ApplyImpulseToBall( UP, 0, 5.f, 0 );

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