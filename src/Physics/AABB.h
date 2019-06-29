#pragma once
#include <Math/Vector3.h>

namespace Tristeon
{
	namespace Physics
	{
		class AABB
		{
		public:
			AABB() :min(Vector3()), max(Vector3()) {}
			AABB(Vector3 min, Vector3 max) : min(min), max(max) {}
			Vector3 getPosition() const;
			
			static bool AABBvsAABB(AABB a, AABB b);
			bool contains(AABB const& other) const;
			static bool AABBvsPoint(AABB a, Vector3 point);
			std::string toString() const;

			Vector3 min;
			Vector3 max;
		};
	}
}