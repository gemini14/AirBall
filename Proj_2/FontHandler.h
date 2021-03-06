#ifndef FONTHANDLER_H
#define FONTHANDLER_H


#include <memory>

#include "IrrManager.h"
#include "TTF_Irr.h"


namespace Tuatara
{

	class IrrManager;

	class FontHandler
	{
	private:

		std::shared_ptr<CGUITTFace> ttface;
		CGUIFreetypeFont *ttfont;

		IrrManager *manager;

		bool LoadFontFace( const irr::io::path& filename );

	public:

		FontHandler( const irr::io::path& filename, IrrManager *manager );
		~FontHandler();

		bool ChangeFontSize( irr::u32 sizeInPoints );
		void ChangeFontForElement( irr::gui::EGUI_DEFAULT_FONT which = irr::gui::EGDF_DEFAULT );
		void ChangeFontColor( irr::gui::EGUI_DEFAULT_COLOR which, irr::video::SColor color );
	};

}

#endif