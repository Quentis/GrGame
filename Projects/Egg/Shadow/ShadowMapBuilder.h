#pragma once

#include "Shadow/ShadowMap.h"
#include "Shadow/Sphere.h"
#include "Cam/LightCam.h"
#include "Math/math.h"
#include "Scene/RenderParameters.h"

using namespace Egg::Math;
using namespace Egg::Cam;

namespace Egg { namespace Shadow { 

	class ShadowMapBuilder
	{

	public:
		ShadowMapBuilder();
		~ShadowMapBuilder();

		ShadowMapBuilder& setWidth(unsigned width);
		ShadowMapBuilder& setHeight(unsigned height);
		ShadowMapBuilder& setContext(ID3D11DeviceContext* context);
		ShadowMapBuilder& setLightDir(const float3& lightDir);
		ShadowMapBuilder& setLightPos(const float3& lightPos);
		ShadowMapBuilder& setBoundingSphere(const float3& center, float radius);
		void setShadowTransformRenderParams(Egg::Scene::RenderParameters& renderParameters);

		ShadowMap::P getShadowMap();

	private:
		static const unsigned defaultWidth;
		static const unsigned defaultHeight;
		static const float  defaultBoundingSphereRadius;
		static const float3 defaultBoundingSphereCenter;
		static const float3 defaultLightDir;
		static const float3 defaultLightPos;

		ID3D11DeviceContext*	context;
		unsigned				width;
		unsigned				height;
		float3					lightDir;
		float3					lightPos;
		Sphere					boundingSphere;
		LightCam::P				lightCam;
	
	};

} }