#include "DXUT.h"
#include "Checkbox.h"
#include <sstream>

using namespace Egg;

Sas::Control::Checkbox::Checkbox(ID3DX11EffectVariable* variable, CDXUTDialog& ui, int& nextControlPos, int& nextControlId, std::map<int, Sas::Control::Base*>& idMap)
	:Base(variable)
{
	std::wstringstream wss;
	float value;
	variable->AsScalar()->GetFloat(&value);
	wss << label;
	idMap[nextControlId] = this;
	ui.AddCheckBox( nextControlId++, wss.str().c_str(), 5, nextControlPos, 100, 22, value, 0, false, &checkbox);
	nextControlPos += 24;
}

Sas::Control::Checkbox::~Checkbox(void)
{
}

void Sas::Control::Checkbox::apply()
{

	variable->AsScalar()->SetBool( checkbox->GetChecked() );

}
