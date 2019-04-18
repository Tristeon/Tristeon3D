#pragma once
#include "AABB.h"
#include <vector>
#include "Core/Components/Component.h"
#include "Math/Vector3.h"
#include "Misc/Delegate.h"
#include "Collision.h"

namespace Tristeon
{
	namespace Physics
	{
		enum EdgeSide { ES_Top, ES_Right, ES_Bottom, ES_Left, ES_Front, ES_Behind };

		class BoxCollider : public Core::Components::Component
		{
			friend class RigidBody;
			friend class Physics;

		public:
			/**
			 *  Create default box collider of size 1
			 */
			explicit BoxCollider();
			explicit BoxCollider(Core::GameObject* obj);
			BoxCollider(const Vector3& center, const Vector3& size);
			BoxCollider(const Vector3& center, const Vector3& size, const float& restitution);
			virtual ~BoxCollider();
			
			void start() override;
			void update() override;

			/**
			 *  Returns true if the point is inside of the collider
			 */
			bool pointvsAABB(const Vector3& point) const;
			/**
			*  Checks if another box collider is colliding with this one
			*/
			bool AABBvsAABB(const AABB& a) const;
			/**
			*  Checks if another box collider is colliding with this one
			*/
			bool intersect(const BoxCollider& collider) const;

			Vector3 getPosition() const;
			/**
			 *  Returns all the points the box consists of
			 */
			std::vector<Vector3> getExtremes() const;

			/**
			*  The minimum and maximum of the collider
			*/
			AABB aabb;
			/**
			 *  The center of the box, measured in the object's local space.
			 */
			Vector3 center;
			/**
			 *  The size of the box, measured in the object's local space.
			 */
			Vector3 size;

			/**
			 *  Is it a trigger?
			 */
			bool isTrigger;

			bool staticObject = true;

			bool ignoreRaycast = false;
			/**
			 *  Bounciness of the object
			 */
			float bounciness;
			/**
			 *  Friction of the object (Proportion it moves along the aligned axis as it reflects)
			 */
			float friction;

			Misc::Delegate<Collision> onTriggerEnter;
			Misc::Delegate<Collision> onTriggerStay;
			Misc::Delegate<Collision> onTriggerExit;
			Misc::Delegate<Collision> onCollisionEnter;
			Misc::Delegate<Collision> onCollisionStay;
			Misc::Delegate<Collision> onCollisionExit;
		private:
			/**
			* Update AABB positions
			*/
			void updateAABB();
			void onDrawGizmos() const;
		};
	}
}
