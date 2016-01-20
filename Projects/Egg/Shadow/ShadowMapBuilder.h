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
		typedef boost::shared_ptr<ShadowMapBuilder> P;

		ShadowMapBuilder();
		~ShadowMapBuilder();

		ShadowMapBuilder& setWidth(unsigned width);
		ShadowMapBuilder& setHeight(unsigned height);
		ShadowMapBuilder& setDevice(ID3D11Device* device);
		ShadowMapBuilder& setLightDir(const float3& lightDir);
		ShadowMapBuilder& setLightPos(const float3& lightPos);
		ShadowMapBuilder& setBoundingSphere(const float3& center, float radius);

		void buildShadowTransformRenderParams(Egg::Scene::RenderParameters& renderParameters, Egg::Mesh::Mien shadowMien);
		void bindDsvAndSetNullRenderTarget(ID3D11DeviceContext* context);

		ShadowMap::P getShadowMap();

	private:
		static const unsigned defaultWidth;
		static const unsigned defaultHeight;
		static const float  defaultBoundingSphereRadius;
		static const float3 defaultBoundingSphereCenter;
		static const float3 defaultLightDir;
		static const float3 defaultLightPos;

		ID3D11Device*				device;
		ID3D11DeviceContext*		context;
		unsigned					width;
		unsigned					height;
		float3						lightDir;
		float3						lightPos;
		Sphere						boundingSphere;
		LightCam::P					lightCam;
		ID3D11Texture2D*			depthMap;
		ID3D11ShaderResourceView*	depthMapSrv;
		ID3D11DepthStencilView*		depthMapDsv;
		D3D11_VIEWPORT				viewport;
	
	};

} }