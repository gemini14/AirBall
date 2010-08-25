#pragma once

#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>
#include <string>
#include <map>

#include "EnumsConstants.h"
#include "Game.h"
#include "State.h"
#include "GUIButton.h"

namespace Tuatara
{
	class Game;
	class GUIButton;

	class ScreenBase : public State<Game>, public irr::IEventReceiver, boost::noncopyable
	{
	protected:

		Game *game;
		irr::video::ITexture *menuBackground;
		
		typedef std::map<int, GUIButton*> ButtonMap;
		ButtonMap buttonList; // derived class add buttons here

	public:
		ScreenBase(void);
		~ScreenBase(void);

		std::string backgroundPath; // derived class sets this
				
		// Not sure how to make this forced...
		// virtual ScreenBase* Instance() = 0;

		// will be called when a button is clicked
		virtual bool OnClick( int ID ) = 0;

		// IEventReceiver, event callback:
		virtual bool OnEvent( const irr::SEvent& event);

		// State<Game> functions:
		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );
	};
}
