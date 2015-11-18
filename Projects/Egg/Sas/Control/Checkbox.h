#pragma once
#include "Control.h"

namespace Egg { namespace Sas { namespace Control
{

	class Checkbox :
		public Sas::Control::Base
	{
		CDXUTCheckBox* checkbox;
	public:
		Checkbox(ID3DX11EffectVariable* variable, CDXUTDialog& ui, int& nextControlPos, int& nextControlId, std::map<int, Sas::Control::Base*>& idMap);
		~Checkbox(void);

		void apply();

	};

}}}