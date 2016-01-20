#include "DXUT.h"
#include "Cam/LightCam.h"

using namespace Egg::Cam;

LightCam::LightCam() {}

LightCam::LightCam(const Egg::Math::float3& lightPosRef, const Egg::Math::float3& lightDirRef,
	const Egg::Shadow::Sphere& boundingSphereRef) : boundingSphere(boundingSphereRef)
{
	float3 up;

	boundingSphere = boundingSphereRef;

	if ((lightDir == float3(0.0F, 0.0F, 0.0F)).all()) {
		lightDir = (lightPosRef - boundingSphere.getCenter()).normalize();
	}
	else {
		lightDir = lightDirRef;
	}

	lightPos = boundingSphere.getCenter() - lightDir * boundingSphere.getRadius() * 2.0F;
	ahead = boundingSphere.getCenter() - lightPos;

	if (float3(0.0F, 1.0F, 0.0F).cross(ahead).length() < 0.001F) {
		up = float3(0.0F, 1.0F, 0.0F);
	}
	else {
		up = float3(1.0F, 0.0F, 0.0F);
	}

	viewMtx = float4x4::view(lightPos, ahead, up);
	float4 targetPosLC = float4(boundingSphere.getCenter(), 1.0F) * viewMtx;

	/* invTan(R^2 / (R^2 + R^2)) = 0.6154 */
	projMtx = float4x4::proj(0.6154, 1, targetPosLC.z - boundingSphere.getRadius(), targetPosLC.z + boundingSphere.getRadius());

	float4x4 ndcToTextureMtx(
		0.5F,  0.0F, 0.0F, 0.0F,
		0.0F, -0.5F, 0.0F, 0.0F,
		0.0F,  0.0F, 1.0F, 0.0F,
		0.5F,  0.5F, 0.0F, 1.0F);

	projMtx = projMtx * ndcToTextureMtx;
	
	viewDirMtx = (float4x4::view(float3::zero, ahead, up) * projMtx).invert();
}


LightCam::~LightCam()
{
}

const Egg::Math::float3& LightCam::getEyePosition() {
	return lightPos;
}

const Egg::Math::float3& LightCam::getAhead() {
	return ahead;
}

const Egg::Math::float4x4& LightCam::getViewDirMatrix() {
	return viewDirMtx;
}

const Egg::Math::float4x4& LightCam::getViewMatrix() {
	return viewMtx;
}

const Egg::Math::float4x4& LightCam::getProjMatrix() {
	return projMtx;
}

void LightCam::setAspect(float aspect) {
	// Do nothing
}
