#pragma once
#include "Math/Vector3.h"

namespace Tristeon
{
	namespace Math { struct Vector3; }
	namespace Physics
	{
		class AABB;
		class BoxCollider;
		class RigidBody;

		class Collision
		{
		public:
			Collision();
			Collision(RigidBody* rb, AABB aabb, Vector3 vel);
			Collision(RigidBody* rb, BoxCollider* collider, Vector3 vel);
			virtual ~Collision();

			/**
			 * The normal of the collider's surface that has been hit
			 */
			Vector3 normal;
			/**
			*  The position of the collider at time of impact
			*/
			Vector3 point;
			BoxCollider* movingCollider = nullptr;
			BoxCollider* staticCollider = nullptr;

			/**
			 * Time step equals -1 if collision failed
			 */
			float timeStep;
			bool failed = true;
		private:
			void calculateCollision(AABB a, AABB b,Vector3 vel, bool failOnZeroNormal);
		};
	}
}
