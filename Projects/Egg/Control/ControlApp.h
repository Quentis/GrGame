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