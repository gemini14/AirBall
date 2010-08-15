#ifndef IRRMANAGER_H
#define IRRMANAGER_H


#include <boost/utility.hpp>
#include <irrlicht.h>
#include <memory>

#include "FontHandler.h"


namespace Tuatara
{
	
	class FontHandler;

	// container class for the big four irrlicht obj's and TrueType font (for adjusting font size)
	class IrrManager : boost::noncopyable
	{
	public:

		IrrManager( irr::IrrlichtDevice *device, irr::video::IVideoDriver *driver, irr::scene::ISceneManager *smgr,
			irr::gui::IGUIEnvironment *guienv );
		~IrrManager();

		std::shared_ptr<FontHandler> font;

		irr::IrrlichtDevice *device;
		irr::video::IVideoDriver *driver;
		irr::scene::ISceneManager *smgr;
		irr::gui::IGUIEnvironment *guienv;
	};

}

#endif