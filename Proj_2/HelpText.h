#ifndef HELPTEXT_H
#define HELPTEXT_H

#include <stdlib.h>
#include <string>

#include <boost/noncopyable.hpp>
#include <irrlicht.h>


namespace Tuatara
{

	class HelpText : boost::noncopyable
	{
	
		irr::gui::IGUIStaticText *helpText;

	public:
		
		HelpText( irr::gui::IGUIEnvironment *guienv, int x0, int y0, int x1, int y1, const std::string& text );
		~HelpText();
	};

}

#endif