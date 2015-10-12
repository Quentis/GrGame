#include "DXUT.h"
#include "Game.h"


Game::Game(ID3D11Device* device) : App(device)
{
}


Game::~Game()
{
}

HRESULT Game::createResources()
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
	device->CreateBuffer(&desc, &initData, &vertexBuffer);

	D3D11_INPUT_ELEMENT_DESC positionElement;
	positionElement.AlignedByteOffset = 0;
	positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	positionElement.InputSlot = 0;
	positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	positionElement.InstanceDataStepRate = 0;
	positionElement.SemanticIndex = 0;
	positionElement.SemanticName = "POSITION";

	loadEffect();

	D3DX11_PASS_DESC idlePassDesc;
	effect->GetTechniqueByName("idle")->GetPassByName("idle")->GetDesc(&idlePassDesc);
	device->CreateInputLayout(
		&positionElement, 1,
		idlePassDesc.pIAInputSignature,
		idlePassDesc.IAInputSignatureSize,
		&inputLayout);

	return S_OK;
}

HRESULT Game::releaseResources()
{
	vertexBuffer->Release();
	inputLayout->Release();
	Egg::App::releaseResources();

	return S_OK;
}

void Game::render(ID3D11DeviceContext* context)
{
	// Clear render target and the depth stencil
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
	effect->GetTechniqueByName("idle")->GetPassByName("idle")->Apply(0, context);
	context->Draw(3, 0);
}
