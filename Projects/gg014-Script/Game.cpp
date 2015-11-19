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

Game::Game(ID3D11Device* device) : Egg::Scene::ManagerApp(device)
{
}

Game::~Game(void)
{
}

HRESULT Game::createResources()
{
	__super::createResources();

	Egg::Cam::FirstPerson::P cam = Egg::Cam::FirstPerson::create();
	cameras["0"] = cam;
	currentCamera = cameras.begin();


	Egg::Mesh::Indexed::P indexedMesh = loadIndexedMesh("giraffe.obj");
	ID3DX11EffectPass* envmappedPass = effect
		->GetTechniqueByName("envmapped")
		->GetPassByName("envmapped");
	Egg::Mesh::Material::P  envmappedGiraffeMaterial = Egg::Mesh::Material::create(envmappedPass, 0);

	ID3DX11EffectPass* texturedPass = effect
		->GetTechniqueByName("textured")
		->GetPassByName("textured");
	Egg::Mesh::Material::P  texturedGiraffeMaterial = Egg::Mesh::Material::create(texturedPass, 0);


	envmappedGiraffeMaterial->set(
		effect
		->GetVariableByName("kdTexture"))
		->AsShaderResource()
		->SetResource(
		loadSrv("giraffe.jpg") );
	envmappedGiraffeMaterial->set(
		effect
		->GetVariableByName("envTexture"))
		->AsShaderResource()
		->SetResource(
		loadSrv("cloudyNoon.dds") );

	Egg::Mesh::Multi::P geopodmmesh = loadMultiMesh("geopod.x", 0, "pod");

	Egg::Scene::StaticEntity::P e3 = 
		Egg::Scene::StaticEntity::create(geopodmmesh);
	entities["pod0"] = e3;

	Egg::Mesh::Shaded::P shadedMesh = binder->bindMaterial(
		texturedGiraffeMaterial,
		indexedMeshes["pod[0]"]);
	Egg::Mesh::Shaded::P shadedMesh1 = binder->bindMaterial(
		envmappedGiraffeMaterial,
		indexedMeshes["pod[1]"]);
	Egg::Mesh::Mien basicMien = getMien("basic");

	Egg::Mesh::Flip::P giraffeFlipMesh = Egg::Mesh::Flip::create();
	giraffeFlipMesh->add(
		basicMien,
		shadedMesh);

	Egg::Mesh::Flip::P giraffeFlipMesh1 = Egg::Mesh::Flip::create();
	giraffeFlipMesh1->add(
		basicMien,
		shadedMesh1);

	Egg::Mesh::Multi::P giraffeMultiMesh = Egg::Mesh::Multi::create();
	giraffeMultiMesh->add(
		giraffeFlipMesh);
	giraffeMultiMesh->add(
		giraffeFlipMesh1);

	Egg::Scene::StaticEntity::P e0 = 
		Egg::Scene::StaticEntity::create(giraffeMultiMesh);

	entities["giraf0"] = e0;

	using namespace Egg::Math;

	Egg::Scene::StaticEntity::P e1 = 
		Egg::Scene::StaticEntity::create(giraffeMultiMesh);
	e1->translate( float3(10, 0, 0.3));
	entities["giraf1"] = e1;


	Egg::Mesh::Indexed::P indexedQuad = Egg::Mesh::Indexed::createQuad(device);
	ID3DX11EffectPass* backgroundPass = effect
		->GetTechniqueByName("envmapped")
		->GetPassByName("background");
	Egg::Mesh::Material::P  backgroundMaterial = Egg::Mesh::Material::create(backgroundPass, 0);

	Egg::Mesh::Shaded::P backgroundShadedMesh = binder->bindMaterial(
		backgroundMaterial,
		indexedQuad);

	Egg::Mesh::Flip::P backgroundFlipMesh = Egg::Mesh::Flip::create();
	backgroundFlipMesh->add(
		basicMien,
		backgroundShadedMesh);

	Egg::Mesh::Multi::P backgroundMultiMesh = Egg::Mesh::Multi::create();
	backgroundMultiMesh->add(
		backgroundFlipMesh);

	Egg::Scene::StaticEntity::P backgroundEntity = 
		Egg::Scene::StaticEntity::create(backgroundMultiMesh);
	backgroundEntity->translate( float3(10, 0, 0.3));
	entities["background"] = backgroundEntity;


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

void	Game::render(ID3D11DeviceContext* context)
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


