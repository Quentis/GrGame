#pragma once
#include "App/App.h"
#include <map>
#include <vector>
#include "DXUTgui.h"
#include "d3dx11effect.h"
#include "Control/Button.h"

namespace Egg { namespace Sas {

	namespace Control
	{
		class Base;
	}

	class SasApp :
		public Egg::App
	{
	private:
		bool hide;
		float guiDt;
		int nextControlId;
	protected:
		CDXUTDialog hud; // manages the 3D UI
		CDXUTDialog ui; // dialog for sample specific controls
		CDXUTDialogResourceManager dialogResourceManager; // manager for shared resources of dialogs

		static void CALLBACK onGuiEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
		void processGuiEvent( UINT nEvent, int nControlID, CDXUTControl* pControl);

		virtual std::string getMainEffectFilename(){ return "sasTest.fx";}

		Sas::Control::Button* addButton(LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey,
			bool bIsDefault, Sas::Control::Button::Functor* functor );

		std::map<int, Sas::Control::Base*> idMap;
		std::vector<Sas::Control::Base*> sasControls;

	public:
		SasApp(ID3D11Device* device);
		virtual ~SasApp();
		virtual HRESULT createResources();
		virtual HRESULT createSwapChainResources();
		virtual HRESULT releaseResources();
		virtual HRESULT releaseSwapChainResources();

		virtual void animate(double dt, double t);
		virtual bool processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void render(ID3D11DeviceContext* context);

	};

}}