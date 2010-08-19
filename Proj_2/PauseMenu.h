#pragma once

#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "EnumsConstants.h"
#include "Game.h"
#include "PauseMenuEventReceiver.h"
#include "state.h"

namespace Tuatara
{
	class Game;
	class PauseMenuEventReceiver;

	class PauseMenu : public State<Game>, boost::noncopyable
	{
	private:
		PauseMenu(void);
		~PauseMenu(void);

		irr::video::ITexture *menuBackground;

		irr::gui::IGUIButton *returnButton;
		irr::gui::IGUIButton *mainMenuButton;

		std::shared_ptr<PauseMenuEventReceiver> menuEventRcvr;

	public:

		static PauseMenu* Instance();

		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );
	};
}