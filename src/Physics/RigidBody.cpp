#include "RigidBody.h"
#include "BoxCollider.h"
#include <algorithm>
#include "Physics.h"
#include "Core/GameObject.h"
#include "Misc/Hardware/Keyboard.h"
#include "Core/Rendering/DebugDrawManager.h"
#include <chrono>
#include <thread>

using namespace std;

namespace Tristeon
{
	namespace Physics
	{
		REGISTER_TYPE_CPP(RigidBody)

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
			if (Physics::instance != nullptr)
				Physics::instance->remove(this);
		}

		RigidBody::RigidBody(Vector3 gravity, float drag, string collisionDetection, Vector3 velocity)
		{
			this->velocity = velocity;
			this->gravity = gravity;
			this->drag = drag;
			this->collisionDetection = collisionDetection;
			mass = 0;
			collider = nullptr;
		}

		nlohmann::json RigidBody::serialize()
		{
			nlohmann::json j;
			j["typeID"] = TRISTEON_TYPENAME(RigidBody);
			j["name"] = "RigidBody";
			j["gravity"] = gravity;
			j["drag"] = drag;
			j["mass"] = mass;
			j["collisionDetection"] = collisionDetection;
			return j;
		}

		void RigidBody::deserialize(nlohmann::json json)
		{
			gravity = json["gravity"];
			drag = json["drag"];
			mass = json["mass"];
			string const collisionDetectionString = json["collisionDetection"];
			collisionDetection = collisionDetectionString;
		}

		void RigidBody::start()
		{
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

		void RigidBody::init()
		{
			Component::init();
			Physics::instance->addRigidbody(this);
		}

		std::vector<Collision> RigidBody::getCollisions(float timeLeft)
		{
			static float totalTime = 0;
			static float totalColliders = 0;
			static int iterations = 0;
			iterations++;

			auto start = chrono::high_resolution_clock::now();

			std::vector<ColliderData> colliders = Physics::instance->getCollidersAlongVelocity(this);
			int const range = static_cast<int>(colliders.size()) / Physics::instance->workQueue->count_threads();
			const Vector3 velLeft = velocity * timeLeft;

			std::vector<Collision> collisions;
			std::vector<unique_ptr<CollisionThreadData>> dataList;
			mutex collision_lock;

			for (int i = 0; i < Physics::instance->workQueue->count_threads(); i++)
			{
				unique_ptr<CollisionThreadData> data = std::make_unique<CollisionThreadData>();
				data->colliders = &colliders;
				data->index = i;
				data->range = range;
				data->velLeft = velLeft;
				data->collisions = &collisions;
				data->collision_lock = &collision_lock;

				//Pass job to work queue
				Physics::instance->workQueue->add_job([&](void* pData)
				{
					CollisionThreadData* d = reinterpret_cast<CollisionThreadData*>(pData);
					calculateCollisions(d->colliders, d->index, d->range, d->velLeft, d->collisions, d->collision_lock);
				}, data.get());

				dataList.push_back(std::move(data));
			}

			//Finish threads
			Physics::instance->workQueue->finish_jobs();

			//Sort by time step; The first collision the rb encounters goes first
			sort(collisions.begin(), collisions.end(), &Physics::compareCollisionsByTimeStep);

			auto end = chrono::high_resolution_clock::now();
			chrono::duration<double> duration = end - start;
			totalTime += duration.count();
			totalColliders += colliders.size();
			
			Misc::Console::write("Collisions found: " + to_string(collisions.size()));
			string debug;
			debug += "Collision check: ";
			debug += "Average time take: " + to_string(totalTime / iterations * 1000.0f) + " ms\n";
			debug += "Average considered colliders: " + to_string(totalColliders / iterations) + " colliders\n";
			Misc::Console::write(debug);
			//Misc::Console::t_assert(totalTime < 30, "Time's up! Average time: " + std::to_string(totalTime / iterations));
			return collisions;
		}

		void RigidBody::calculateCollisions(std::vector<ColliderData>* colliderData, int index, int range, Vector3 velLeft, std::vector<Collision>* collisions, mutex* collision_lock)
		{
			auto start = colliderData->begin() + range * index;
			auto end = start + range;
			if (index == Physics::instance->workQueue->count_threads() - 1)
				end = colliderData->end();

			std::vector<Collision> cols;
			for (; start < end; ++start)
			{
				ColliderData const data = *start;
				if (gameObject == data.gameObject) continue;
				
				Collision col = Collision(this, data.aabb, velLeft);
				if (col.failed)	continue;
				col.staticCollider = data.collider;
				
				cols.push_back(col);
			}

			lock_guard<mutex> col_lock(*collision_lock);
			collisions->insert(collisions->begin(), cols.begin(), cols.end());
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

			velocity = velocity - col.normal * velocity.dot(col.normal) * (1 + col.staticCollider->bounciness);
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
