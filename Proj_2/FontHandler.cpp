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

	FontHandler::FontHandler( const irr::io::path& filename, IrrManager *manager )
		: manager( manager ), ttface( std::make_shared<CGUITTFace>() )
	{
		LoadFontFace( "pala.ttf" );

		ttfont = new CGUIFreetypeFont( manager->driver );
		ttfont->AntiAlias = true;

		ChangeFontSize( 16 );
	}

	FontHandler::~FontHandler()
	{
		ttfont->drop();
	}
}