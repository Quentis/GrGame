#include "DXUT.h"
#include "Particle.h"

void Particle::reborn() {
	using namespace Egg::Math;
	position = float3::random(-1, 1);
	velocity = position * 5;
	age = 0;
	lifespan = float1::random(2, 5);
}