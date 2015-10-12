#include "DXUT.h"
#include "App.h"
#include "EffectInclude.h"

using namespace Egg;

void App::loadEffect()
{
	ID3DBlob* compilationErrors = NULL;
	ID3DBlob* compiledEffect = NULL;

	EffectInclude effectInclude(systemEnvironment);

	if(FAILED(
		D3DX11CompileFromFileA(
			systemEnvironment.resolveEffectPath( getMainEffectFilename(), true).c_str(), NULL, &effectInclude, NULL,
			"fx_5_0", 0, 0, NULL, &compiledEffect, &compilationErrors, NULL))) {
				if(compilationErrors)
					MessageBoxA( NULL, 
					(LPSTR)compilationErrors->GetBufferPointer(),
					"Failed to load effect file!", MB_OK);
				else
					MessageBoxA( NULL, 
					"File cound not be opened",
					"Failed to load effect file!", MB_OK);
				exit(-1);
		}

	if(FAILED(
	D3DX11CreateEffectFromMemory(
		compiledEffect->GetBufferPointer(), 
		compiledEffect->GetBufferSize(),
		0,
		device,
		&effect)  )) {
		MessageBoxA( NULL, 
			"D3DX11CreateEffectFromMemory failed.",
			"Failed to load effect file!", MB_OK);
		exit(-1);
		}
}