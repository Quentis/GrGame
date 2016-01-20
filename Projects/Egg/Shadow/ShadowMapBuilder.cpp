#include "DXUT.h"
#include "Shadow/ShadowMapBuilder.h"
#include "Scene/RenderParameters.h"

using namespace Egg::Shadow;
using namespace Egg::Cam;

const unsigned	ShadowMapBuilder::defaultWidth  = 1024;
const unsigned	ShadowMapBuilder::defaultHeight = 1024;
const float3	ShadowMapBuilder::defaultLightDir = float3(0.0F, 0.0F, 1.0F);
const float3	ShadowMapBuilder::defaultLightPos = float3(0.0F, 0.0F, 1.0F);;
const float3 	ShadowMapBuilder::defaultBoundingSphereCenter = float3(0.0F, 0.0F, 0.0F);
const float		ShadowMapBuilder::defaultBoundingSphereRadius = 10.0F;

ShadowMapBuilder::ShadowMapBuilder() :
	width(defaultWidth), height(defaultHeight), context(nullptr),
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

ShadowMapBuilder& ShadowMapBuilder::setContext(ID3D11DeviceContext* context) {
	this->context = context;
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


void ShadowMapBuilder::setShadowTransformRenderParams(Egg::Scene::RenderParameters& renderParameters) {
	lightCam = LightCam::P(new LightCam(lightPos, lightDir, boundingSphere));
	renderParameters.eyePos->SetFloatVector((float*)&lightCam->getEyePosition());
	renderParameters.viewDirMatrix->SetMatrix((float*)&lightCam->getViewDirMatrix());
	renderParameters.camera = lightCam;
	renderParameters.mien = 
	
}