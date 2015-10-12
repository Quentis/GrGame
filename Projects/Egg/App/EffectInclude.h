#pragma once
#include "d3dcommon.h"

namespace Egg {

	class SystemEnvironment;

	class EffectInclude :
		public ID3DInclude
	{
		SystemEnvironment& systemEnvironment;
	public:
		STDMETHOD(Open)(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
		STDMETHOD(Close)(THIS_ LPCVOID pData);

		EffectInclude(SystemEnvironment& systemEnvironment);
	};

} // namespace Egg