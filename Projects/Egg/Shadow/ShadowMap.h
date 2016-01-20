#pragma once
#include "d3dx11effect.h"
#include <boost/shared_ptr.hpp>
#include "Cam/LightCam.h"

using namespace Egg::Cam;

namespace Egg {
	namespace Shadow {
		class ShadowMap
		{
			friend class ShadowMapBuilder;

		public:
			typedef boost::shared_ptr<ShadowMap> P;

			ShadowMap(ID3D11Device* device, unsigned width, unsigned height);
			~ShadowMap();

			ID3D11ShaderResourceView* getDepthMapSrv();
		private:
			ShadowMap(const ShadowMap& rhs);
			ShadowMap& operator=(const ShadowMap& rhs);
		private:
			unsigned					width;
			unsigned					height;
			LightCam					lightCam;
			ID3D11Texture2D*			depthMap;
			ID3D11ShaderResourceView*	depthMapSrv;
			ID3D11DepthStencilView*		depthMapDsv;
			D3D11_VIEWPORT				viewport;

		};
	}
}
