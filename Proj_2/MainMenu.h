#ifndef MAINMENU_H
#define MAINMENU_H

#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "ScreenBase.h"


namespace Tuatara
{
	class LightSoundSystem;

	class MainMenu : public ScreenBase
	{

	private:

		MainMenu();
		~MainMenu();

		GUIButton *exitButton;
		GUIButton *newGameButton;
		
		std::unique_ptr<LightSoundSystem> soundSystem;

	public:

		static MainMenu* Instance();

		virtual void Enter( Game *game );
		virtual void Execute( Game *game );
		virtual void Exit( Game *game );

		// will be called when a button is clicked
		virtual bool OnClick( int ID );

		// will be called when a key is pressed:
		virtual bool OnKeyEvent( irr::EKEY_CODE code );

		void Pause( bool pause );
	};
}

#endif