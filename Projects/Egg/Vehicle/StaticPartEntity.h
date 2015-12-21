#pragma once
#include "Scene/Entity.h"
#include "Mesh/Multi.h"

using namespace Egg::Scene;

namespace Egg { namespace Vehicle
{
	/// An entity with a changable model transform.
	class StaticPartEntity : public Egg::Scene::Entity
	{

		/// Position and scale of the entity
		Egg::Math::float3 position, scale;

		/// Quaternion which describes entity orientation.
		Egg::Math::float4 rotation;

		Egg::Math::float4x4 modelMatrix;


		/// Private constructor. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		StaticPartEntity(Mesh::Multi::P multiMesh)
			:Scene::Entity(multiMesh)
		{
			using namespace Egg::Math;
			position = float3();
			scale = float3(1,1,1);
			rotation = float4(0,1,0,0);
			modelMatrix = float4x4::identity;
		}

		void updateMatrix()
		{
			using namespace Egg::Math;
			modelMatrix = float4x4::scaling(scale) * float4x4::rotation(rotation.xyz,rotation.w)*float4x4::translation(position);
		}

	public:
		/// Local shorthand for shared pointer type.
		typedef boost::shared_ptr<StaticPartEntity> P;
		/// Local shorthand for weak pointer type.
		typedef boost::weak_ptr<StaticPartEntity> W;

		/// Factory method for instantiation. Initializes the model transformation to indentity.
		/// @param shadedMesh the ShadedMesh which the Entity uses for displaying itself.
		/// @return shared pointer referencing created instance
		static StaticPartEntity::P create(Mesh::Multi::P multiMesh)
		{
			return StaticPartEntity::P(new StaticPartEntity(multiMesh));
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
			return position;
		}

		/// Returns the orientation as a quaternion.
		virtual Egg::Math::float4 getOrientation()
		{
			return rotation;
		}

		/// Returns the the rotation matrix.
		virtual Egg::Math::float4x4 getRotationMatrix()
		{
			using namespace Egg::Math;
			return getModelMatrix() * float4x4::translation(-position);
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
			position += offset;
			updateMatrix();
		}

		/// Sets a position and a rotation to the model transformation.
		/// @param pos the translation vector
		void setPosition(const Egg::Math::float3& pos)
		{
			using namespace Egg::Math;
			position = pos;
			updateMatrix();
		}

		/// Appends a rotation to the model transformation.
		/// @param axis the rotation axis vector
		/// @param angle the rotation angle
		void setRotation(const Egg::Math::float4& inrot)
		{
			using namespace Egg::Math;

			float3 tmprot = inrot.xyz;
			float theta = 2*acos( inrot.w );
			float4 rot = float4((tmprot / sin(0.5f*theta)).xyz, theta);
			if ((rot.isinf().any() == false) && (rot.isnan().any() == false))
			{
				rotation=rot;
				updateMatrix();
			}
		}

	};

}}
