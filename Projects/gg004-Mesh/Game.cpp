#include "DXUT.h"
#include "Game.h"
#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>
#include "Mesh/Importer.h"
#include "UtfConverter.h"


Game::Game(ID3D11Device* device) : App(device)
{
}


Game::~Game()
{
}

HRESULT Game::createResources()
{
	//D3D11_BUFFER_DESC desc;
	//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//desc.ByteWidth = sizeof(D3DXVECTOR3) * 3;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;
	//desc.StructureByteStride = sizeof(D3DXVECTOR3);
	//desc.Usage = D3D11_USAGE_IMMUTABLE;
	//D3DXVECTOR3 vertexPositionArray[3] = {
	//	D3DXVECTOR3(0, 0, 0.5),
	//	D3DXVECTOR3(0, 1, 0.5),
	//	D3DXVECTOR3(1, 0, 0.5) };
	//D3D11_SUBRESOURCE_DATA initData;
	//initData.pSysMem = vertexPositionArray;
	//initData.SysMemPitch = 0;
	//initData.SysMemSlicePitch = 0;
	//device->CreateBuffer(&desc, &initData, &vertexBuffer);

	//D3D11_INPUT_ELEMENT_DESC positionElement;
	//positionElement.AlignedByteOffset = 0;
	//positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//positionElement.InputSlot = 0;
	//positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//positionElement.InstanceDataStepRate = 0;
	//positionElement.SemanticIndex = 0;
	//positionElement.SemanticName = "POSITION";

	loadEffect();

	binder = Egg::Mesh::Binder::create(device);

	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile(
		App::getSystemEnvironment().resolveMediaPath("giraffe.obj"), 0);
	// if the import failed
	if (!assScene || !assScene->HasMeshes() ||
		assScene->mNumMeshes == 0)
	{
		shadedMesh.reset();
		return E_FAIL;
	}

	Egg::Mesh::Indexed::P indexedMesh =
		Egg::Mesh::Importer::fromAiMesh(device, assScene->mMeshes[0]);
	
	ID3DX11EffectPass* idlePass =
		effect->GetTechniqueByName("idle")->GetPassByName("idle");
	
	Egg::Mesh::Material::P  idleMaterial =
		Egg::Mesh::Material::create(idlePass, 0);
	
	shadedMesh = binder->bindMaterial(idleMaterial, indexedMesh);
	
	return S_OK;

	//D3DX11_PASS_DESC idlePassDesc;
	//effect->GetTechniqueByName("idle")->GetPassByName("idle")->GetDesc(&idlePassDesc);
	//device->CreateInputLayout(
	//	&positionElement, 1,
	//	idlePassDesc.pIAInputSignature,
	//	idlePassDesc.IAInputSignatureSize,
	//	&inputLayout);

	//return S_OK;
}

HRESULT Game::releaseResources()
{
	binder.reset();
	shadedMesh.reset();
	
	return Egg::App::releaseResources();
}

void Game::render(ID3D11DeviceContext* context)
{
	// Clear render target and the depth stencil
	float clearColor[4] = { 0.9f, 0.7f, 0.1f, 0.0f };
	ID3D11RenderTargetView* defaultRtv = DXUTGetD3D11RenderTargetView();
	ID3D11DepthStencilView* defaultDsv = DXUTGetD3D11DepthStencilView();
	context->ClearRenderTargetView(defaultRtv, clearColor);
	context->ClearDepthStencilView(defaultDsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	shadedMesh->draw(context);
}
