#include "DXUT.h"
#include "Control/ControlApp.h"
#include "Script/LuaTable.h"
#include "Physics/PhysicsMathConversions.h"
#include "Control/ControlledEntity.h"

using namespace Egg;
using namespace Egg::Physics;
using namespace Egg::Control;
using namespace physx;

ControlApp::ControlApp(ID3D11Device* device)
	:PhysicsApp(device)
{
	using namespace luabind;

	module(luaState)
	[
		class_<ControlledEntity, PhysicsEntity>("CControlledEntity"),

		class_<ControlApp, bases<PhysicsApp, Egg::Script::ScriptedApp> >("ControlApp")
			.def("ControlledEntity", &ControlApp::addControlledEntity)
			.def("addForceAndTorque", &ControlApp::addForceToPhysicsEntity)
			.def("setForceAndTorqueForTarget", &ControlApp::setForceAndTorqueForTargetToPhysicsEntity)
			.def("spawn", &ControlApp::spawnControlledEntity)
	];

	call_function<ControlApp*>(luaState, "setEgg", this);
}

HRESULT ControlApp::createResources()
{
	__super::createResources();

	return S_OK;
}

HRESULT ControlApp::releaseResources()
{
	std::vector<ControlledEntity::W>::iterator iControlledEntity = controlledEntities.begin();
	while(iControlledEntity != controlledEntities.end())
	{
		ControlledEntity::P controlledEntity = (*iControlledEntity).lock();
		if(controlledEntity)
			controlledEntity->releaseControlState();
		iControlledEntity++;
	}
	controlledEntities.clear();
	return __super::releaseResources();
}

void ControlApp::render(ID3D11DeviceContext* context)
{	
	__super::render(context);
}

void ControlApp::animate(double dt, double t)
{
	using namespace luabind;
	try
	{
		object keysPressed = globals(luaState)["keysPressed"];
		if( type(keysPressed) == LUA_TTABLE )
		{
			int iKey=0;
			for (iterator i(keysPressed), end; i != end; ++i, ++iKey)
			{
				std::string kks = luabind::object_cast<std::string>(i.key());
				int keycode = kks.at(0);
				if( kks == "VK_SPACE" )   keycode = VK_SPACE;
				if( kks == "VK_NUMPAD0" ) keycode = VK_NUMPAD0;
				if( kks == "VK_NUMPAD1" ) keycode = VK_NUMPAD1;
				if( kks == "VK_NUMPAD2" ) keycode = VK_NUMPAD2;
				if( kks == "VK_NUMPAD3" ) keycode = VK_NUMPAD3;
				if( kks == "VK_NUMPAD4" ) keycode = VK_NUMPAD4;
				if( kks == "VK_NUMPAD5" ) keycode = VK_NUMPAD5;
				if( kks == "VK_NUMPAD6" ) keycode = VK_NUMPAD6;
				if( kks == "VK_NUMPAD7" ) keycode = VK_NUMPAD7;
				if( kks == "VK_NUMPAD8" ) keycode = VK_NUMPAD8;
				if( kks == "VK_NUMPAD9" ) keycode = VK_NUMPAD9;

				*i = controlParameters.keyPressed[keycode]?true:false;
			}
		}
		timeRemainingOfTimestep -= dt;
		if(timeRemainingOfTimestep < 0)
		{
			timeRemainingOfTimestep += timestep;
			globals(luaState)["dt"] = timestep;
			__super::animate(dt, t);
			//LABTODO merge spawned entities
			scene->simulate(timestep);
			scene->fetchResults(true);
		}
	}
	catch(luabind::error exception){ 
		std::string errs = lua_tostring(luaState, -1);
		MessageBoxA( NULL, errs.c_str(), "Script error!", MB_OK);
		exit(-1); }
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }

	__super::animate(dt, t);
}

bool ControlApp::processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::processMessage(hWnd, uMsg, wParam, lParam);
}

void ControlApp::addControlledEntity(luabind::object nil, luabind::object attributes, luabind::object initializer)
{
	using namespace Egg::Script;
	LuaTable attributeTable(attributes, "ControlledEntity");
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
		ControlledEntity::P controlledEntity = ControlledEntity::create(scene, position, float4::quatAxisAngle(axis, angle), iMultiMesh->second);
		controlledEntity->setControlState(attributeTable.getLuaBindObject("controlState"));
		densityForEntityBeingAdded = 1.0;
		initializer( boost::dynamic_pointer_cast<PhysicsEntity>(controlledEntity) );
		if(PxRigidDynamic* rigidDynamic = controlledEntity->getActor()->isRigidDynamic())
			PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, densityForEntityBeingAdded);
		controlledEntity->getActor()->userData = controlledEntity.get();
		scene->addActor(*controlledEntity->getActor());
		entities[entityName] = controlledEntity;
		controlledEntities.push_back(controlledEntity);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

void ControlApp::addForceToPhysicsEntity(PhysicsEntity::P physicsEntity, luabind::object attributes)
{
	try
	{
		using namespace Egg::Script;
		LuaTable attributeTable(attributes, "addForceAndTorque");
		PxRigidDynamic* rigidDynamic = physicsEntity->getActor()->isRigidDynamic();
		if(rigidDynamic)
		{
			rigidDynamic->addForce( ~((attributeTable.getFloat3("force").xyz1 *  physicsEntity->getRotationMatrix()).xyz));
			rigidDynamic->addTorque(~((attributeTable.getFloat3("torque").xyz1 * physicsEntity->getRotationMatrix()).xyz));
		}
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

bool ControlApp::setForceAndTorqueForTargetToPhysicsEntity(PhysicsEntity::P physicsEntity, luabind::object attributes)
{
	try
	{
		using namespace Egg::Scene;
		using namespace Egg::Script;
		using namespace luabind;
		using namespace Egg::Math;

		PxRigidDynamic* rigidDynamic = physicsEntity->getActor()->isRigidDynamic();
		if(!rigidDynamic)
			return false;

		LuaTable attributeTable(attributes, "addForceAndTorqueToTarget");
		float proximityRadius = attributeTable.getFloat("proximityRadius");
		float maxForce = attributeTable.getFloat("maxForce");
		float maxTorque = attributeTable.getFloat("maxTorque");
		float3 markPosition = attributeTable.getFloat3("position");
		std::string markName = attributeTable.getString("mark", "");
		if(!markName.empty())
		{
			Directory<Entity>::iterator iEntity = entities.find(markName);
			//TODO: handle cases where mark has been destroyed
			if(iEntity == entities.end())
				attributeTable.throwError(std::string("Unknown entity ") + markName + " specified as mark.");
			markPosition = (markPosition.xyz1 * iEntity->second->getModelMatrix()).xyz;
		}
		// LABTODO: compute force and torque
		float4 ahead4 = float4(1, 0, 0, 0) * physicsEntity->getRotationMatrix();
		float3 ahead = ahead4.xyz;
		float3 markDir = markPosition - physicsEntity->getPosition();
		float markDist = markDir.length();
		markDir = markDir.normalize();

		rigidDynamic->addForce( ~(ahead * ahead.dot(markDir)   * maxForce));
		rigidDynamic->addTorque(~(ahead * ahead.cross(markDir) * maxTorque));

		return proximityRadius > markDist;
		// LABTODO: if reached waypoint, return true
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }

	return false;
}

void ControlApp::spawnControlledEntity(PhysicsEntity::P parentPhysicsEntity, luabind::object attributes, luabind::object initializer)
{
	using namespace Egg::Script;
	LuaTable attributeTable(attributes, "spawn");
	try
	{
		std::string entityName = attributeTable.getString("name");
		std::string multiMeshName = attributeTable.getString("multiMesh");
		using namespace Egg::Math;
		float3 position = attributeTable.getFloat3("position");
		position = (position.xyz1 * parentPhysicsEntity->getModelMatrix()).xyz;
		float3 axis = attributeTable.getFloat3("orientationAxis", float3(0, 1, 0));
		float angle = attributeTable.getFloat("orientationAngle");
		float4 orientation = parentPhysicsEntity->getOrientation().quatMul( float4::quatAxisAngle(axis, angle) );

		Scene::Directory<Mesh::Multi>::iterator iMultiMesh = multiMeshes.find(multiMeshName);
		if(iMultiMesh == multiMeshes.end())
			attributeTable.throwError(std::string("Unknown MultiMesh '") +multiMeshName+ "'.");
		ControlledEntity::P controlledEntity = ControlledEntity::create(scene, position, orientation, iMultiMesh->second);
		controlledEntity->setControlState(attributeTable.getLuaBindObject("controlState"));
		densityForEntityBeingAdded = 1.0;
		initializer( boost::dynamic_pointer_cast<PhysicsEntity>(controlledEntity) );
		if(PxRigidDynamic* rigidDynamic = controlledEntity->getActor()->isRigidDynamic())
		{
			rigidDynamic->setLinearVelocity( ~((~rigidDynamic->getLinearVelocity()).xyz1 * parentPhysicsEntity->getRotationMatrix()).xyz );
			rigidDynamic->setAngularVelocity( ~((~rigidDynamic->getAngularVelocity()).xyz1 * parentPhysicsEntity->getRotationMatrix()).xyz );

			PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, densityForEntityBeingAdded);
		}
		controlledEntity->getActor()->userData = controlledEntity.get();
		scene->addActor(*controlledEntity->getActor());
		//LABTODO insert entity into container for spawned entities
		controlledEntities.push_back(controlledEntity);
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}

