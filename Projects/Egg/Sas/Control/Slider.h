#pragma once
#include "Control.h"

namespace Egg { namespace Sas { namespace Control
{
	class Slider :
		public Sas::Control::Base
	{
		CDXUTSlider* slider;
		CDXUTStatic* stat;
	public:
		Slider(ID3DX11EffectVariable* variable, CDXUTDialog& ui, int& nextControlPos, int& nextControlId, std::map<int, Sas::Control::Base*>& idMap);
		~Slider(void);

		void apply();
	};

}}}