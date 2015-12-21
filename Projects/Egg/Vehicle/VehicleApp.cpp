#include "DXUT.h"
#include "Vehicle/VehicleApp.h"
#include "Control/ControlApp.h"
#include "Script/LuaTable.h"
#include "Physics/PhysicsMathConversions.h"
#include "Vehicle/VehicleEntity.h"

using namespace Egg;
using namespace Egg::Physics;
using namespace Egg::Vehicle;
using namespace Egg::Control;
using namespace physx;

VehicleApp::VehicleApp(ID3D11Device* device)
	:ControlApp(device),vehicleSimulationEventCallback(this)
{
	using namespace luabind;

	module(luaState)
		[
			class_<VehicleEntity, ControlledEntity>("VehicleEntity"),
			class_<VehicleApp, bases<ControlApp, Egg::Script::ScriptedApp> >("VehicleApp")
			.def("spawnVehicle", &VehicleApp::spawnVehicle)
			.def("driveVehicle", &VehicleApp::driveVehicleEntity)
			.def("resetVehicle", &VehicleApp::resetVehicleEntity)
			.def("Drivable", &VehicleApp::setDrivableOptions)
		];

	call_function<VehicleApp*>(luaState, "setEgg", this);

	timestep = 0.005;
}

HRESULT VehicleApp::createResources()
{
	__super::createResources();

	return S_OK;
}

void VehicleApp::initializePhysX()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), false);

	// check if PvdConnection manager is available on this platform
	if(physics->getPvdConnectionManager() != NULL)
	{
		// setup connection parameters
		const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
		int             port        = 5425;         // TCP port to connect to, where PVD is listening
		unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
		// consoles and remote PCs need a higher timeout.
		PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

		// and now try to connect
		pvdConnection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(),
			pvd_host_ip, port, timeout, connectionFlags);
	}
	else
		pvdConnection = NULL;

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	if(!sceneDesc.cpuDispatcher)
	{
		cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher    = cpuDispatcher;
	}
	if(!sceneDesc.filterShader)
		sceneDesc.filterShader    = VehicleFilterShader;//gDefaultFilterShader;


	scene = physics->createScene(sceneDesc);

	cookinglib = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams());

	PxInitVehicleSDK(*physics);

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1));

	scene->simulate(0.01);
}

HRESULT VehicleApp::releaseResources()
{
	PxCloseVehicleSDK();

	return __super::releaseResources();
}

void VehicleApp::render(ID3D11DeviceContext* context)
{	
	__super::render(context);
}

void VehicleApp::animate(double dt, double t)
{
	using namespace luabind;


	__super::animate(dt, t);


}

bool VehicleApp::processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::processMessage(hWnd, uMsg, wParam, lParam);
}

//Applies control inputs to the PhysX vehicle
void VehicleApp::driveVehicleEntity(Egg::Vehicle::VehicleEntity::P vehicleEntity, luabind::object attributes)
{
	using namespace Egg::Script;
	LuaTable attributeTable(attributes, "VehicleEntity");
	try
	{
		int accel = attributeTable.getInt("accelerate", -1);
		if (accel != -1)
		{
			vehicleEntity->setAccelerate(accel == 1 ? true : false);
		}

		int brake = attributeTable.getInt("brake", -1);
		if (brake != -1)
		{
			vehicleEntity->setBrake(brake == 1 ? true : false);
		}

		int turnLeft = attributeTable.getInt("turnleft", -1);
		if (turnLeft != -1)
		{
			vehicleEntity->setTurnLeft(turnLeft == 1 ? true : false);
		}

		int turnRight = attributeTable.getInt("turnright", -1);
		if (turnRight != -1)
		{
			vehicleEntity->setTurnRight(turnRight == 1 ? true : false);
		}

		int gearDn = attributeTable.getInt("geardown", -1);
		if (gearDn != -1)
		{
			vehicleEntity->setGearDown(gearDn == 1 ? true : false);
		}

		int gearUp = attributeTable.getInt("gearup", -1);
		if (gearUp != -1)
		{
			vehicleEntity->setGearUp(gearUp == 1 ? true : false);
		}
	}
	catch(Egg::HrException exception){ Egg::Script::ScriptedApp::exitWithErrorMessage(exception); }
}

void  VehicleApp::SimulationEventCallback::onContact (const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
{
	for(PxU32 i=0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			ControlledEntity::P controlledEntity0 = boost::dynamic_pointer_cast<ControlledEntity>(((PhysicsEntity*)pairHeader.actors[0]->userData)->shared_from_this());
			ControlledEntity::P controlledEntity1 = boost::dynamic_pointer_cast<ControlledEntity>(((PhysicsEntity*)pairHeader.actors[1]->userData)->shared_from_this());

			if(controlledEntity0)
				controlledEntity0->onContact(controlledEntity1);
			if(controlledEntity1)
				controlledEntity1->onContact(controlledEntity0);
		}
	}
}

void VehicleApp::resetVehicleEntity(Egg::Vehicle::VehicleEntity::P vehicleEntity, luabind::object attributes)
{
	using namespace Egg::Script;
	LuaTable attributeTable(attributes, "VehicleEntity");
	try
	{
		vehicleEntity->resetVehicle();
	}
	catch(Egg::HrException exception){ Egg::Script::ScriptedApp::exitWithErrorMessage(exception); }

}


void VehicleApp::spawnVehicle(luabind::object nil, luabind::object attributes, luabind::object initializer)
{
	using namespace Egg::Script;
	using namespace Egg::Math;

	LuaTable attributeTable(attributes, "VehicleEntity");
	try
	{
		//Getting data from lua table
		std::string entityName = attributeTable.getString("name");
		std::string chassisMultiMeshName = attributeTable.getString("chassismultimesh");
		std::string wheel1MultiMeshName = attributeTable.getString("wheelmultimesh");
		std::string chassisphysicsmesh = App::getSystemEnvironment().resolveMediaPath(attributeTable.getString("chassisphysxmodel")) ;
		std::string  wheelphysicsmesh = App::getSystemEnvironment().resolveMediaPath(attributeTable.getString("wheelphysxmodel"));
		std::string physicsMaterialName = attributeTable.getString("material");
		float3 position = attributeTable.getFloat3("position");
		float3 axis = attributeTable.getFloat3("orientationAxis", float3(0, 1, 0));
		float angle = attributeTable.getFloat("orientationAngle");

		Scene::Directory<Mesh::Multi>::iterator iMultiMesh = multiMeshes.find(chassisMultiMeshName);
		if(iMultiMesh == multiMeshes.end())
			attributeTable.throwError(std::string("Unknown MultiMesh '") +chassisMultiMeshName+ "'.");

		Scene::Directory<Mesh::Multi>::iterator iMultiMeshWheel1 = multiMeshes.find(wheel1MultiMeshName);
		if(iMultiMeshWheel1 == multiMeshes.end())
			attributeTable.throwError(std::string("Unknown MultiMesh '") +wheel1MultiMeshName+ "'.");

		PhysicsMaterialDirectory::iterator iMaterial = physicsMaterials.find(physicsMaterialName);
		if(iMaterial == physicsMaterials.end())
			attributeTable.throwError(std::string("Unknown MultiMesh '") +physicsMaterialName+ "'.");

		Vehicle::VehicleEntity::P vehicleEntity = Vehicle::VehicleEntity::create(scene, cookinglib,iMaterial->second, position, float4::quatAxisAngle(axis, angle), iMultiMesh->second, iMultiMeshWheel1->second,chassisphysicsmesh, wheelphysicsmesh);

		vehicleEntity->setControlState(attributeTable.getLuaBindObject("controlState"));

		entities[entityName] = vehicleEntity;
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); 
	}


}


void VehicleApp::setDrivableOptions(Egg::Physics::PhysicsEntity::P entity, luabind::object attributes)
{
	using namespace Egg::Script;
	LuaTable attributeTable(attributes, "Dynamics");
	try
	{
		const int numShapes = entity->getActor()->getNbShapes();
		PxShape** shapes = new PxShape*[numShapes]; //4 wheels + chassis
		entity->getActor()->getShapes(shapes,numShapes);

		PxFilterData simFilterData;
		PxFilterData qryFilterData;

		int drivable = attributeTable.getInt("drivable", 0);

		if (drivable == 1)
		{
			//We're setting this physical object to be static, and allow driving on it
			//this means, that the wheel must not collide with this object.
			if (entity->getActor()->isRigidDynamic())
			{
				simFilterData.word0=COLLISION_FLAG_DRIVABLE_OBSTACLE;
				simFilterData.word1=COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;
				qryFilterData.word3 = (PxU32)(SAMPLEVEHICLE_DRIVABLE_SURFACE);
			}
			else
			{
				simFilterData.word0=COLLISION_FLAG_GROUND;
				simFilterData.word1=COLLISION_FLAG_GROUND_AGAINST;
				qryFilterData.word3 = (PxU32)(SAMPLEVEHICLE_DRIVABLE_SURFACE);
			}
		}
		else
		{
			//We're setting this phyysical object to be dynamic, and undrivable
			//this means, that the wheel will collide with this object.
			if (entity->getActor()->isRigidDynamic())
			{
				simFilterData.word0=COLLISION_FLAG_OBSTACLE;
				simFilterData.word1=COLLISION_FLAG_OBSTACLE_AGAINST;
				qryFilterData.word3 = (PxU32)(SAMPLEVEHICLE_UNDRIVABLE_SURFACE);
			}
			else
			{
				simFilterData.word0=COLLISION_FLAG_OBSTACLE;
				simFilterData.word1=COLLISION_FLAG_OBSTACLE_AGAINST;
				qryFilterData.word3 = (PxU32)(SAMPLEVEHICLE_UNDRIVABLE_SURFACE);
			}
		}

		for(int i = 0; i < numShapes; i++)
		{
			shapes[i]->setSimulationFilterData(simFilterData);
			shapes[i]->setQueryFilterData(qryFilterData);
		}
	}
	catch(Egg::HrException exception){ exitWithErrorMessage(exception); }
}