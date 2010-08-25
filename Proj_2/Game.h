#ifndef GAME_H
#define GAME_H


#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "IrrManager.h"
#include "MainMenu.h"
#include "StateMachine.h"


namespace Tuatara
{
	
	// Wrapper/container class which contains necessary info, FSM, and Irrlicht objects (through the IrrManager)
	class Game : boost::noncopyable
	{
	private:

		irr::u32 windowWidth;
		irr::u32 windowHeight;

		irr::core::rect<irr::s32> windowRect;
	
	public:

		Game( irr::IrrlichtDevice *device, irr::video::IVideoDriver *driver, irr::scene::ISceneManager *smgr,
			irr::gui::IGUIEnvironment *guienv );
		~Game();

		std::shared_ptr<StateMachine<Game>> stateMachine;
		std::shared_ptr<IrrManager> manager;
		
		inline const irr::u32& GetWinWidth() { return windowWidth; }
		inline const irr::u32& GetWinHeight() { return windowHeight; }
		inline const irr::core::rect<irr::s32>& GetWindowRect() { return windowRect; }
		void Update();

		irr::f32 frameDelta;

	};

}

#endif