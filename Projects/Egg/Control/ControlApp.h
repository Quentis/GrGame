#pragma once
#include "Physics/PhysicsApp.h"
#include "Control/ControlledEntity.h"

namespace Egg { namespace Control
{
	/// Application class with scene management
	class ControlApp : public Egg::Physics::PhysicsApp
	{
	protected:
		std::vector<ControlledEntity::W> controlledEntities;
		Egg::Scene::Directory<Egg::Scene::Entity> spawnedEntities;

		class SimulationEventCallback : public physx::PxSimulationEventCallback
		{
			PhysicsApp* app;
		public:
			SimulationEventCallback(PhysicsApp* app):app(app){}
			void  onConstraintBreak (physx::PxConstraintInfo *constraints, physx::PxU32 count){} 
			void  onWake (physx::PxActor **actors, physx::PxU32 count){} 
			void  onSleep (physx::PxActor **actors, physx::PxU32 count){} 
			void  onContact (const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs); 
			void  onTrigger (physx::PxTriggerPair *pairs, physx::PxU32 count){}
		} simulationEventCallback;

	public:
		ControlApp(ID3D11Device* device);
		virtual HRESULT createResources();
		virtual HRESULT releaseResources();

		virtual void animate(double dt, double t);
		virtual bool processMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void render(ID3D11DeviceContext* context);

		void addControlledEntity(luabind::object nil, luabind::object attributes, luabind::object initializer);

		void addForceToPhysicsEntity(Egg::Physics::PhysicsEntity::P physicsEntity, luabind::object attributes);
		bool setForceAndTorqueForTargetToPhysicsEntity(Egg::Physics::PhysicsEntity::P physicsEntity, luabind::object attributes);

		void spawnControlledEntity(Egg::Physics::PhysicsEntity::P parentPhysicsEntity, luabind::object attributes, luabind::object initializer);
	};

}}