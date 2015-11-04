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

	binder = Egg::Mesh::Binder::create(device);

	/*Particle-START*/
	for (int i = 0; i<40; i++)
		particles.push_back(Particle());
	
	D3D11_INPUT_ELEMENT_DESC particleElements[3];
	particleElements[0].AlignedByteOffset = offsetof(Particle, position);
	particleElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	particleElements[0].InputSlot = 0;
	particleElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	particleElements[0].InstanceDataStepRate = 0;
	particleElements[0].SemanticIndex = 0;
	particleElements[0].SemanticName = "POSITION";
	particleElements[1].AlignedByteOffset = offsetof(Particle, lifespan);
	particleElements[1].Format = DXGI_FORMAT_R32_FLOAT;
	particleElements[1].InputSlot = 0;
	particleElements[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	particleElements[1].InstanceDataStepRate = 0;
	particleElements[1].SemanticIndex = 0;
	particleElements[1].SemanticName = "LIFESPAN";
	particleElements[2].AlignedByteOffset = offsetof(Particle, age);
	particleElements[2].Format = DXGI_FORMAT_R32_FLOAT;
	particleElements[2].InputSlot = 0;
	particleElements[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	particleElements[2].InstanceDataStepRate = 0;
	particleElements[2].SemanticIndex = 0;
	particleElements[2].SemanticName = "AGE";

	Egg::Mesh::VertexStreamDesc particleBufferDesc;
	particleBufferDesc.elements = particleElements;
	particleBufferDesc.nElements = 3;
	particleBufferDesc.nVertices =	particles.size();
	particleBufferDesc.vertexStride = sizeof(Particle);
	particleBufferDesc.vertexData =	&particles.at(0);
	particleBufferDesc.cpuAccessFlags =	D3D11_CPU_ACCESS_WRITE;
	particleBufferDesc.usage = D3D11_USAGE_DYNAMIC;

	Egg::Mesh::VertexStream::P particleVertexStream =
		Egg::Mesh::VertexStream::create(device, particleBufferDesc);

	ID3DX11EffectPass* billboardPass =	effect->GetTechniqueByName("billboard")->GetPassByName("fire");
	Egg::Mesh::Material::P fireMaterial = Egg::Mesh::Material::create(billboardPass, 0);
	fireBillboardSet = binder->bindMaterial(fireMaterial, particleVertexStream);

	D3DX11CreateShaderResourceViewFromFileA(device, systemEnvironment.resolveMediaPath(
		"particle.dds").c_str(), NULL, NULL, &billboardSrv, NULL);
	effect->GetVariableByName("billboardTexture")->AsShaderResource()->SetResource(billboardSrv);
	/*Particle-END*/

	D3DX11CreateShaderResourceViewFromFileA(
		device,	systemEnvironment.resolveMediaPath("giraffe.jpg").c_str(),
		NULL, NULL, &kdSrv, NULL);

	D3DX11CreateShaderResourceViewFromFileA(
		device, systemEnvironment.resolveMediaPath("cloudyNoon.dds").c_str(),
		NULL, NULL, &envSrv, NULL);

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
	firstPersonCam->setView(float3(0, 0, 200), float3(0, 0, -1))->setProj(1.2, 1, 1, 1000)->setSpeed(50);

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
	fireBillboardSet.reset();
	billboardSrv->Release();
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
	fireBillboardSet->draw(context);
}

void Game::animate(double dt, double t)
{
	if (!firstPersonCam) return;
	firstPersonCam->animate(dt);
	setTransformation();

	for (int i = 0; i < particles.size(); i++)
	{
		particles.at(i).move(dt);
	}

	struct CameraDepthComparator {
		float3 ahead;
		bool operator()(const Particle& a,
			const Particle& b) {
			return
				a.position.dot(ahead) >
				b.position.dot(ahead) + 0.01;
		}
	} comp = { firstPersonCam->getAhead() };
	std::sort(particles.begin(),
		particles.end(), comp);

	ID3D11Buffer* vertexBuffer = fireBillboardSet->getGeometry()->getPrimaryBuffer();
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);
	D3D11_MAPPED_SUBRESOURCE mappedVertexData;
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexData);
	memcpy(mappedVertexData.pData, &particles.at(0), sizeof(Particle) *	particles.size());
	context->Unmap(vertexBuffer, 0);
	context->Release();
}

bool Game::processMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!firstPersonCam) return false;
	firstPersonCam->processMessage(hWnd, uMsg, wParam, lParam);
	return false;
}

HRESULT Game::createSwapChainResources()
{
	using namespace Egg::Math;
	float4 worldBillboardSize(50.0, 50.0, 0, 0);
	float4 screenBillboardSize = worldBillboardSize * firstPersonCam->getProjMatrix();
	effect->GetVariableByName("billboardWidth")->AsScalar()->SetFloat(screenBillboardSize.x);
	effect->GetVariableByName("billboardHeight")->AsScalar()->SetFloat(screenBillboardSize.y);

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

