#pragma once
#include <boost/shared_ptr.hpp>
#include <vector>
#include "Mesh/Multi.h"
#include "Math/math.h"
#include "Scene/RenderParameters.h"
#include "Scene/ControlParameters.h"
#include "Directory.h"

namespace Egg { namespace Scene
{
	/// Base class for game objects
	class Entity
	{
	protected:
		/// A mesh complete with shading information for multiple rendering miens for multiple geometries (render component)
		Mesh::Multi::P multiMesh;

		/// Constructor
		/// @param multiMesh the mesh object responsible for rendering the entity
		Entity(Mesh::Multi::P multiMesh):multiMesh(multiMesh){}
	public:
		/// Shader pointer to Entity class
		typedef boost::shared_ptr<Entity> P;

		virtual ~Entity(){}

		/// Returns the model matrix. To be used for rendering, and positioning of light sources and cameras attached to the entity.
		virtual Egg::Math::float4x4 getModelMatrix()=0;

		/// Returns the inverse of the model matrix. To be used for rendering.
		virtual Egg::Math::float4x4 getModelMatrixInverse()=0;

		/// Returns reference point in world space.
		virtual Egg::Math::float3 getPosition()=0;

		/// Returns the orientation as a quaternion.
		virtual Egg::Math::float4 getOrientation()=0;

		/// Returns the rotation matrix.
		virtual Egg::Math::float4x4 getRotationMatrix()=0;

		/// Returns the inverse of the rotation matrix. To be used for the view transformation of attached cameras.
		virtual Egg::Math::float4x4 getRotationMatrixInverse()=0;

		/// Renders the entity as seen from a given camera in a given mien.
		virtual void render(const RenderParameters& renderParameters);

		/// Updates time-varying entity properties.
		/// @param dt time step in seconds
		virtual bool animate(float dt){return true;}
	};
}}
