#pragma once

#include "Math/math.h"

using namespace Egg::Math;

namespace Egg {
	namespace Shadow {

		class Sphere {
		public:
			Sphere();
			Sphere(const float3& center, float radius);
			~Sphere();

			void setCenter(const float3& center);
			float3 getCenter() const;
			void setRadius(const float radius);
			float getRadius() const;

		private:
			float3 center;
			float radius;
		};

	}
}