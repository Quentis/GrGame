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
	loadEffect();

	binder = Egg::Mesh::Binder::create(device);

	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile(
		App::getSystemEnvironment().resolveMediaPath("giraffe.obj"), 0);
	// if the import failed
	if (!assScene || !assScene->HasMeshes() ||
		assScene->mNumMeshes == 0)
	{
		//shadedMesh.reset();
		releaseResources();
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
