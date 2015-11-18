#pragma once
#include "Control.h"

namespace Egg { namespace Sas { namespace Control
{

	class Button :
		public Sas::Control::Base
	{
	public:
		class Functor
		{
		public:
			virtual void operator()()=0;
		};
	private:
		CDXUTButton* button;
		Functor* functor;

	public:
		Button(CDXUTButton* button, Functor* functor);
		~Button(void);

		void setText(LPCWSTR text);

		void apply();
	};

}}}