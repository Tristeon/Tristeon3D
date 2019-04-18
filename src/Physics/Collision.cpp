#include "Collision.h"
#include "Ray.h"
#include "RigidBody.h"
#include "Core/Transform.h"

namespace Tristeon
{
	namespace Physics
	{
		Collision::Collision() : movingCollider(nullptr), staticCollider(nullptr), timeStep(0), failed(true)
		{
		}

		Collision::Collision(RigidBody* rb, BoxCollider* collider, Vector3 vel)
		{
			movingCollider = rb->collider;
			staticCollider = collider;

			if (vel.getLength() == 0) return;

			AABB a = movingCollider->aabb;
			AABB b = staticCollider->aabb;

			Vector3 const offset = a.max - a.min;
			AABB const scaledBCollider(b.min - offset / 2, b.max + offset / 2);

			int collidingSide = -1;

			Ray ray(a.getPosition(), vel);

			float f_low = -1;
			failed = !ray.lineAABBIntersection(scaledBCollider, point, vel.getLength(), f_low);
			//if (failed && AABB::AABBvsAABB(AABB(ray.origin + vel, ray.origin + vel), scaledBCollider)) Debug::logError("Fck my life");
			timeStep = f_low;
			if (failed) return;

			Vector3 const min = scaledBCollider.min;
			Vector3 const max = scaledBCollider.max;

			float const approximation = 0.001;

			Vector3 impactNormal = point - rb->transform.get()->position.get();
			impactNormal.normalize();

			//Apply small tolerance if float points do not round properly to zero
			if (abs(min.x - point.x) < approximation) collidingSide = ES_Front;
			if (abs(min.y - point.y) < approximation) collidingSide = ES_Bottom;
			if (abs(min.z - point.z) < approximation) collidingSide = ES_Right;
			if (abs(max.x - point.x) < approximation) collidingSide = ES_Behind;
			if (abs(max.y - point.y) < approximation) collidingSide = ES_Top;
			if (abs(max.z - point.z) < approximation) collidingSide = ES_Left;

			switch (collidingSide)
			{
			case -1:
				//if (vel.getLength() != 0 && !collider->isTrigger) Debug::logError("Collided side couldn't be found");
				normal = Vector3::zero;
				failed = true;
				break;
			case ES_Top:
				normal = Vector3::up;
				break;
			case ES_Right:
				normal = Vector3::back;
				break;
			case ES_Bottom:
				normal = Vector3::down;
				break;
			case ES_Left:
				normal = Vector3::forward;
				break;
			case ES_Front:
				normal = Vector3::left;
				break;
			case ES_Behind:
				normal = Vector3::right; //TODO: Check if these normals are the right direction in world space
				break;
			default:
				break;
			}
		}

		Collision::~Collision()
		{
			movingCollider = nullptr;
			staticCollider = nullptr;
		}
	}
}
