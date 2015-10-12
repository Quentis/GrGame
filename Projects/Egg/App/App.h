#pragma once

#include "d3dx11effect.h"
#include "SystemEnvironment.h"

namespace Egg {
	class App
	{
	protected:
		ID3D11Device* device;
		IDXGISwapChain* swapChain;
		DXGI_SURFACE_DESC backbufferSurfaceDesc;
		ID3DX11Effect* effect;

		SystemEnvironment systemEnvironment;

		App(ID3D11Device* device)
			{this->device = device; this->effect = NULL; swapChain = NULL;}
	public:
		void setSwapChain(IDXGISwapChain* swapChain,
		 const DXGI_SURFACE_DESC* backbufferSurfaceDesc) {
			 this->swapChain = swapChain;
			 this->backbufferSurfaceDesc =
				 *backbufferSurfaceDesc;}
		virtual ~App(){}

		virtual std::string getMainEffectFilename(){ return "basic.fx";}

		virtual void loadEffect();
		SystemEnvironment& getSystemEnvironment(){return systemEnvironment;}

		ID3DX11Effect* getEffect(){return effect;}

		virtual HRESULT createResources()
			{return S_OK;}
		virtual HRESULT createSwapChainResources()
			{return S_OK;}
		virtual HRESULT releaseResources() { if(effect) effect->Release(); return S_OK; }
		virtual HRESULT releaseSwapChainResources()
			{return S_OK;}
		virtual void animate(double dt, double t){}
		virtual bool processMessage( HWND hWnd, 
			UINT uMsg, WPARAM wParam, LPARAM lParam)
			{return false;}
		virtual void render(
			ID3D11DeviceContext* context){}
	};
} // namespace Egg