#include "DXUT.h"
#include "Game.h"
#include <assimp/importer.hpp>
#include <assimp/Scene.h>
#include <assimp/PostProcess.h>
#include "Mesh/Importer.h"
#include "Scene/StaticEntity.h"
#include "UtfConverter.h"
#include "Math/math.h"
#include <windows.h>

using namespace Egg::Math;

Game::Game(ID3D11Device* device) : Egg::Control::ControlApp(device)
{
}

Game::~Game(void)
{
}

HRESULT Game::createResources()
{
	__super::createResources();

	runScript("podScene.lua");

    return S_OK;
}

HRESULT Game::releaseResources()
{
	return __super::releaseResources();
}

HRESULT Game::createSwapChainResources()
{
	return __super::createSwapChainResources();	
}

HRESULT Game::releaseSwapChainResources()
{
	return __super::releaseSwapChainResources();	
}

void Game::render(ID3D11DeviceContext* context)
{
	float clearColor[4] = 
		{ 0.9f, 0.7f, 0.1f, 0.0f };
	ID3D11RenderTargetView* defaultRtv = 	DXUTGetD3D11RenderTargetView();
	ID3D11DepthStencilView* defaultDsv = 	DXUTGetD3D11DepthStencilView();
	context->ClearRenderTargetView( defaultRtv, clearColor );
	context->ClearDepthStencilView( defaultDsv, 	D3D11_CLEAR_DEPTH, 1.0, 0 );

	__super::render(context);
}

void Game::animate(double dt, double t)
{
	__super::animate(dt, t);
}

bool Game::processMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::processMessage(hWnd, uMsg, wParam, lParam);
}


