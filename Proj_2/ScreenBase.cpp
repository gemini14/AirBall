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

		// setup font for GUI:
		gui::IGUISkin* skin = game->manager->guienv->getSkin();
        gui::IGUIFont* font = game->manager->guienv->getFont("media/fonthaettenschweiler.bmp");
		if (font) { skin->setFont(font); }
		skin->setFont(game->manager->guienv->getBuiltInFont(), gui::EGDF_TOOLTIP);
		skin->setColor(gui::EGUI_DEFAULT_COLOR::EGDC_BUTTON_TEXT,video::SColor(255, 255, 255, 255));


		// Add buttons to GUI:
	    for(ButtonMap::const_iterator it = buttonList.begin(); it != buttonList.end(); ++it)
		{
			it->second->AddToGUI( game );
		}

	}

	void ScreenBase::Execute(Game *game)
	{
		using namespace irr;

		// make sure device is still operational
		if( game->manager->device->run() && game->manager->driver )
		{
			if( game->manager->device->isWindowActive() )
			{
				game->manager->driver->beginScene();

				// draw the background image
				game->manager->driver->draw2DImage( menuBackground, game->GetWindowRect(),
					core::rect<s32>( 0, 0, menuBackground->getOriginalSize().Width, menuBackground->getOriginalSize().Height) );

				// draw the GUI
				game->manager->guienv->drawAll();

				game->manager->driver->endScene();
			}
			else
			{
				// if window isn't active, yield resources to OS
				game->manager->device->yield();
			}
		}
	}

	void ScreenBase::Exit(Game *game)
	{
		// clear buttons from GUI before changing state
		for(ButtonMap::const_iterator it = buttonList.begin(); it != buttonList.end(); ++it)
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
			ButtonMap::iterator iter = buttonList.find(id);
			if ( iter != buttonList.end() )
			{
				return iter->second->HandleEvent( event );
			}
		}
		return false;
	}

}
