#pragma once
#include "Scene/Entity.h"
#include "Mesh/Multi.h"

namespace Egg { namespace Scene
{
	/// An entity with a fixed model transform.
	class StaticEntity : public Egg::Scene::Entity
	{
		/// Model transformation which describes entity position and orientation.
		Egg::Math::float4x4 modelMatrix;

		/// Quaternion which describes entity orientation.
		Egg::Math::float4 orientation;

		/// Private constructor. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		StaticEntity(Mesh::Multi::P multiMesh)
			:Scene::Entity(multiMesh)
		{
			using namespace Egg::Math;
			modelMatrix = float4x4::identity;
			orientation = float4::wUnit;
		}
	public:
		/// Local shorthand for shared pointer type.
		typedef boost::shared_ptr<StaticEntity> P;
		/// Local shorthand for weak pointer type.
		typedef boost::weak_ptr<StaticEntity> W;

		/// Factory method for instantiation. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		/// @return shared pointer referencing created instance
		static StaticEntity::P create(Mesh::Multi::P multiMesh)
		{
			return StaticEntity::P(new StaticEntity(multiMesh));
		}

		/// Returns the model matrix. To be used for rendering, and positioning of light sources and cameras attached to the entity.
		virtual Egg::Math::float4x4 getModelMatrix()
		{
			return modelMatrix;
		}

		/// Returns the inverse of the model matrix.
		virtual Egg::Math::float4x4 getModelMatrixInverse()
		{
			using namespace Egg::Math;
			return modelMatrix.invert();
		}

		/// Returns reference point in world space.
		virtual Egg::Math::float3 getPosition()
		{
			using namespace Egg::Math;
			return (float4::wUnit * modelMatrix).xyz;
		}

		/// Returns the orientation as a quaternion.
		virtual Egg::Math::float4 getOrientation()
		{
			return orientation;
		}

		/// Returns the the rotation matrix.
		virtual Egg::Math::float4x4 getRotationMatrix()
		{
			using namespace Egg::Math;
			return modelMatrix * float4x4::translation(-getPosition());
		}

		/// Returns the inverse of the rotation matrix. To be used for the view transformation of attached cameras.
		virtual Egg::Math::float4x4 getRotationMatrixInverse()
		{
			return getRotationMatrix().transpose();
		}

		/// Appends a translation to the model transformation.
		/// @param offset the translation vector
		void translate(const Egg::Math::float3& offset)
		{
			using namespace Egg::Math;
			modelMatrix *= float4x4::translation(offset);
		}

		/// Appends a rotation to the model transformation.
		/// @param axis the rotation axis vector
		/// @param angle the rotation angle
		void rotate(const Egg::Math::float3& axis, float angle)
		{
			using namespace Egg::Math;
			modelMatrix *= float4x4::rotation(axis, angle);
			float4 q = float4::quatAxisAngle(axis, angle);
			orientation = orientation.quatMul(q);
		}

	};

}}
