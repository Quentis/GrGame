#include "DXUT.h"
#include "Button.h"

using namespace Egg;

Sas::Control::Button::Button(CDXUTButton* button, Functor* functor)
	:Base(NULL)
{
	this->button = button;
	this->functor = functor;
}

Sas::Control::Button::~Button(void)
{
	delete functor;
}

void Sas::Control::Button::apply()
{
	if(variable == NULL)
		(*functor)();
}

void Sas::Control::Button::setText(LPCWSTR text)
{
	button->SetText(text);
}