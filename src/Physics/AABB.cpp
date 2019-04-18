#include "AABB.h"

namespace Tristeon
{
	namespace Physics
	{
		Vector3 AABB::getPosition() const
		{
			Vector3 const dXY = max - min;
			return min + Vector3(dXY.x / 2, dXY.y / 2, dXY.z / 2);
		}

		bool AABB::AABBvsAABB(AABB a, AABB b)
		{
			//Exit with no intersection if found separated along an axis
			if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
			if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
			if (a.max.z < b.min.z || a.min.z > b.max.z) return false;

			// No separating axis found, therefor there is at least one overlapping axis
			return true;
		}

		std::string AABB::toString() const
		{
			return "Min: " + min.toString() + "\n\tMax: " + max.toString() + "\n";
		}
	}
}