#include "DXUT.h"
#include "EffectInclude.h"
#include "SystemEnvironment.h"
#include "UtfConverter.h"

using namespace Egg;

EffectInclude::EffectInclude(SystemEnvironment& systemEnvironment)
	:systemEnvironment(systemEnvironment)
{

}

HRESULT EffectInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	// Read the file content

	std::string filename = pFileName;

	std::string filepath = systemEnvironment.resolveEffectPath(filename, IncludeType != D3D_INCLUDE_TYPE::D3D_INCLUDE_SYSTEM);

	FILE *f = fopen(filepath.c_str(), "rb");

	if(f == nullptr)
	{
		//error message
		return E_FAIL;
	}

	// Get the file size
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Get the file content
	char *buffer = new char[sz];
	fread(buffer, 1, sz, f);
	fclose(f);

	// Save the file data into ppData and the size into pBytes.
	*ppData = buffer;

	*pBytes = sz;

	return S_OK;
}

HRESULT EffectInclude::Close(THIS_ LPCVOID pData)
{
	char* pChar = (char*)pData;
	delete [] pChar;
	return S_OK;
}

