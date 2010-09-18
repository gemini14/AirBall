#include "HelpText.h"

namespace Tuatara
{
	HelpText::HelpText( irr::gui::IGUIEnvironment *guienv, int x0, int y0, int x1, int y1, const std::string& text )
		: helpText( nullptr )
	{
		using namespace irr;
		int size = mbstowcs( nullptr, text.c_str(), 0 );
		if( size != -1 )
		{
			wchar_t *convertedText = new wchar_t[ size + 1 ];
			mbstowcs( convertedText, text.c_str(), size + 1 );
			helpText = guienv->addStaticText( convertedText, core::rect<s32>( x0, y0, x1, y1 ) );
			delete[] convertedText;
		}
	}

	HelpText::~HelpText()
	{
		if( helpText != nullptr )
		{
			helpText->remove();
		}
	}
}