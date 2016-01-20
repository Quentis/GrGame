#pragma once

#include "ShadowMap.h"
#include "Math/math.h"

using namespace Egg::Math;

class Sphere {
public:
	Sphere() : center(float3(0.0F, 0.0F, 0.0F)), radius(radius) {}

	Sphere(const float3& center, float radius) : center(center) {
		setRadius(radius);
	}

	~Sphere() {}

	void setCenter(const float3& center) {
		this->center = center;
	}
	
	float3 getCenter() {
		return center;
	}

	void setRadius(const float radius) {
		if (radius > 0.0F) {
			this->radius = radius;
		}
	}

	float getRadius() {
		return radius;
	}

private:
	float3 center;
	float radius;
};

class ShadowMapBuilder
{
	

public:
	ShadowMapBuilder();
	~ShadowMapBuilder();

	ShadowMapBuilder& setWidth(unsigned width);
	ShadowMapBuilder& setHeight(unsigned height);
	ShadowMapBuilder& setDevice(ID3D11Device* device);
	ShadowMapBuilder& setLightDir(const float3& lightDir);
	ShadowMapBuilder& setLightPos(const float3& lightPos);
	ShadowMapBuilder& setBoundingSphere(const float3& center, float radius);
	void BuildShadowTransform();

	ShadowMap::P getShadowMap();

private:
	static const unsigned defaultWidth;
	static const unsigned defaultHeight;
	static const float  defaultBoundingSphereRadius;
	static const float3 defaultBoundingSphereCenter;
	static const float3 defaultLightDir;
	static const float3 defaultLightPos;

	ID3D11Device*	device;
	unsigned		width;
	unsigned		height;
	float3			lightDir;
	float3			lightPos;
	Sphere			boundingSphere;
	float4x4		shadowMapTransformMtx;
	
};

