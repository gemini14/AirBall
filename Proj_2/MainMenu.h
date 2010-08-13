#ifndef MENUSTATES_H
#define MENUSTATES_H


#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "EnumsConstants.h"
#include "Game.h"
#include "MainMenuEventReceiver.h"
#include "State.h"


namespace Tuatara
{

	class Game;
	class MainMenuEventReceiver;
	
	// MainMenu state
	class MainMenu : public State<Game>, boost::noncopyable
	{

	private:

		MainMenu();
		~MainMenu();

		irr::video::ITexture *menuBackground;

		irr::gui::IGUIButton *newGameButton;
		irr::gui::IGUIButton *exitButton;

		std::shared_ptr<MainMenuEventReceiver> menuEventRcvr;

	public:

		static MainMenu* Instance();

		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );

	};

}

#endif