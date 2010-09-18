#pragma once
#include <irrlicht.h>
#include <string>

namespace Tuatara
{
	enum TriggerType
	{
		triggerLocation,
		triggerNamed,
	};

	// simple trigger mechanism for tutorials
	class Trigger
	{
	public:
		Trigger(  irr::scene::ISceneManager *smgr );
		~Trigger(void);

		TriggerType type;
		irr::core::vector3di blockLoc;	// some triggers are based on ball location
		std::string name;				// some triggers are on specific named events
		std::string prompt;				// the text to display
		bool triggered;					// has it already been triggered?

		void HandleMouseClick( irr::s32 x, irr::s32 y );
		void Show();
		void Close();
		bool Update();

	private:
		enum triggerState
		{
			unknown,
			opening,
			displaying,
			closing,
		};

		irr::gui::IGUIEnvironment* GUIEnv;
		irr::scene::ISceneManager* smgr;

		irr::gui::IGUIImage* img;
		irr::gui::IGUIStaticText* text;

		triggerState currentState;
		int currentStep;
		bool promptShown;

	};
}

