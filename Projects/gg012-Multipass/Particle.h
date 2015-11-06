#pragma once
#include "Math/math.h"

class Particle
{
	friend class Game;
	Egg::Math::float3 position;
	Egg::Math::float3 velocity;
	float lifespan;
	float age;

public:
	void reborn();
	Particle::Particle(){ reborn(); }
	void move(float dt);
};