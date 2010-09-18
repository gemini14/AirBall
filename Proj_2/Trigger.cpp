#include "Trigger.h"
#include "EnumsConstants.h"
#include <stdlib.h>

#define	FINAL_X_SIZE	696
#define FINAL_Y_SIZE	466
#define CENTER_X		512
#define CENTER_Y		400
#define	NUM_STEPS		100


namespace Tuatara
{
	Trigger::Trigger(  irr::scene::ISceneManager *smgr ) : triggered( false ), promptShown( false )
	{
		irr::video::ITexture* tex;

		this->smgr = smgr;
		GUIEnv = smgr->getGUIEnvironment();

		tex = smgr->getVideoDriver()->getTexture("TriggerPopup.png");
		
		img = GUIEnv->addImage(irr::core::rect<irr::s32>(-2, -2, 1, 1) );
		img->setUseAlphaChannel(true);
		img->setImage(tex);
		img->setScaleImage( true );
		smgr->getVideoDriver()->removeTexture(tex);
	}

	Trigger::~Trigger(void)
	{
	}

	void Trigger::Show()
	{
		triggered = true;
		currentState = opening;
		currentStep = 0;

		img->setRelativePosition( irr::core::rect<irr::s32>(levelSize / 2, levelSize / 2, 1, 1) );
	}

	void Trigger::Close()
	{
		currentStep = NUM_STEPS;
		currentState = closing;
	}

	void Trigger::HandleMouseClick( irr::s32 x, irr::s32 y )
	{
		if ( currentState == displaying) currentState = closing;
	}

	bool Trigger::Update()
	{
		static float stepYSize = ( FINAL_Y_SIZE / NUM_STEPS );
		static float stepXSize = ( FINAL_X_SIZE / NUM_STEPS );

		switch (currentState)
		{
		case (opening):
			if (currentStep < NUM_STEPS)
			{
				float currentWidth = FINAL_X_SIZE * ((float)currentStep / NUM_STEPS);
				float currentHeight = FINAL_Y_SIZE * ((float)currentStep / NUM_STEPS);
				int left = CENTER_X - ((int)currentWidth / 2);
				int right = left + (int)currentWidth;
				int top = CENTER_Y - ((int)currentHeight / 2);
				int bottom = top + (int)currentHeight;
				img->setRelativePosition( irr::core::rect<irr::s32>( left, top, right, bottom ) );
				currentStep++;
			}
			else
			{
				currentStep = 0;
				currentState = displaying;
			}
			break;

		case (displaying):
			if (!promptShown)
			{
			int size = mbstowcs( nullptr, prompt.c_str(), 0 ); 
			if( size != -1 ) 
			{ 
				wchar_t *convertedText = new wchar_t[ size + 1 ]; 
				mbstowcs( convertedText, prompt.c_str(), size + 1 ); 
				irr::core::rect<irr::s32> imgLoc = img->getAbsolutePosition();
				irr::core::rect<irr::s32> loc(
					imgLoc.UpperLeftCorner.X + (148 + 20),
					imgLoc.UpperLeftCorner.Y + (123 + 20),
					imgLoc.LowerRightCorner.X - (111 + 20),
					imgLoc.LowerRightCorner.Y - (108 + 20) );

				text = GUIEnv->addStaticText( convertedText, loc ); 
				text->setWordWrap(true);
				delete[] convertedText; 
			} 
				promptShown = true;
			}
			break;

		case (closing):
			if (promptShown)
			{
				text->remove();
				promptShown = false;
			}
			if (currentStep > 0)
			{
				float currentWidth = FINAL_X_SIZE * ((float)currentStep / NUM_STEPS);
				float currentHeight = FINAL_Y_SIZE * ((float)currentStep / NUM_STEPS);
				int left = CENTER_X - ((int)currentWidth / 2);
				int right = left + (int)currentWidth;
				int top = CENTER_Y - ((int)currentHeight / 2);
				int bottom = top + (int)currentHeight;
				img->setRelativePosition( irr::core::rect<irr::s32>( left, top, right, bottom ) );
				currentStep--;
			}
			else
			{
				img->setRelativePosition(irr::core::rect<irr::s32>(-2, -2, 1, 1) );
				currentStep = 0;
				currentState = unknown;
				return false;
			}
			break;

		case (unknown):
			break;

		}

		return true;
	}
}
