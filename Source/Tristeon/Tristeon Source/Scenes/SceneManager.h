#pragma once
#include "Core/Managers/Manager.h"
#include <string>
#include "Scene.h"

namespace Tristeon
{
	namespace Scenes
	{
		class SceneManager final : public Core::Managers::Manager
		{
		public:
			~SceneManager();

			void init() override;

			static void loadScene(int id);
			void loadScene(std::string name);
			static void loadScene(Scene* scene);

			static Scene* getActiveScene() { return activeScene.get(); }
		
		private:
			void reset() override;
			static std::unique_ptr<Scene> activeScene;
		};
	}
}
