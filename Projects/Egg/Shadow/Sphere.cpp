#include "DXUT.h"
#include "Sphere.h"

using namespace Egg::Shadow;

Sphere::Sphere() : center(float3(0.0F, 0.0F, 0.0F)), radius(radius) {}

Sphere::Sphere(const float3& center, float radius) : center(center) {
	setRadius(radius);
}

Sphere::~Sphere() {}

void Sphere::setCenter(const float3& center) {
	this->center = center;
}

float3 Sphere::getCenter() const{
	return center;
}

void Sphere::setRadius(const float radius) {
	if (radius > 0.0F) {
		this->radius = radius;
	}
}

float Sphere::getRadius() const {
	return radius;
}
