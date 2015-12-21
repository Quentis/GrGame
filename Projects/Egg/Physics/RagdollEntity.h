#pragma once
#include "Physics/PhysicsEntity.h"
#include "Mesh/Multi.h"
#include "PxPhysicsAPI.h"

namespace Egg { namespace Physics
{
	class RagdollEntity : public Egg::Scene::Entity
	{
	protected:
		std::vector<Egg::Physics::PhysicsEntity::P> boneEntities;
		std::vector<Egg::Math::float4x4> riggingBoneTransforms;

		/// Private constructor. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		RagdollEntity(Egg::Math::float3 position, Egg::Math::float4 orientation, Mesh::Multi::P multiMesh);
	public:
		/// Local shorthand for shared pointer type.
		typedef boost::shared_ptr<RagdollEntity> P;
		/// Local shorthand for weak pointer type.
		typedef boost::weak_ptr<RagdollEntity> W;

		/// Factory method for instantiation. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		/// @return shared pointer referencing created instance
		static RagdollEntity::P create(Egg::Math::float3 position, Egg::Math::float4 orientation, Mesh::Multi::P multiMesh)
		{
			return RagdollEntity::P(new RagdollEntity(position, orientation, multiMesh));
		}

		virtual ~RagdollEntity();

		/// Returns the model matrix. To be used for rendering, and positioning of light sources and cameras attached to the entity.
		virtual Egg::Math::float4x4 getModelMatrix();

		/// Returns the inverse of the model matrix.
		virtual Egg::Math::float4x4 getModelMatrixInverse();

		/// Returns reference point in world space.
		virtual Egg::Math::float3 getPosition();

		/// Returns orientation as a quaternion in world space.
		virtual Egg::Math::float4 getOrientation();

		/// Returns the rotation matrix.
		virtual Egg::Math::float4x4 getRotationMatrix();

		/// Returns the inverse of the rotation matrix. To be used for the view transformation of attached cameras.
		virtual Egg::Math::float4x4 getRotationMatrixInverse();

		void addBoneEntity(PhysicsEntity::P boneEntity);
		void render(const Egg::Scene::RenderParameters& renderParameters);

	};

}}
