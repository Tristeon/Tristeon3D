#pragma once
#include "Core/TObject.h"
#include <vector>
#include <memory>
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Scenes
	{
		class Scene final : public Core::TObject
		{
			friend Core::GameObject;
		public:
			Scene();
			~Scene() override;
			void init();

			std::vector<std::shared_ptr<Core::GameObject>> gameObjects;

			nlohmann::json serialize() override;

			void deserialize(nlohmann::json json) override;

			//The counter that keeps track of the amount of instances that have been created
			int instanceCount;
		private:
			REGISTER_TYPE_H(Scene)
		};
	}
}