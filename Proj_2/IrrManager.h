#ifndef IRRMANAGER_H
#define IRRMANAGER_H


#include <boost/utility.hpp>
#include <irrlicht.h>


namespace Tuatara
{
	
	// container class for the big four irrlicht obj's
	class IrrManager : boost::noncopyable
	{
	public:

		IrrManager( irr::IrrlichtDevice *device, irr::video::IVideoDriver *driver, irr::scene::ISceneManager *smgr,
			irr::gui::IGUIEnvironment *guienv );
		~IrrManager();

		irr::IrrlichtDevice *device;
		irr::video::IVideoDriver *driver;
		irr::scene::ISceneManager *smgr;
		irr::gui::IGUIEnvironment *guienv;
	};

}

#endif