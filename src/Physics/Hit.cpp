#include "Hit.h"
#include "BoxCollider.h"
#include "Ray.h"

namespace Tristeon
{
	namespace Physics
	{
		Hit::Hit(BoxCollider* a, BoxCollider* b, Vector3 vel)
		{
			//Hit constructor calculates the point of impact and the collision normal
			//by resizing the box b by the size of box a and then putting box a 1 step back and "raycasting"
			//the velocity to see which side it hits into and what point of the box it hits into

			Vector3 const offset = a->aabb.max - a->aabb.min;
			AABB const scaledBCollider(b->aabb.min - offset / 2, b->aabb.max + offset / 2);

			Vector3 const previousPosition = a->getPosition() - vel;
			int collidingSide = -1;

			Ray ray(previousPosition, vel);

			float f_low = 0;
			failed = !ray.lineAABBIntersection(scaledBCollider, pointOfImpact, vel.getLength(), f_low);
			resultantVectorScale = 1 - f_low;

			Vector3 min = b->aabb.min;
			Vector3 max = b->aabb.max;

			float const approximation = 0.001;
			//TODO: Apply small tolerance if float points do not round properly to zero
			if (abs(min.x - pointOfImpact.x) < approximation) collidingSide = ES_Front;
			if (abs(min.y - pointOfImpact.y) < approximation) collidingSide = ES_Bottom;
			if (abs(min.z - pointOfImpact.z) < approximation) collidingSide = ES_Right;
			if (abs(max.x - pointOfImpact.x) < approximation) collidingSide = ES_Behind;
			if (abs(max.y - pointOfImpact.y) < approximation) collidingSide = ES_Top;
			if (abs(max.z - pointOfImpact.z) < approximation) collidingSide = ES_Left;

			switch (collidingSide)
			{
			case -1:
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
			case ES_Behind:
				normal = Vector3::right; //TODO: Check if these normals are the right direction in world space
			default:
				break;
			}
		}
	}
}