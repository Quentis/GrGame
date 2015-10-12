//--------------------------------------------------------------------------------------
// File: gg001-Triangle.cpp
//
// Empty starting point for new Direct3D 9 and/or Direct3D 11 applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"

// global variables
ID3D11Buffer* vertexBuffer;
ID3D11InputLayout* inputLayout;
ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(D3DXVECTOR3) * 3;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(D3DXVECTOR3);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	D3DXVECTOR3 vertexPositionArray[3] = {
		D3DXVECTOR3(0, 0, 0.5),
		D3DXVECTOR3(0, 1, 0.5),
		D3DXVECTOR3(1, 0, 0.5) };
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertexPositionArray;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	pd3dDevice->CreateBuffer(&desc, &initData, &vertexBuffer);

	const char* vertexShaderCode = 
		"float4 vsIdle(float4 pos :POSITION ) : SV_Position{ return pos; }";
	ID3DBlob* vertexShaderByteCode;
	D3DX11CompileFromMemory(vertexShaderCode, strlen(vertexShaderCode), NULL, NULL, NULL,
		"vsIdle", "vs_5_0", 0, 0, NULL, &vertexShaderByteCode, NULL, NULL);
	pd3dDevice->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		NULL, &vertexShader);

	D3D11_INPUT_ELEMENT_DESC positionElement;
	positionElement.AlignedByteOffset = 0;
	positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	positionElement.InputSlot = 0;
	positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	positionElement.InstanceDataStepRate = 0;
	positionElement.SemanticIndex = 0;
	positionElement.SemanticName = "POSITION";

	pd3dDevice->CreateInputLayout(
		&positionElement, 1, vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(), &inputLayout);

	const char* pixelShaderCode = "float4 psIdle() : SV_Target{ return float4(1, 0, 0, 1); }";
	ID3DBlob* pixelShaderByteCode;
	D3DX11CompileFromMemory(pixelShaderCode, strlen(pixelShaderCode), NULL, NULL,
		NULL, "psIdle", "ps_5_0", 0, 0, NULL, &pixelShaderByteCode, NULL, NULL);
	pd3dDevice->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		NULL, &pixelShader);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
    // Clear render target and the depth stencil 
	ID3D11DeviceContext* context = pd3dImmediateContext;
	float clearColor[4] = { 0.9f, 0.7f, 0.1f, 0.0f };
	ID3D11RenderTargetView* defaultRtv = DXUTGetD3D11RenderTargetView();
	ID3D11DepthStencilView* defaultDsv = DXUTGetD3D11DepthStencilView();
	context->ClearRenderTargetView(defaultRtv, clearColor);
	context->ClearDepthStencilView(defaultDsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	unsigned int stride = sizeof(D3DXVECTOR3);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout);
	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);
	context->Draw(3, 0);
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	vertexBuffer->Release();
	inputLayout->Release();
	vertexShader->Release();
	pixelShader->Release();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D11) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    // Perform any application-level initialization here

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"gg001-Triangle" );

    // Only require 10-level hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 640, 480 );
    DXUTMainLoop(); // Enter into the DXUT ren  der loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}


