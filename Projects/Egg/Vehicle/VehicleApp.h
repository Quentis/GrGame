#pragma once
#include "Control/ControlApp.h"
#include "Vehicle/VehicleEntity.h"
#include "vehicle\PxVehicleSDK.h"

namespace Egg { namespace Vehicle
{
	/// Application class with scene management
	class VehicleApp : public Egg::Control::ControlApp
	{

	protected:

		class SimulationEventCallback : public physx::PxSimulationEventCallback
		{
			VehicleApp* app;
		public:
			SimulationEventCallback(VehicleApp* app):app(app){}
			void  onConstraintBreak (physx::PxConstraintInfo *constraints, physx::PxU32 count){} 
			void  onWake (physx::PxActor **actors, physx::PxU32 count){} 
			void  onSleep (physx::PxActor **actors, physx::PxU32 count){} 
			void  onContact (const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs); 
			void  onTrigger (physx::PxTriggerPair *pairs, physx::PxU32 count){}
		} vehicleSimulationEventCallback;

		physx::PxCooking* cookinglib;

	public:

		VehicleApp(ID3D11Device* device);
		virtual HRESULT createResources();
		virtual HRESULT releaseResources();

		virtual void animate(double dt, double t);
		virtual bool processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void render(ID3D11DeviceContext* context);


		virtual void initializePhysX();

		//Applies control inputs to the PhysX vehicle
		void driveVehicleEntity(Egg::Vehicle::VehicleEntity::P vehicleEntity, luabind::object attributes);

		void spawnVehicle(luabind::object nil, luabind::object attributes, luabind::object initializer);

		void resetVehicleEntity(Egg::Vehicle::VehicleEntity::P vehicleEntity, luabind::object attributes);

		void setDrivableOptions(Egg::Physics::PhysicsEntity::P entity, luabind::object attributes);

		//This is the filter shader that is run by physx at every collision.
		//The vehicle's wheel must not collide with the ground (or with any other drivable surface), because the suspension raycast will hold the vehicle up, taking suspension into account. We filter the wheel-ground collision by returning eSUPPRESS
		//in every other case we allow the collision.
		static PxFilterFlags VehicleFilterShader(	
			PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
			PxFilterObjectAttributes attributes1, PxFilterData filterData1,
			PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
		{
			PX_FORCE_PARAMETER_REFERENCE(constantBlock);
			PX_FORCE_PARAMETER_REFERENCE(constantBlockSize);

			// let triggers through
			if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
			{
				pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
				return PxFilterFlags();
			}

			// use a group-based mechanism for all other pairs:
			// - Objects within the default group (mask 0) always collide
			// - By default, objects of the default group do not collide
			//   with any other group. If they should collide with another
			//   group then this can only be specified through the filter
			//   data of the default group objects (objects of a different
			//   group can not choose to do so)
			// - For objects that are not in the default group, a bitmask
			//   is used to define the groups they should collide with

			//UNCOMMENT
			if ((filterData0.word0 != 0 && filterData1.word0 != 0) &&
				!(filterData0.word0&filterData1.word1 || filterData1.word0&filterData0.word1))
				return PxFilterFlag::eSUPPRESS;


			pairFlags = PxPairFlag::eCONTACT_DEFAULT;

			//enable CCD stuff -- for now just for everything or nothing.
			if((filterData0.word3|filterData1.word3) & 1) // 1 ==> SWEPT_INTEGRATION_LINEAR
				pairFlags |= PxPairFlag::eSWEPT_INTEGRATION_LINEAR;

			// The pairFlags for each object are stored in word2 of the filter data. Combine them.
			pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));
			return PxFilterFlags();

		}

	};
}}