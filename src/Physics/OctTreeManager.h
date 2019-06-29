#pragma once
#include "PartitionTree.h"

namespace Tristeon
{
	namespace Physics
	{
		class OctTreeManager : public Core::Components::Component
		{
		public:
			void start() override;
			void update() override;
			void spawnObject(bool addRigidbody);

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			int initialSpawnedObjects = 10000;
			int maxColliderSize = 0;
			int seed = 0;
			int moveSpeed;

		private:
			REGISTER_TYPE_H(OctTreeManager)
		};
	}
}
