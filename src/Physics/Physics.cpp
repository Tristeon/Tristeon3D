#include "Physics.h"
#include "AABB.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Ray.h"
#include "Collision.h"
#include "Misc/Hardware/Keyboard.h"
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Physics
	{
		Physics* Physics::instance;

		Physics::Physics()
		{
			rigidBodies = std::vector<RigidBody*>();
			colliders = std::vector<BoxCollider*>();
			instance = this;
		}

		void Physics::resetRigidBodies()
		{
			for (int i = 0; i < rigidBodies.size(); i++)
			{
				rigidBodies[i]->Reset();
			}
		}

		void Physics::update()
		{
			if (Misc::Keyboard::getKeyDown(Misc::T)) enableTimeStep = !enableTimeStep;
			if (enableTimeStep && !Misc::Keyboard::getKeyDown(Misc::F)) return;

			for (RigidBody* rigidBody : rigidBodies)
			{
				if (rigidBody->gameObject.get()->isActive())
					rigidBody->velocity += rigidBody->gravity;
			}

			for (RigidBody* rb : rigidBodies)
			{
				float timeLeft = 1;
				if (!rb->gameObject.get()->isActive()) continue;
				if (rb->collider == nullptr) continue;

				int iterations = 0;
				while (timeLeft > 0)
				{
					std::vector<Collision> collisions = rb->getCollisions(timeLeft); //sweep check

					bool collision = false;
					for (Collision col : collisions)
					{
						if (col.staticCollider->isTrigger)
						{
							rb->collider->onTriggerEnter.invoke(col);
							col.staticCollider->onTriggerEnter.invoke(col);
						}
						else
						{
							rb->resolveCollision(col);
							rb->collider->onCollisionEnter.invoke(col);
							col.staticCollider->onCollisionEnter.invoke(col);
							timeLeft -= col.timeStep;
							Misc::Console::t_assert(timeLeft >= 0, "Physics error: Incorrect timeLeft: " + std::to_string(timeLeft));
							collision = true;
							break;
						}
					}

					if (!collision)
					{
						rb->move(timeLeft);
						break;
					}
					if (iterations > 100)
					{
						Misc::Console::error("Physics error: More than 100 iterations have been gone through, timeLeft: " + std::to_string(timeLeft));
						break;
					}
					iterations++;
				}
			}
		}

		bool Physics::raycast(Ray ray)
		{
			Vector3 intersection;
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				if (ray.lineAABBIntersection(colliders[i]->aabb, intersection, 100000))
					return true;
			}
			return false;
		}

		bool Physics::raycast(Ray ray, float raylength)
		{
			Vector3 intersection;
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				if (ray.lineAABBIntersection(colliders[i]->aabb, intersection, raylength))
					return true;
			}
			return false;
		}

		bool Physics::raycast(Ray ray, Core::GameObject *& obj)
		{
			Vector3 intersection;
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				if (ray.lineAABBIntersection(colliders[i]->aabb, intersection, 100000))
				{
					obj = colliders[i]->gameObject;
					return true;
				}
			}
			return false;
		}

		bool Physics::raycast(Ray ray, Core::GameObject*& obj, float rayLength)
		{
			Vector3 intersection;
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				if (ray.lineAABBIntersection(colliders[i]->aabb, intersection, rayLength))
				{
					obj = colliders[i]->gameObject;
					return true;
				}
			}
			return false;
		}

		//TODO: Fix sorting for all the other raycasts
		bool Physics::raycast(Ray ray, Core::GameObject*& obj, Vector3& hit)
		{
			bool output = false;
			float currentDistance = 100000;
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				float f_low;
				Vector3 hitPoint;
				if (ray.lineAABBIntersection(colliders[i]->aabb, hitPoint, 100000, f_low))
				{
					if (f_low < currentDistance)
					{
						obj = colliders[i]->gameObject;
						output = true;
						hit = hitPoint;
						currentDistance = f_low;
					}
				}
			}

			return output;
		}

		bool Physics::raycast(Ray ray, Vector3& hit, float raylength)
		{
			for (int i = 0; i < colliders.size(); ++i)
			{
				if (colliders[i]->ignoreRaycast) continue;
				if (ray.lineAABBIntersection(colliders[i]->aabb, hit, raylength))
					return true;
			}
			return false;
		}

		bool Physics::compareCollisionsByTimeStep(Collision col1, Collision col2)
		{
			return col1.timeStep < col2.timeStep;
		}

		void Physics::remove(RigidBody* rb)
		{
			vector<RigidBody*>::iterator const itr = find(rigidBodies.begin(), rigidBodies.end(), rb);
			if (itr != end(rigidBodies))
				rigidBodies.erase(itr);
		}

		void Physics::remove(BoxCollider* collider)
		{
			vector<BoxCollider*>::iterator const itr = find(colliders.begin(), colliders.end(), collider);
			if (itr != end(colliders))
				colliders.erase(itr);
		}
	}
}
