#pragma once
#include <boost/shared_ptr.hpp>
#include "Scene/ManagerApp.h"
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
}
#include "luabind/luabind.hpp"
#include "luabind/adopt_policy.hpp"
#include "App/ThrowOnFail.h"

namespace Egg { namespace Script
{
	/// Application class with scene management
	class ScriptedApp : public Egg::Scene::ManagerApp
	{
	protected:
		lua_State* luaState;
	
		void exitWithErrorMessage(Egg::HrException exception);
	public:
		void addIndexedMesh(luabind::object nil, luabind::object attributes);
		void addMeshMaterial(luabind::object nil, luabind::object attributes, luabind::object initalizer);
		void addSrvToMaterial(Egg::Mesh::Material::P material, luabind::object attributes);

		void addMultiMesh(luabind::object nil, luabind::object attributes, luabind::object initalizer);
		void addFlipMeshToMultiMesh(Egg::Mesh::Multi::P multiMesh, luabind::object attributes, luabind::object initalizer);
		void addShadedMeshToFlipMesh(Egg::Mesh::Flip::P flipMesh, luabind::object attributes);

		void addExistingFlipMeshToMultiMesh(Egg::Mesh::Multi::P multiMesh, luabind::object attributes);

		void addMultiMeshFromFile(luabind::object nil, luabind::object attributes);

		void addStaticEntity(luabind::object nil, luabind::object attributes);
		void addFirstPersonCam(luabind::object nil, luabind::object attributes);
		void addFixedCam(luabind::object nil, luabind::object attributes);

		void runScript(const std::string& luaFilename);

		ScriptedApp(ID3D11Device* device);
		virtual HRESULT createResources();
		virtual HRESULT releaseResources();

		virtual void animate(double dt, double t);
		virtual bool processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void render(ID3D11DeviceContext* context);

	};

}}