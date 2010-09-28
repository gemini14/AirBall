#include "Credits.h"


#include "MainMenu.h"


namespace Tuatara
{
	Credits::Credits() : ScreenBase()
	{
		backgroundPath = "creditsScreen.png";
	}

	Credits* Credits::Instance()
	{
		static Credits instance;
		return &instance;
	}

	void Credits::Enter( Game *game )
	{
		ScreenBase::Enter( game );
	}

	void Credits::Execute( Game *game )
	{
		ScreenBase::Execute( game );
	}

	void Credits::Exit( Game *game )
	{
		using namespace irr;

		ScreenBase::Exit( game );
	}

	bool Credits::OnClick( int ID )
	{
		return false;
	}

	bool Credits::OnKeyEvent( irr::EKEY_CODE code )
	{
		using namespace irr;

		switch( code )
		{
		case KEY_RETURN:
			game->stateMachine->ChangeState( MainMenu::Instance() );
			return true;
		default:
			return false;
		}
		return false;
	}

	Credits::~Credits()
	{
	}
}