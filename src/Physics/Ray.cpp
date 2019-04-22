#include "Ray.h"
#include <algorithm>

namespace Tristeon
{
	namespace Physics
	{
		bool Ray::lineAABBIntersection(AABB aabb, Vector3& intersectionPoint, float rayLength) const
		{
			//If the ray is not pointing towards the object return false
			if (direction.dot(aabb.getPosition() - origin) < 0) return false;

			float f_low = 0;
			float f_high = 1;

			//Clip X-axis
			if (!clipLine(0, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			//Clip Y-axis
			if (!clipLine(1, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			//Clip Z-axis
			if (!clipLine(2, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			intersectionPoint = origin + direction * f_low * rayLength;

			return true;
		}

		bool Ray::lineAABBIntersection(AABB aabb, Vector3& intersectionPoint, float rayLength, float& f_low) const
		{
			//If the ray is not pointing towards the object return false
			//if (direction.dot(aabb.getPosition() - origin) < 0) return false;

			f_low = 0;
			float f_high = 1;

			//Clip X-axis
			if (!clipLine(0, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			//Clip Y-axis
			if (!clipLine(1, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			//Clip Z-axis
			if (!clipLine(2, aabb, origin, origin + direction * rayLength, f_low, f_high))
				return false;

			intersectionPoint = origin + direction * f_low * rayLength;

			return true;
		}

		bool Ray::clipLine(int axis, AABB aabb, Vector3 startPoint, Vector3 endPoint, float& f_low, float& f_high) const
		{
			//If 0, line does not move within this dimension
			if (endPoint.getAxis(axis) - startPoint.getAxis(axis) == 0)
				return startPoint.getAxis(axis) > aabb.min.getAxis(axis) && startPoint.getAxis(axis) < aabb.max.getAxis(axis);

			float f_dim_low = (aabb.min.getAxis(axis) - startPoint.getAxis(axis)) / (endPoint.getAxis(axis) - startPoint.getAxis(axis));
			float f_dim_high = (aabb.max.getAxis(axis) - startPoint.getAxis(axis)) / (endPoint.getAxis(axis) - startPoint.getAxis(axis));

			if (f_dim_low > f_dim_high) std::swap(f_dim_low, f_dim_high);

			if (f_dim_low > f_high) return false;
			if (f_dim_high < f_low) return false;

			f_low = std::max(f_dim_low, f_low);
			f_high = std::max(f_dim_high, f_high);

			if (f_low > f_high) return false;
			if (f_low > 1) return false;

			return true;
		}

		Ray::Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction)
		{
			this->direction.normalize();
		}
	}
}