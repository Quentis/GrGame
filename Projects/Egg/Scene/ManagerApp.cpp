#include "DXUT.h"
#include "Scene/ManagerApp.h"
#include "App/UtfConverter.h"
#include "Mesh/Indexed.h"
#include "Mesh/Importer.h"
#include "App/ThrowOnFail.h"
#include "App/stdConversions.h"
#include "Shadow/ShadowMapBuilder.h"

using namespace Egg;

HRESULT Scene::ManagerApp::createResources()
{
	SasApp::createResources();
	binder = Egg::Mesh::Binder::create(device);

	renderParameters.mien = getMien("basic");
	renderParameters.effect = effect;
	renderParameters.modelMatrix = renderParameters.effect->GetVariableByName("modelMatrix")->AsMatrix();
	renderParameters.modelMatrixInverse = renderParameters.effect->GetVariableByName("modelMatrixInverse")->AsMatrix();
	renderParameters.modelViewProjMatrix = renderParameters.effect->GetVariableByName("modelViewProjMatrix")->AsMatrix();
	renderParameters.viewDirMatrix = renderParameters.effect->GetVariableByName("viewDirMatrix")->AsMatrix();
	renderParameters.eyePos = renderParameters.effect->GetVariableByName("eyePos")->AsVector();

	enableShadows();
	shadowMapBuilder = Shadow::ShadowMapBuilder::P(new Shadow::ShadowMapBuilder);
	shadowMapBuilder->setDevice(device);


	return S_OK;
}

HRESULT Scene::ManagerApp::releaseResources()
{
	binder.reset();
	shaderResourceViews.releaseAll();
	indexedMeshes.clear();
	multiMeshes.clear();
	materials.clear();
	cameras.clear();
	entities.clear();
	return SasApp::releaseResources();
}

void Scene::ManagerApp::render(ID3D11DeviceContext* context)
{
	renderParameters.context = context;

	if(currentCamera != cameras.end())
	{
		if (shadowEnable){
			shadowMapBuilder->setLightDir(float3(0.0F, 0.0F, 0.0F)).setLightPos(float3(1.0F, 1.0F, 1.0F));
			shadowMapBuilder->setBoundingSphere(float3(0.0F, 0.0F, 0.0F), 1000.0F);

			shadowMapBuilder->buildShadowTransformRenderParams(renderParameters, getMien("shadowMap"));

			ID3D11RenderTargetView* defaultRtv = DXUTGetD3D11RenderTargetView();
			ID3D11DepthStencilView* defaultDsv = DXUTGetD3D11DepthStencilView();

			shadowMapBuilder->bindDsvAndSetNullRenderTarget(context);

			Directory<Entity>::iterator iEntity = entities.begin();
			Directory<Entity>::iterator eEntity = entities.end();
			while (iEntity != eEntity)
			{
				iEntity->second->render(renderParameters);
				iEntity++;
			}

			context->OMSetRenderTargets(1, &defaultRtv, defaultDsv);
			context->ClearDepthStencilView(defaultDsv, D3D11_CLEAR_DEPTH, 1.0, 0);
		}

		renderParameters.eyePos->SetFloatVector((float*)&currentCamera->second->getEyePosition());
		renderParameters.viewDirMatrix->SetMatrix((float*)&currentCamera->second->getViewDirMatrix());
		renderParameters.camera = currentCamera->second;
		Directory<Entity>::iterator iEntity = entities.begin();
		Directory<Entity>::iterator eEntity = entities.end();
		while(iEntity != eEntity)
		{
			iEntity->second->render(renderParameters);
			iEntity++;
		}
	}
	Sas::SasApp::render(context);
}

void Scene::ManagerApp::animate(double dt, double t)
{
	Sas::SasApp::animate(dt, t);
	if(currentCamera != cameras.end())
		currentCamera->second->animate(dt);

	Directory<Entity>::iterator iEntity = entities.begin();
	Directory<Entity>::iterator eEntity = entities.end();
	while(iEntity != eEntity)
	{
		if(!iEntity->second->animate(dt))
		{
			Directory<Entity>::iterator iKilled = iEntity;
			iKilled->second->kill();
			iEntity++;
			entities.erase(iKilled);
		}
		else
			iEntity++;
	}
}

bool Scene::ManagerApp::processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	controlParameters.processMessage(hWnd, uMsg, wParam, lParam);
	if(currentCamera != cameras.end())
	{
		currentCamera->second->processMessage(hWnd, uMsg, wParam, lParam);
		if(uMsg == WM_KEYDOWN && wParam == 'B')
		{
			currentCamera++;
			if(currentCamera == cameras.end())
				currentCamera = cameras.begin();
		}
	}
	return Sas::SasApp::processMessage(hWnd, uMsg, wParam, lParam);
}

ID3D11ShaderResourceView* Scene::ManagerApp::loadSrv(const std::string& filename, const std::string& alias)
{
	std::string filepath = App::getSystemEnvironment().resolveMediaPath(filename);
	if(filename == "")
		filepath = App::getSystemEnvironment().resolveMediaPath("uvGrid.jpg");
		
	DxResourceDirectory<ID3D11ShaderResourceView>::iterator i = shaderResourceViews.find( filepath );
	if(i != shaderResourceViews.end())
		return i->second;
	ID3D11ShaderResourceView* srv;
	ThrowOnFail((std::string("Could not load texture file: ") + filepath).c_str(), __FILE__, __LINE__) = 
		D3DX11CreateShaderResourceViewFromFileA(device, filepath.c_str(), NULL, NULL, &srv, NULL);
	shaderResourceViews[filepath] = srv;
	if(!alias.empty())
		shaderResourceViews[alias] = srv;
	return srv;
}

Mesh::Indexed::P Scene::ManagerApp::loadIndexedMesh(const std::string& filename, const std::string& alias)
{
	std::string filepath = App::getSystemEnvironment().resolveMediaPath(filename);
	Directory<Mesh::Indexed>::iterator i = indexedMeshes.find( filepath );
	if(i != indexedMeshes.end())
		return i->second;

	Assimp::Importer importer;
	const aiScene* assScene = importer.ReadFile( filepath, 0);
  
	// if the import failed
	if( !assScene || !assScene->HasMeshes() || assScene->mNumMeshes == 0) 
	{
		return Mesh::Indexed::P();
	}
	Egg::Mesh::Indexed::P indexedMesh = Egg::Mesh::Importer::fromAiMesh(device, assScene->mMeshes[0]);
	indexedMeshes[filepath] = indexedMesh;
	if(!alias.empty())
		indexedMeshes[alias] = indexedMesh;
	return indexedMesh;
}

Mesh::Mien Scene::ManagerApp::getMien(const std::string& mienName)
{
	MienDirectory::iterator i = miens.find(mienName);
	if(i != miens.end())
		return i->second;
	Mesh::Mien mien;
	miens[mienName] = mien;
	return mien;
}

Mesh::Multi::P Scene::ManagerApp::loadMultiMesh(const std::string& filename, unsigned int flags, const std::string& alias)
{
	std::string filepath = App::getSystemEnvironment().resolveMediaPath(filename);
	Directory<Mesh::Multi>::iterator i = multiMeshes.find( filepath );
	if(i != multiMeshes.end())
		return i->second;

	Assimp::Importer importer;

	const aiScene* assScene = importer.ReadFile( filepath, flags);
  
	// if the import failed
	if( !assScene || !assScene->HasMeshes() || assScene->mNumMeshes == 0) 
	{
		std::string error = importer.GetErrorString();
		throw HrException(E_FAIL, error.c_str(), __FILE__, __LINE__);
		return Mesh::Multi::P();
	}

	Mesh::Multi::P multiMesh = Mesh::Multi::create();

	for(int i=0; i<assScene->mNumMeshes; i++)
	{
		Mesh::Indexed::P indexedMesh = Mesh::Importer::fromAiMesh(device, assScene->mMeshes[i]);
		if(!alias.empty())
			indexedMeshes[alias + "[" + toString(i) + "]"] = indexedMesh;

		Mesh::Flip::P flipMesh = Mesh::Flip::create();
		addDefaultShadedMeshes(flipMesh, indexedMesh,
			assScene->mMaterials[assScene->mMeshes[i]->mMaterialIndex]);

		multiMesh->add(flipMesh);
	}
	multiMeshes[filepath] = multiMesh;
	if(!alias.empty())
		multiMeshes[alias] = multiMesh;
	return multiMesh;
}

void Scene::ManagerApp::addDefaultShadedMeshes(Mesh::Flip::P flipMesh, Mesh::Indexed::P indexedMesh, aiMaterial* assMaterial)
{
		aiString texturePath;
		assMaterial->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);

		ID3D11ShaderResourceView* srv = loadSrv(texturePath.data);

		bool skinned = false;
		const D3D11_INPUT_ELEMENT_DESC* elements;
		unsigned int nElements;
		indexedMesh->getElements(elements, nElements);
		for(int iElement=0; iElement<nElements; iElement++)
			if( strcmp(elements[iElement].SemanticName, "BLENDINDICES") == 0)
				skinned = true;

		if(!skinned)
		{
			Mesh::Material::P material = Mesh::Material::create(effect->GetTechniqueByName("textured")->GetPassByName("textured"), 0);

			ID3DX11EffectVariable* var = effect->GetVariableByName("kdTexture");
			var->AsShaderResource()->SetResource(srv);
			material->saveVariable(var);

			Mesh::Shaded::P shadedMesh = binder->bindMaterial(material, indexedMesh);
			flipMesh->add( getMien("basic"), shadedMesh);
		}
		else
		{
			Mesh::Material::P material = Mesh::Material::create(effect->GetTechniqueByName("skinned")->GetPassByName("skinned"), 0);

			ID3DX11EffectVariable* var = effect->GetVariableByName("kdTexture");
			var->AsShaderResource()->SetResource(srv);
			material->saveVariable(var);

			Mesh::Shaded::P shadedMesh = binder->bindMaterial(material, indexedMesh);
			flipMesh->add( getMien("basic"), shadedMesh);
		}
}
