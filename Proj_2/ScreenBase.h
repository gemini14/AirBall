#ifndef SCREENBASE_H
#define SCREENBASE_H

#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <map>
#include <memory>
#include <string>

#include "EnumsConstants.h"
#include "Game.h"
#include "GUIButton.h"
#include "State.h"


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
		ScreenBase();
		~ScreenBase();

		std::string backgroundPath; // derived class sets this


		// will be called when a button is clicked
		virtual bool OnClick( int ID ) = 0;

		virtual bool OnKeyEvent( irr::EKEY_CODE code ) = 0;

		// IEventReceiver, event callback:
		virtual bool OnEvent( const irr::SEvent& event);

		// State<Game> functions:
		virtual void Enter( Game* game );
		virtual void Execute( Game* game );
		virtual void Exit( Game* game );
	};
}

#endif