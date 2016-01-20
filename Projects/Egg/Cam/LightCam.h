#pragma once
#include "Base.h"
#include "Shadow/Sphere.h"

namespace Egg { namespace Cam {

	class LightCam : public Egg::Cam::Base
	{
	public:
		LightCam();
		LightCam(const Egg::Math::float3& lightPos, const Egg::Math::float3& lightDir,
			const Egg::Shadow::Sphere& boundingSphere);
		~LightCam();

		typedef boost::shared_ptr<LightCam> P;

		/// Returns eye position.
		virtual const Egg::Math::float3& getEyePosition() override;
		/// Returns the ahead vector.
		virtual const Egg::Math::float3& getAhead() override;
		/// Returns the ndc-to-world-view-direction matrix to be used in shaders.
		virtual const Egg::Math::float4x4& getViewDirMatrix() override;
		/// Returns view matrix to be used in shaders.
		virtual const Egg::Math::float4x4& getViewMatrix() override;
		/// Returns projection matrix to be used in shaders.
		virtual const Egg::Math::float4x4& getProjMatrix() override;

		virtual void setAspect(float aspect) override;

	private:
		Egg::Math::float3 lightPos;
		Egg::Math::float3 lightDir;
		Egg::Math::float3 ahead;
		Egg::Shadow::Sphere boundingSphere;
		Egg::Math::float4x4 viewMtx;
		Egg::Math::float4x4 viewDirMtx;
		Egg::Math::float4x4 projMtx;
	};
} }