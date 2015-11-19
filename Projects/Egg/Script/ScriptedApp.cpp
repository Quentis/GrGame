#include "DXUT.h"
#include "Script/ScriptedApp.h"
#include "Script/LuaTable.h"
#include "Scene/StaticEntity.h"
#include "Cam/FirstPerson.h"

using namespace Egg;

Script::ScriptedApp::ScriptedApp(ID3D11Device* device)
	:ManagerApp(device)
{
	using namespace luabind;

	luaState = lua_open();

	open(luaState);
	luaL_openlibs(luaState);

	module(luaState)
	[
		class_<Mesh::Indexed>("MeshIndexed"),
		class_<Mesh::Flip>("MeshFlip"),
		class_<Mesh::Multi>("MeshMulti"),
		class_<Mesh::Material>("MeshMaterial"),

		class_<Script::ScriptedApp>("ScriptedApp")
			.def("IndexedMesh", &Script::ScriptedApp::addIndexedMesh)
			.def("Material", &Script::ScriptedApp::addMeshMaterial)
			.def("setSrv", &Script::ScriptedApp::addSrvToMaterial)
			.def("MultiMesh", &Script::ScriptedApp::addMultiMesh)
			.def("FlipMesh", &Script::ScriptedApp::addFlipMeshToMultiMesh)
			.def("ShadedMesh", &Script::ScriptedApp::addShadedMeshToFlipMesh)
			.def("MultiMeshFromFile", &Script::ScriptedApp::addMultiMeshFromFile)
			.def("StaticEntity", &Script::ScriptedApp::addStaticEntity)
			.def("FirstPersonCam", &Script::ScriptedApp::addFirstPersonCam)
			.def("FlipMeshFromExistingMultiMesh", &Script::ScriptedApp::addExistingFlipMeshToMultiMesh)
	];

	int s = luaL_dostring(luaState, "O = nil; _ = nil; function setEgg(egg) O = egg end");
	if(s != 0){std::string errs = lua_tostring(luaState, -1);MessageBoxA( NULL, errs.c_str(), "Lua error!", MB_OK);  }
	call_function<Script::ScriptedApp*>(luaState, "setEgg", this);
}

HRESULT Script::ScriptedApp::createResources()
{
	return Scene::ManagerApp::createResources();
}

HRESULT Script::ScriptedApp::releaseResources()
{
	// no references to lua objects should remain (e.g. controlStates)
	lua_close(luaState);
	return Scene::ManagerApp::releaseResources();
}

void Script::ScriptedApp::render(ID3D11DeviceContext* context)
{
	
	Scene::ManagerApp::render(context);
}

void Script::ScriptedApp::animate(double dt, double t)
{
	Scene::ManagerApp::animate(dt, t);
}


bool Script::ScriptedApp::processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Scene::ManagerApp::processMessage(hWnd, uMsg, wParam, lParam);
}

void Script::ScriptedApp::exitWithErrorMessage(Egg::HrException exception)
{
	std::stringstream errorMessage;
	errorMessage << exception.errorMessage << " [" << exception.filename << "," << exception.lineNumber << "]";
	MessageBoxA( NULL, errorMessage.str().c_str(), "Error!", MB_OK);
	exit(-1);
}

void Script::ScriptedApp::addIndexedMesh(luabind::object nil, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "IndexedMesh");
	try
	{
		std::string indexedMeshName = attributeTable.getString("name");
		std::string fileName = attributeTable.getString("file");
		Egg::Mesh::Indexed::P indexedMesh = loadIndexedMesh(fileName);
		indexedMeshes[indexedMeshName] = indexedMesh;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addMeshMaterial(luabind::object nil, luabind::object attributes, luabind::object initializer)
{
	LuaTable attributeTable(attributes, "MeshMaterial");
	try
	{
		std::string materialName = attributeTable.getString("name");
		std::string techniqueName = attributeTable.getString("technique");
		std::string passName = attributeTable.getString("pass");
		ID3DX11EffectTechnique* technique = effect->GetTechniqueByName(techniqueName.c_str());
		if(!technique->IsValid()) 
			attributeTable.throwError(std::string("Technique '") +techniqueName+ "' not found in effect.");
		ID3DX11EffectPass* pass = technique->GetPassByName(passName.c_str());
		if(!pass->IsValid())
			attributeTable.throwError(std::string("Pass <<") +passName+ ">> not found in technique '" + techniqueName + "'.");
		Egg::Mesh::Material::P material = Egg::Mesh::Material::create(pass, 0);
		initializer(material);
		materials[materialName] = material;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addSrvToMaterial(Egg::Mesh::Material::P material, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "setSrv");
	try
	{
		std::string effectVariableName = attributeTable.getString("effectVariable");
		ID3DX11EffectVariable* effectVariable = effect->GetVariableByName(effectVariableName.c_str());
		if(!effectVariable->IsValid())
			attributeTable.throwError(std::string("Variable '") +effectVariableName+ "' not found in effect.");
		std::string fileName = attributeTable.getString("file");
		ID3D11ShaderResourceView* srv = loadSrv(fileName);
		material->set(effectVariable)->AsShaderResource()->SetResource(srv);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addMultiMesh(luabind::object nil, luabind::object attributes, luabind::object initializer)
{
	LuaTable attributeTable(attributes, "MultiMesh");
	try
	{
		std::string name = attributeTable.getString("name");
		Egg::Mesh::Multi::P multiMesh = Egg::Mesh::Multi::create();
		initializer(multiMesh);
		multiMeshes[name] = multiMesh;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addFlipMeshToMultiMesh(Egg::Mesh::Multi::P multiMesh, luabind::object attributes, luabind::object initializer)
{
	LuaTable attributeTable(attributes, "FlipMesh");
	try
	{
		Egg::Mesh::Flip::P flipMesh = Egg::Mesh::Flip::create();
		initializer(flipMesh);
		multiMesh->add(flipMesh);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addExistingFlipMeshToMultiMesh(
	Egg::Mesh::Multi::P multiMesh, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "ExisitingFlipMesh");
	try
	{
		std::string mmName = attributeTable.getString("fromMultiMesh");
		int index = attributeTable.getInt("subMeshIndex");
		Scene::Directory<Mesh::Multi>::iterator i = multiMeshes.find(mmName);
		if(i == multiMeshes.end())
			attributeTable.throwError("No such multiMesh!");
		Egg::Mesh::Flip::P flipMesh = i->second->getSubmesh(index);
		multiMesh->add(flipMesh);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addShadedMeshToFlipMesh(Egg::Mesh::Flip::P flipMesh, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "ShadedMesh");
	try
	{
		std::string mienName = attributeTable.getString("mien");
		std::string indexedName = attributeTable.getString("indexedMesh");
		std::string materialName = attributeTable.getString("material");
		Scene::Directory<Mesh::Indexed>::iterator iIndexed = indexedMeshes.find(indexedName);
		if(iIndexed == indexedMeshes.end())
			attributeTable.throwError(std::string("Unknown IndexedMesh '") +indexedName+ "'.");
		Scene::Directory<Mesh::Material>::iterator iMaterial = materials.find(materialName);
		if(iMaterial == materials.end())
			attributeTable.throwError(std::string("Unknown Material '") +materialName+ "'.");
		Egg::Mesh::Shaded::P shadedMesh = binder->bindMaterial(iMaterial->second, iIndexed->second);
		flipMesh->add(getMien(mienName), shadedMesh);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addMultiMeshFromFile(luabind::object nil, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "MultiMeshFromFile");
	try
	{
		std::string name = attributeTable.getString("name");
		std::string filename = attributeTable.getString("file");
		Egg::Mesh::Multi::P multi = loadMultiMesh(filename, 0, name);
		multiMeshes[name] = multi;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addStaticEntity(luabind::object nil, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "StaticEntity");
	try
	{
		std::string entityName = attributeTable.getString("name");
		std::string multiMeshName = attributeTable.getString("multiMesh");
		using namespace Egg::Math;
		float3 position = attributeTable.getFloat3("position");
		float3 axis = attributeTable.getFloat3("orientationAxis", float3(0, 1, 0));
		float angle = attributeTable.getFloat("orientationAngle");
		Scene::Directory<Mesh::Multi>::iterator iMultiMesh = multiMeshes.find(multiMeshName);
		if(iMultiMesh == multiMeshes.end())
			attributeTable.throwError(std::string("Unknown MultiMesh '") +multiMeshName+ "'.");
		Egg::Scene::StaticEntity::P staticEntity = Egg::Scene::StaticEntity::create(iMultiMesh->second);
		staticEntity->translate(position);
		staticEntity->rotate(axis, angle);
		entities[entityName] = staticEntity;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::addFirstPersonCam(luabind::object nil, luabind::object attributes)
{
	LuaTable attributeTable(attributes, "FirstPersonCam");
	try
	{
		using namespace Egg::Math;
		std::string name = attributeTable.getString("name");
		Egg::Cam::FirstPerson::P firstPersonCam = Egg::Cam::FirstPerson::create()
			/*->setView(attributeTable.getFloat3("position", float3::zero), attributeTable.getFloat3("ahead", -float3::zUnit))
			  ->setProj(attributeTable.getFloat("fov", 1.2), attributeTable.getFloat("aspect", 1), attributeTable.getFloat("front", 0.1), attributeTable.getFloat("back", 1000.0))
			  ->setSpeed(attributeTable.getFloat("speed", 5));*/;

		cameras[name] = firstPersonCam;
		currentCamera = cameras.begin();
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void Script::ScriptedApp::runScript(const std::string& luaFilename)
{
	std::string filepath = App::getSystemEnvironment().resolveMediaPath(luaFilename);
	int s = luaL_dofile(luaState, filepath.c_str());

	if(s != 0)
	{
		std::string errs = lua_tostring(luaState, -1);
		MessageBoxA( NULL, errs.c_str(), "Script error!", MB_OK);
		exit(-1);
	}
}