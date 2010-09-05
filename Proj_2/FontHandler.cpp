#include "FontHandler.h"

namespace Tuatara
{
	bool FontHandler::LoadFontFace( const irr::io::path& filename )
	{
		return ( ttface->load( filename ) ? true : false );
	}

	bool FontHandler::ChangeFontSize( irr::u32 sizeInPoints )
	{
		if( ttfont->attach( ttface.get(), sizeInPoints ) )
		{
			irr::gui::IGUISkin *skin = manager->guienv->getSkin();
			skin->setFont( ttfont );
			return true;
		}

		return false;
	}

	void FontHandler::ChangeFontForElement( irr::gui::EGUI_DEFAULT_FONT which )
	{
		irr::gui::IGUISkin *skin = manager->guienv->getSkin();
		skin->setFont( ttfont, which );
	}

	void FontHandler::ChangeFontColor( irr::gui::EGUI_DEFAULT_COLOR which, irr::video::SColor color )
	{
		irr::gui::IGUISkin *skin = manager->guienv->getSkin();
		skin->setColor( which, color );
	}

	FontHandler::FontHandler( const irr::io::path& filename, IrrManager *manager )
		: manager( manager ), ttface( std::make_shared<CGUITTFace>() )
	{
		LoadFontFace( "pala.ttf" );

		ttfont = new CGUIFreetypeFont( manager->driver );
		ttfont->AntiAlias = true;

		// this sets the default font size for GUI objects
		ChangeFontSize( 16 );
	}

	FontHandler::~FontHandler()
	{
		ttfont->drop();
	}
}