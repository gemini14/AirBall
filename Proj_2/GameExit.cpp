#include "GameExit.h"

namespace Tuatara
{

	GameExit::GameExit()
	{
	}

	GameExit* GameExit::Instance()
	{
		static GameExit instance;
		return &instance;
	}

	void GameExit::Enter(Game *game)
	{
	}

	void GameExit::Execute(Game *game)
	{
	}

	void GameExit::Exit(Game *game)
	{

	}

}