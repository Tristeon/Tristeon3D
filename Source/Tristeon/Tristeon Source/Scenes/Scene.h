#pragma once
#include "Core/TObject.h"
#include <vector>
#include <memory>
#include "Core/GameObject.h"
#include "Editor/Prefab.h"

namespace Tristeon
{
	namespace Scenes
	{
		class Scene final : public Core::TObject
		{
			friend SceneManager;
		public:
			Scene();
			void init();

			void addGameObject(std::unique_ptr<Core::GameObject> gameObj);

			void removeGameObject(Core::GameObject* gameObj);

			Core::GameObject* getGameObject(std::string instanceID);

			nlohmann::json serialize() override;

			void deserialize(nlohmann::json json) override;

		private:
			std::vector<std::unique_ptr<Tristeon::Core::GameObject>> gameObjects;
			static DerivedRegister<Scene> reg;
		};
	}
}
