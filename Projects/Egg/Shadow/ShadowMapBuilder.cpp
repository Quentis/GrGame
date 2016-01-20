#include "DXUT.h"
#include "ShadowMapBuilder.h"


const unsigned	ShadowMapBuilder::defaultWidth  = 1024;
const unsigned	ShadowMapBuilder::defaultHeight = 1024;
const float3	ShadowMapBuilder::defaultLightDir = float3(0.0F, 0.0F, 1.0F);
const float3	ShadowMapBuilder::defaultLightPos = float3(0.0F, 0.0F, 1.0F);;
const float3 	ShadowMapBuilder::defaultBoundingSphereCenter = float3(0.0F, 0.0F, 0.0F);
const float		ShadowMapBuilder::defaultBoundingSphereRadius = 10.0F;

ShadowMapBuilder::ShadowMapBuilder() :
	width(defaultWidth), height(defaultHeight), device(nullptr),
	lightDir(defaultLightDir), lightPos(defaultLightPos),
	boundingSphere(defaultBoundingSphereCenter, defaultBoundingSphereRadius)
{}

ShadowMapBuilder::~ShadowMapBuilder(){}

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

void ShadowMapBuilder::BuildShadowTransform() {
	float3 lightToCenter;
	float3 up;

	if ((lightDir == float3(0.0F, 0.0F, 0.0F)).all()) {
		lightDir = (lightPos - boundingSphere.getCenter()).normalize();
	}

	lightPos = boundingSphere.getCenter() - lightDir * boundingSphere.getRadius() * 2.0F;
	lightToCenter = boundingSphere.getCenter() - lightPos;
	
	if (float3(0.0F, 1.0F, 0.0F).cross(lightToCenter).length() < 0.001F) {
		up = float3(0.0F, 1.0F, 0.0F);
	}
	else {
		up = float3(1.0F, 0.0F, 0.0F);
	}

	float4x4 lightViewMtx = float4x4::view(lightPos, lightToCenter, up);
	float4 targetPosLC = float4(boundingSphere.getCenter(), 1.0F) * lightViewMtx;

	/* invTan(R^2 / (R^2 + R^2)) = 0.6154 */
	float4x4 lightProjMtx = float4x4::proj(0.6154, 1, targetPosLC.z - boundingSphere.getRadius(), targetPosLC.z + boundingSphere.getRadius());

	float4x4 ndcToTextureMtx(
		0.5F,  0.0F, 0.0F, 0.0F,
		0.0F, -0.5F, 0.0F, 0.0F,
		0.0F,  0.0F, 1.0F, 0.0F,
		0.5F,  0.5F, 0.0F, 1.0F);

	float4x4 shadowMapTransformMtx = lightViewMtx * lightProjMtx * ndcToTextureMtx;

}