#ifndef ENUMSCONSTANTS_H
#define ENUMSCONSTANTS_H

namespace Tuatara
{
	// anonymous enum for GUI object IDs
	enum
	{
		MAINMENU_BUTTON_NEWGAME,
		MAINMENU_BUTTON_EXIT,
		PAUSEMENU_BUTTON_RETURN,
		PAUSEMENU_BUTTON_MAINMENU,
		MAINMENU_TITLE,
	};

	enum Direction 
	{ 
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARD,
	};
}

#endif