#include "IrrManager.h"


namespace Tuatara
{

	IrrManager::IrrManager( irr::IrrlichtDevice *device, irr::video::IVideoDriver *driver,
		irr::scene::ISceneManager *smgr, irr::gui::IGUIEnvironment *guienv )
		: device( device ), driver( driver ), smgr( smgr ), guienv( guienv )
	{
	}

	IrrManager::~IrrManager()
	{
		// drop irrlicht device for final cleanup
		if( device != nullptr )
		{
			device->drop();
		}
	}

}