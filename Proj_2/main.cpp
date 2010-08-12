// SG 490
// Project 2

// Jerry Chien Chang
// Ashlea Dugan
// Ted Hess
// Tarik Sturm-Dahal
// Amos Dylan Vance

// CRT memory leak checker
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// Visual Leak Detector (http://sites.google.com/site/dmoulding/vld) There's a 2.0 version, but I had problems
// with it a few months back.
#include <vld.h>
#include <irrlicht.h>

#include "Game.h"
#include "GameExit.h"
#include "IrrManager.h"


namespace Tuatara
{
	bool InitIrrObjects( const irr::video::E_DRIVER_TYPE& Type, irr::IrrlichtDevice** device, 
		irr::video::IVideoDriver** driver, irr::scene::ISceneManager** smgr,
		irr::gui::IGUIEnvironment** guienv );
}


int main()
{
// macro to spew out leaks at program termination (if any)
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	using namespace irr;
	using namespace Tuatara;

	video::E_DRIVER_TYPE driverType;
	// set renderer according to OS (at compile time)
#ifdef _IRR_WINDOWS_
	driverType = video::EDT_DIRECT3D9;
#elif defined(_IRR_LINUX_PLATFORM_)
	driverType = video::EDT_OPENGL;
#endif

	// the four key objects for Irrlicht
	IrrlichtDevice *device = nullptr;
	video::IVideoDriver *driver = nullptr;
	scene::ISceneManager *smgr = nullptr;
	gui::IGUIEnvironment *guienv = nullptr;

	if( !InitIrrObjects( driverType, &device, &driver, &smgr, &guienv ) )
	{
		return 1;
	}

	// note: no need to drop device at program end, it will be dropped automatically through Game
	Game game( new IrrManager( device, driver, smgr, guienv ) );

	// continue running FSM until exit state is reached
	while( !game.stateMachine->isInState( (*GameExit::Instance()) ) )
	{
		game.Update();
	}

	return 0;
}

namespace Tuatara
{

	bool InitIrrObjects( const irr::video::E_DRIVER_TYPE& Type, irr::IrrlichtDevice** device,
		irr::video::IVideoDriver** driver, irr::scene::ISceneManager** smgr,
		irr::gui::IGUIEnvironment** guienv )
	{
		using namespace irr;

		// init device with null to acquire monitor resolution first
		*device = createDevice( video::EDT_NULL );
		core::dimension2d<u32> resolution = (*device)->getVideoModeList()->getDesktopResolution();
		u32 screenBitDepth = (*device)->getVideoModeList()->getDesktopDepth();

		// snip 200 pixels off so it doesn't take up entire screen
		resolution.Height -= 200;
		resolution.Width -= 200;
		
		// create the device with the calculated resolution and bit depth
		*device = createDevice( Type, resolution, screenBitDepth, false, true );

		if( (*device) == nullptr )
		{
			return false;
		}

		*driver = (*device)->getVideoDriver();
		*smgr = (*device)->getSceneManager();
		*guienv = (*device)->getGUIEnvironment();

		return true;
	}

}