#include "ScreenBase.h"

namespace Tuatara
{
	class Game;
	class GUIButton;

	ScreenBase::ScreenBase(void) : menuBackground( nullptr )
	{
	}


	ScreenBase::~ScreenBase(void)
	{
	}

	void ScreenBase::Enter(Game *game)
	{
		using namespace irr;

		// set event receiver on entry (replaces any current receiver)
		this->game = game;
		game->manager->device->setEventReceiver( this );

		// if menu background texture hasn't been loaded, load it
		if( menuBackground == nullptr )
		{
			menuBackground = game->manager->driver->getTexture( backgroundPath.c_str() );
		}

		// set up font for GUI
		game->manager->font->ChangeFontSize( 16 );
		game->manager->font->ChangeFontForElement( gui::EGDF_TOOLTIP );
		game->manager->font->ChangeFontColor( gui::EGDC_BUTTON_TEXT, video::SColor( 255, 255, 255, 255 ) );

		// Add buttons to GUI:
		for(auto it = buttonList.cbegin(); it != buttonList.cend(); ++it)
		{
			it->second->AddToGUI( game );
		}

	}

	void ScreenBase::Execute(Game *game)
	{
		using namespace irr;

		game->manager->driver->beginScene();

		// draw the background image
		game->manager->driver->draw2DImage( menuBackground, game->GetWindowRect(),
			core::rect<s32>( 0, 0, menuBackground->getOriginalSize().Width, menuBackground->getOriginalSize().Height) );

		// draw the GUI
		game->manager->guienv->drawAll();

		game->manager->driver->endScene();
	}

	void ScreenBase::Exit(Game *game)
	{
		// clear buttons from GUI before changing state
		for(auto it = buttonList.cbegin(); it != buttonList.cend(); ++it)
		{
			it->second->RemoveFromGUI();
		}
	}

	bool ScreenBase::OnEvent( const irr::SEvent& event)
	{
		using namespace irr;
		
		if( event.EventType == EET_GUI_EVENT )
		{
			s32 id = event.GUIEvent.Caller->getID();

			// find correct button and handle the event:
			auto iter = buttonList.find(id);
			if ( iter != buttonList.end() )
			{
				return iter->second->HandleEvent( event );
			}
		}
		else if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			if ( event.KeyInput.PressedDown == true )
			{
				// pass key event to derived class
				return this->OnKeyEvent( event.KeyInput.Key );
			}
		}

		return false;
	}
}
