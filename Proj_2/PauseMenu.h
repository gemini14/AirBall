#pragma once

#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "EnumsConstants.h"
#include "ScreenBase.h"
#include "GUIButton.h"

namespace Tuatara
{
	class PauseMenu : public ScreenBase
	{
	private:
		PauseMenu(void);
		~PauseMenu(void);

		GUIButton* returnButton;
		GUIButton* mainMenuButton;

	public:
		
		static PauseMenu* Instance();

		// will be called when a button is clicked
		virtual bool OnClick( int ID );
	};
}