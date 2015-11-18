#include "DXUT.h"	
#include "SasApp.h"
#include "Control/Button.h"
#include "Control/Slider.h"
#include "Control/Checkbox.h"
#include <string>
#include <sstream>
#include "SDKmisc.h"

using namespace Egg;

Sas::SasApp::SasApp(ID3D11Device* device)
: Egg::App(device)
{
	guiDt = 0.0001;
	hide = false;
}

Sas::SasApp::~SasApp()
{
}

class FullScreenFunctor : public Sas::Control::Button::Functor
{
public:
	void operator()()
	{
		DXUTToggleFullScreen();
	}
};

HRESULT Sas::SasApp::createResources()
{
	loadEffect();

	hud.Init( &dialogResourceManager );
    ui.Init( &dialogResourceManager );
    hud.SetCallback( onGuiEvent, this);
    ui.SetCallback( onGuiEvent, this );

	int nextPosition = 10;
	nextControlId = 0;

	CDXUTButton* button;
	hud.AddButton( nextControlId++, L"Toggle full screen", 5, 10, 125, 22, 0, false, &button );
	Sas::Control::Button* sasButton = new Sas::Control::Button(button, new FullScreenFunctor());
	idMap[button->GetID()] = sasButton;
	sasControls.push_back(sasButton);

	int nVariable = 0;
	ID3DX11EffectVariable* variable;
	while( (variable = effect->GetVariableByIndex(nVariable))->IsValid() )
	{
		ID3DX11EffectStringVariable* controlTypeString = variable->GetAnnotationByName("SasUiControl")->AsString();
		const char* controlTypeName = NULL;
		controlTypeString->GetString(&controlTypeName);
		if(controlTypeName)
		{
			if(strcmp(controlTypeName, "Slider") == 0)
			{
				Sas::Control::Slider* sasSlider = new Sas::Control::Slider(variable, ui, nextPosition, nextControlId, idMap);
				sasControls.push_back(sasSlider);
			}
			if(strcmp(controlTypeName, "CheckBox") == 0)
			{
				Sas::Control::Checkbox* sasCheckbox = new Sas::Control::Checkbox(variable, ui, nextPosition, nextControlId, idMap);
				sasControls.push_back(sasCheckbox);
			}
		}
		nVariable++;
	}

	ID3D11DeviceContext* immediateDeviceContext = DXUTGetD3D11DeviceContext();
	dialogResourceManager.OnD3D11CreateDevice( device, immediateDeviceContext );

	return S_OK;
}

HRESULT Sas::SasApp::createSwapChainResources()
{
	dialogResourceManager.OnD3D11ResizedSwapChain(device, &backbufferSurfaceDesc);

	hud.SetLocation( backbufferSurfaceDesc.Width-170, 0 );
    hud.SetSize( 170, 170 );
    ui.SetLocation( 0, 0 );
    ui.SetSize( 170, 500 );
	return S_OK;
}

HRESULT Sas::SasApp::releaseResources()
{
	dialogResourceManager.OnD3D11DestroyDevice();

	DXUTGetGlobalResourceCache().OnDestroyDevice();
	std::vector<Sas::Control::Base*>::iterator i = sasControls.begin();
	std::vector<Sas::Control::Base*>::iterator e = sasControls.end();
	while(i != e)
	{
		delete *i;
		i++;
	}

	return Egg::App::releaseResources();
}

HRESULT Sas::SasApp::releaseSwapChainResources()
{
	dialogResourceManager.OnD3D11ReleasingSwapChain();

	return S_OK;
}

void Sas::SasApp::animate(double dt, double t)
{
	guiDt = dt;
}

bool Sas::SasApp::processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN && wParam == VK_SPACE)
		hide = !hide;

	if(hide)
		return false;

	bool noFurther;
    noFurther = dialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( noFurther )
        return true;

	// Give the dialogs a chance to handle the message first
    noFurther = hud.MsgProc( hWnd, uMsg, wParam, lParam );
    if( noFurther )
        return true;
    noFurther = ui.MsgProc( hWnd, uMsg, wParam, lParam );
    if( noFurther )
        return true;

	return false;
}

void Sas::SasApp::render(ID3D11DeviceContext* context)
{
	if(hide)
		return;
	#pragma region clear
	if(typeid(*this) == typeid(Egg::Sas::SasApp))
	{
		float clearColor[4] = { 0.0f, 0.0f, 0.5f, 0.0f };

		ID3D11RenderTargetView* defaultRtv = DXUTGetD3D11RenderTargetView();
		ID3D11DepthStencilView* defaultDsv = DXUTGetD3D11DepthStencilView();
		context->ClearRenderTargetView( defaultRtv, clearColor );
		context->ClearDepthStencilView( defaultDsv, D3D11_CLEAR_DEPTH, 1.0, 0 );
	}
	#pragma endregion a little cheat to clear the screen when testing Sas::SasApp without deriving from it

	if(swapChain == NULL)
		return;
	hud.OnRender( guiDt ); 
    ui.OnRender( guiDt );
}

void CALLBACK Sas::SasApp::onGuiEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	((Sas::SasApp*)pUserContext)->processGuiEvent(nEvent, nControlID, pControl);
}

void Sas::SasApp::processGuiEvent( UINT nEvent, int nControlID, CDXUTControl* pControl)
{
	std::map<int, Sas::Control::Base*>::iterator iControl = idMap.find(nControlID);
	if(iControl != idMap.end())
		iControl->second->apply();
}


Sas::Control::Button* Sas::SasApp::addButton(LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey,
			bool bIsDefault, Sas::Control::Button::Functor* functor )
{
	CDXUTButton* button;
	hud.AddButton( nextControlId++, strText, x, y, width, height, nHotkey, false, &button );
	Sas::Control::Button* sasButton = new Sas::Control::Button(button, functor);
	idMap[button->GetID()] = sasButton;
	sasControls.push_back(sasButton);
	return sasButton;
}