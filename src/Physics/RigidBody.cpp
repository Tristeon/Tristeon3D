#include "RigidBody.h"
#include "BoxCollider.h"
#include <algorithm>
#include "Physics.h"
#include "Core/GameObject.h"
#include "Misc/Hardware/Keyboard.h"

namespace Tristeon
{
	namespace Physics
	{
		RigidBody::RigidBody()
		{
			collisionDetection = "discrete";
			velocity = Vector3::zero;
			mass = 0;
			drag = 1;
			collider = nullptr;
		}

		RigidBody::~RigidBody()
		{
			Physics::instance->remove(this);
		}

		RigidBody::RigidBody(Vector3 gravity, float drag, std::string collisionDetection, Vector3 velocity)
		{
			this->velocity = velocity;
			this->gravity = gravity;
			this->drag = drag;
			this->collisionDetection = collisionDetection;
			mass = 0;
			collider = nullptr;
		}

		void RigidBody::start()
		{
			Physics::instance->rigidBodies.push_back(this);
			collider = getComponent<BoxCollider>();

			if (collider == nullptr)
			{
				Misc::Console::error("Rigidbody without collider found! GameObject: " + gameObject.get()->name);
				return;
			}
			collider->staticObject = false;
			originalPos = transform.get()->position.get();
			originalVel = velocity;
		}

		std::vector<Collision> RigidBody::getCollisions(float timeLeft)
		{
			std::vector<Collision> collisions;
			for (BoxCollider* collider : Physics::instance->colliders)
			{
				if (gameObject.get() == collider->gameObject.get() || !collider->gameObject.get()->isActive()) continue;
				Collision col = Collision(this, collider, velocity*timeLeft);
				if (col.failed)	continue;
				if (collisions.size() > 4)
					Misc::Console::error("Wut");
				collisions.push_back(col);
			}
			sort(collisions.begin(), collisions.end(), &Physics::compareCollisionsByTimeStep);
			return collisions;
		}

		Hit RigidBody::generateHit(BoxCollider* collider) const
		{
			BoxCollider* collider1 = this->collider;
			BoxCollider* collider2 = collider;
			Hit output = Hit(collider1, collider2, velocity);
			return output;
		}

		void RigidBody::addForce(Vector3 force)
		{
			addForce(force, Force);
		}

		void RigidBody::addForce(float x, float y, float z)
		{
			addForce(Vector3(x, y, z));
		}

		void RigidBody::addForce(Vector3 force, ForceMode mode)
		{
			switch (mode)
			{
			case Force:
				velocity += force;
				break;
			case Acceleration:
				break;
			case Impulse:
				break;
			case VelocityChange:
				break;
			}
		}

		void RigidBody::resolveCollision(Collision col)
		{
			if (col.failed) return;
			transform.get()->position.set(col.point + col.normal*0.0001f);

			velocity = velocity - col.normal * velocity.dot(col.normal);
			velocity *= 1 - col.staticCollider->friction;
			//TODO: This movement does not check for collisions!
			this->collider->updateAABB();
			if (col.staticCollider->AABBvsAABB(collider->aabb));
		}

		void RigidBody::clearCollidingObjects()
		{
			previousCollidingObjects = collidingObjects;
			collidingObjects = std::vector<BoxCollider*>();
		}

		void RigidBody::move(float timeLeft)
		{
			//TODO: Implement drag
			velocity = Vector3(velocity.x*drag, velocity.y, velocity.z*drag);
			transform.get()->position.set(transform.get()->position.get() + velocity * timeLeft);
			collider->updateAABB();
		}

		void RigidBody::Reset()
		{
			transform.get()->position.set(originalPos);
			velocity = originalVel;
		}

		void RigidBody::update()
		{
			if (Misc::Keyboard::getKeyDown(Misc::R)) Reset();
		}
	}
}
