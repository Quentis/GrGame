#include "DXUT.h"
#include "Game.h"
#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>
#include "Mesh/Importer.h"
#include "UtfConverter.h"
#include "Math/math.h"

using namespace Egg::Math;

Game::Game(ID3D11Device* device) : App(device)
{
}


Game::~Game()
{
}

HRESULT Game::createResources()
{
	loadEffect();

	D3DX11CreateShaderResourceViewFromFileA(
		device,	systemEnvironment.resolveMediaPath("giraffe.jpg").c_str(),
		NULL, NULL, &kdSrv, NULL);

	D3DX11CreateShaderResourceViewFromFileA(
		device, systemEnvironment.resolveMediaPath("cloudyNoon.dds").c_str(),
		NULL, NULL, &envSrv, NULL);

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

	Egg::Mesh::Indexed::P indexedMesh =	Egg::Mesh::Importer::fromAiMesh(device, assScene->mMeshes[0]);
	ID3DX11EffectPass* basicPass = effect->GetTechniqueByName("envmapped")->GetPassByName("envmapped");
	Egg::Mesh::Material::P envmappedMaterial = Egg::Mesh::Material::create(basicPass, 0);

	Egg::Mesh::Indexed::P indexedQuad =	Egg::Mesh::Indexed::createQuad(device);
	ID3DX11EffectPass* backgroundPass =	effect->GetTechniqueByName("envmapped")->GetPassByName("background");
	Egg::Mesh::Material::P backgroundMaterial = Egg::Mesh::Material::create(backgroundPass, 0);
	backgroundQuad = binder->bindMaterial(backgroundMaterial, indexedQuad);

	envmappedMaterial->set(effect->GetVariableByName("kdTexture"))->AsShaderResource()->SetResource(kdSrv);

	effect->GetVariableByName("envTexture")->AsShaderResource()->SetResource(envSrv);

	shadedMesh = binder->bindMaterial(envmappedMaterial, indexedMesh);
	
	firstPersonCam = Egg::Cam::FirstPerson::create();

	return S_OK;
}

HRESULT Game::releaseResources()
{
	kdSrv->Release();
	envSrv->Release();
	binder.reset();
	shadedMesh.reset();
	backgroundQuad.reset();
	firstPersonCam.reset();

	return Egg::App::releaseResources();
}

void Game::render(ID3D11DeviceContext* context)
{
	// Clear render target and the depth stencil
	effect->GetVariableByName("eyePos")->AsVector()
		->SetFloatVector((float*) &firstPersonCam->getEyePosition());

	effect->GetVariableByName("viewDirMatrix")->AsMatrix()
		->SetMatrix((float*)&firstPersonCam->getViewDirMatrix());

	float clearColor[4] = { 0.9f, 0.7f, 0.1f, 0.0f };
	ID3D11RenderTargetView* defaultRtv = DXUTGetD3D11RenderTargetView();
	ID3D11DepthStencilView* defaultDsv = DXUTGetD3D11DepthStencilView();
	context->ClearRenderTargetView(defaultRtv, clearColor);
	context->ClearDepthStencilView(defaultDsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	shadedMesh->draw(context);
	backgroundQuad->draw(context);
}

void Game::animate(double dt, double t)
{
	if (!firstPersonCam) return;
	firstPersonCam->animate(dt);
	setTransformation();
}

bool Game::processMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!firstPersonCam) return false;
	firstPersonCam->processMessage(hWnd, uMsg, wParam, lParam);
	return false;
}

HRESULT Game::createSwapChainResources()
{
	firstPersonCam->setAspect((float)backbufferSurfaceDesc.Width / backbufferSurfaceDesc.Height);
	return S_OK;
}

HRESULT Game::releaseSwapChainResources()
{
	return S_OK;
}

void Game::setTransformation()
{
	effect->GetVariableByName("modelMatrix")->AsMatrix()->SetMatrix((float*)&float4x4::identity);
	effect->GetVariableByName("modelMatrixInverse")->AsMatrix()->SetMatrix(	(float*)&float4x4::identity);
	float4x4 viewProjMatrix = firstPersonCam->getViewMatrix() * firstPersonCam->getProjMatrix();
	effect->GetVariableByName("modelViewProjMatrix")->AsMatrix()->SetMatrix((float*)&viewProjMatrix);
}

