#ifndef GUIBUTTON_H
#define GUIBUTTON_H


#include <irrlicht.h>
#include <string>

#include "Game.h"
#include "ScreenBase.h"


namespace Tuatara
{
	class Game;
	class ScreenBase;
	
	class GUIButton
	{
	private:

		irr::gui::IGUIButton* irrButton;
		ScreenBase* screen;
		Game* game;

	public:

		GUIButton( int ID, std::wstring text, ScreenBase* screen );
		~GUIButton();

		std::string pathEnabledTexture;
		std::string pathHoverEnabledTexture;

		int ID;
		std::wstring text;
		irr::core::rect<irr::s32> location;

		void AddToGUI( Game* game );
		void RemoveFromGUI();		
		bool HandleEvent( const irr::SEvent& event );
	};
}

#endif