#ifndef GAME_H
#define GAME_H


#include <boost/noncopyable.hpp>

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

		Game( Tuatara::IrrManager *manager );
		~Game();

		StateMachine<Game> *stateMachine;
		IrrManager *manager;
		
		inline irr::u32 GetWinWidth() { return windowWidth; }
		inline irr::u32 GetWinHeight() { return windowHeight; }
		inline irr::core::rect<irr::s32> GetWindowRect() { return windowRect; }
		void Update();		

	};

}

#endif