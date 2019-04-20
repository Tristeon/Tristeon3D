#include "OctTreeManager.h"
#include "Physics.h"
#include "Misc/Hardware/Keyboard.h"
#include "Core/Transform.h"
#include "Scenes/SceneManager.h"
#include "RigidBody.h"

namespace Tristeon
{
	namespace Physics
	{
		REGISTER_TYPE_CPP(OctTreeManager)

		void OctTreeManager::start()
		{
			srand(seed);

			for (int i = 0; i < initialSpawnedObjects; i++)
			{
				spawnObject(false);
			}
		}

		void OctTreeManager::update()
		{
			if (Misc::Keyboard::getKeyDown(Misc::NUM_1))
				spawnObject(false);
			if (Misc::Keyboard::getKeyDown(Misc::NUM_2))
				spawnObject(true);
		}

		void OctTreeManager::spawnObject(bool addRigidbody)
		{
			int size = 90;
			if (maxColliderSize == 0)
				maxColliderSize = 1;
			std::unique_ptr<Core::GameObject> gameObject = std::make_unique<Core::GameObject>();
			Vector3 const randomSize = Vector3(rand() % maxColliderSize + 1, rand() % maxColliderSize + 1, rand() % maxColliderSize + 1);
			Vector3 randomPos = Vector3(rand() % int(size), rand() % int(size), rand() % int(size));
			randomPos.x -= size / 2.0f;
			randomPos.y -= size / 2.0f;
			randomPos.z -= size / 2.0f;
			BoxCollider* collider = gameObject->addComponent<BoxCollider>();
			collider->init();
			collider->size = randomSize;
			collider->transform.get()->position = randomPos;
			gameObject->transform.get()->position = randomPos;

			if (addRigidbody)
			{
				Vector3 randomDirection = Vector3(rand(), rand(), rand());
				RigidBody* rb = gameObject.get()->addComponent<RigidBody>();
				rb->velocity = randomDirection.getNormalized()*moveSpeed;
				rb->gravity = Vector3(rand(), -0.1f, rand()).getNormalized();
				rb->init();
			}

			Scenes::SceneManager::getActiveScene()->addGameObject(move(gameObject));
		}

		nlohmann::json OctTreeManager::serialize()
		{
			nlohmann::json j;
			j["typeID"] = TRISTEON_TYPENAME(OctTreeManager);
			j["name"] = "OctTreeManager";
			j["seed"] = seed;
			j["moveSpeed"] = moveSpeed;
			j["maxColliderSize"] = maxColliderSize;
			j["initialSpawnedObjects"] = initialSpawnedObjects;
			return j;
		}

		void OctTreeManager::deserialize(nlohmann::json json)
		{
			moveSpeed = json["moveSpeed"];
			seed = json["seed"];
			maxColliderSize = json["maxColliderSize"];
			initialSpawnedObjects = json["initialSpawnedObjects"];
		}
	}
}
