#include "GUIButton.h"

namespace Tuatara
{
	GUIButton::GUIButton( int ID, std::wstring text, ScreenBase* screen ) 
		: location(0,0,0,0), screen( screen ), ID( ID ), text( text )
	{
		pathEnabledTexture = "defaultButton.png";
		pathHoverEnabledTexture = "defaultHoverButton.png";
	}


	GUIButton::~GUIButton(void)
	{
	}

	void GUIButton::AddToGUI( Game* game )
	{
		using namespace irr;

		this->game = game;

		irrButton = game->manager->guienv->addButton( location, 0, ID, text.c_str() );
		irrButton->setImage( game->manager->driver->getTexture( pathEnabledTexture.c_str() ) );
		irrButton->setScaleImage( true );
		irrButton->setUseAlphaChannel( true );
	}

	void GUIButton::RemoveFromGUI( void )
	{
		irrButton->remove();
	}
		
	bool GUIButton::HandleEvent( const irr::SEvent& event )
	{
		using namespace irr;
		
		switch( event.GUIEvent.EventType )
		{
			case gui::EGET_BUTTON_CLICKED:
				// gui button was clicked
				return screen->OnClick( ID );
				break;

			case gui::EGET_ELEMENT_HOVERED:
				irrButton->setImage( game->manager->driver->getTexture( pathHoverEnabledTexture.c_str() ) );
				return true;
				break;

			case gui::EGET_ELEMENT_LEFT:
				irrButton->setImage( game->manager->driver->getTexture( pathEnabledTexture.c_str() ) );
				return true;
				break;

			default:
				return false;
				break;
		}
	}
}