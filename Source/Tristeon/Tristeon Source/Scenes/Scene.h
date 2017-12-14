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
			friend SceneManager;
		public:
			Scene();
			~Scene() override;
			void init();

			void addGameObject(Core::GameObject* gameObj);

			void removeGameObject(Core::GameObject* gameObj);

			Core::GameObject* getGameObject(std::string instanceID);

			nlohmann::json serialize() override;

			void deserialize(nlohmann::json json) override;

		private:
			//Scene(const Scene& scene) {} //The class should not be allowed to be copied
			std::vector<std::unique_ptr<Tristeon::Core::GameObject>> gameObjects;
			static DerivedRegister<Scene> reg;
		};
	}
}