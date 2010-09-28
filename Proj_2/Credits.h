#ifndef CREDITS_H
#define CREDITS_H

#include "ScreenBase.h"


namespace Tuatara
{

	class Credits : public ScreenBase
	{
	private:

		Credits();
		~Credits();

	public:

		static Credits* Instance();

		virtual void Enter( Game *game );
		virtual void Execute( Game *game );
		virtual void Exit( Game *game );

		// will be called when a button is clicked
		virtual bool OnClick( int ID );

		// will be called when a key is pressed:
		virtual bool OnKeyEvent( irr::EKEY_CODE code );
	};

}

#endif