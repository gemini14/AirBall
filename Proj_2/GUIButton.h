#pragma once
#include <irrlicht.h>
#include <string>

#include "Game.h"
#include "ScreenBase.h"

namespace Tuatara
{
	class ScreenBase;
	class Game;

	class GUIButton
	{
	private:
		irr::gui::IGUIButton* irrButton;
		ScreenBase* screen;
		Game* game;

	public:
		GUIButton( int ID, std::wstring text, ScreenBase* screen );
		~GUIButton( void );

		std::string pathEnabledTexture;
		std::string pathHoverEnabledTexture;

		// TODO: Should change these to make them accessors:
		int ID;
		std::wstring text;
		irr::core::rect<irr::s32> location;

		void AddToGUI( Game* game );
		void RemoveFromGUI( void );
		
		bool HandleEvent( const irr::SEvent& event );
	};
}