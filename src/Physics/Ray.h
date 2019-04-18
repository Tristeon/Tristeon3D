#pragma once
#include "BoxCollider.h"

namespace Tristeon
{
	namespace Physics
	{
		class Ray
		{
			friend class Hit;
			friend Physics;
			friend class Collision;
		public:
			Ray(Vector3 origin, Vector3 direction);
			Vector3 origin;
			Vector3 direction;
		private:
			bool lineAABBIntersection(AABB aabb, Vector3& intersectionPoint, float rayLength) const;
			bool lineAABBIntersection(AABB aabb, Vector3& intersectionPoint, float rayLength, float& f_low) const;
			bool clipLine(int axis, AABB aabb, Vector3 startPoint, Vector3 endPoint, float& f_low, float& f_high) const;
		};
	}
}