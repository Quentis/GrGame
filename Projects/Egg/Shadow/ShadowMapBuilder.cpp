#include "DXUT.h"
#include "Shadow/ShadowMapBuilder.h"
#include "Scene/RenderParameters.h"
#include "d3dx11effect.h"

using namespace Egg::Shadow;
using namespace Egg::Cam;

const unsigned	ShadowMapBuilder::defaultWidth  = 1024;
const unsigned	ShadowMapBuilder::defaultHeight = 1024;
const float3	ShadowMapBuilder::defaultLightDir = float3(0.0F, 0.0F, 1.0F);
const float3	ShadowMapBuilder::defaultLightPos = float3(0.0F, 0.0F, 1.0F);;
const float3 	ShadowMapBuilder::defaultBoundingSphereCenter = float3(0.0F, 0.0F, 0.0F);
const float		ShadowMapBuilder::defaultBoundingSphereRadius = 10.0F;

ShadowMapBuilder::ShadowMapBuilder() :
width(defaultWidth), height(defaultHeight), device(nullptr), context(nullptr),
	lightDir(defaultLightDir), lightPos(defaultLightPos),
	boundingSphere(defaultBoundingSphereCenter, defaultBoundingSphereRadius), 
	depthMap(nullptr), depthMapSrv(nullptr), depthMapDsv(nullptr)
{
	// Viewport that matches the shadow map dimensions.
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going
	// to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	device->CreateTexture2D(&texDesc, 0, &depthMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDsv);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(depthMap, &srvDesc, &depthMapSrv);
}

ShadowMapBuilder::~ShadowMapBuilder()
{
	depthMap->Release();
	depthMapDsv->Release();
	depthMapSrv->Release();
}

ShadowMapBuilder& ShadowMapBuilder::setWidth(unsigned width) {
	if (width != 0U) {
		this->width = width;
	}
	else {
		this->width = defaultWidth;
	}
	return *this;
}

ShadowMapBuilder& ShadowMapBuilder::setHeight(unsigned height) {
	if (height != 0U) {
		this->height = height;
	}
	else {
		this->height = defaultHeight;
	}
	return *this;
}

ShadowMapBuilder& ShadowMapBuilder::setDevice(ID3D11Device* device) {
	this->device = device;
	return *this;
}

ShadowMapBuilder& ShadowMapBuilder::setLightDir(const float3& lightDir) {
	this->lightDir = lightDir;
	return *this;
}

ShadowMapBuilder& ShadowMapBuilder::setLightPos(const float3& lightPos) {
	this->lightPos = lightPos;
	return *this;
}

ShadowMapBuilder& ShadowMapBuilder::setBoundingSphere(const float3& center, float radius) {
	this->boundingSphere.setCenter(center);
	this->boundingSphere.setRadius(radius);
	return *this;
}


void ShadowMapBuilder::buildShadowTransformRenderParams(Egg::Scene::RenderParameters& renderParameters, Egg::Mesh::Mien shadowMien) {
	lightCam = LightCam::P(new LightCam(lightPos, lightDir, boundingSphere));
	renderParameters.eyePos->SetFloatVector((float*)&lightCam->getEyePosition());
	renderParameters.viewDirMatrix->SetMatrix((float*)&lightCam->getViewDirMatrix());
	renderParameters.camera = lightCam;
}

void ShadowMapBuilder::bindDsvAndSetNullRenderTarget(ID3D11DeviceContext* context)
{
	context->RSSetViewports(1, &viewport);
	// Set null render target because we are only going to draw
	// to depth buffer. Setting a null render target will disable
	// color writes.
	ID3D11RenderTargetView* renderTargets[1] = { nullptr };
	context->OMSetRenderTargets(1, renderTargets, depthMapDsv);
	context->ClearDepthStencilView(depthMapDsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
}