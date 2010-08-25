#pragma once


#include <boost/noncopyable.hpp>
#include <irrlicht.h>
#include <memory>

#include "EnumsConstants.h"
#include "ScreenBase.h"
#include "GUIButton.h"

namespace Tuatara
{
	class MainMenu : public ScreenBase
	{

	private:

		MainMenu();
		~MainMenu();

		GUIButton *newGameButton;
		GUIButton *exitButton;

	public:

		static MainMenu* Instance();

		// will be called when a button is clicked
		virtual bool OnClick( int ID );
	};
}