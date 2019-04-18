#pragma once
#include "Math/Vector3.h"

namespace Tristeon
{
	namespace Physics
	{
		class BoxCollider;

		class Hit
		{
		public:
			Hit(BoxCollider* a, BoxCollider* b, Vector3 vel);

			/**
			 *  The position of the collider when it collides
			 */
			Vector3 pointOfImpact;
			/**
			 * The normal of the surface the ray hit.
			 */
			Vector3 normal;

			float resultantVectorScale;

			/**
			 * Whether there actually was a hit
			 */
			bool failed;
		};
	}
}