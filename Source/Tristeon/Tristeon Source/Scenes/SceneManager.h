#pragma once
#include "Core/Managers/Manager.h"
#include <string>
#include "Scene.h"
#include "Editor/Asset Browser/SceneFileitem.h"

namespace Tristeon
{
	namespace Scenes
	{
		class SceneManager final : public Core::Managers::Manager
		{
			friend class Tristeon::Editor::SceneFileItem;
		public:
			~SceneManager() = default;

			void init() override;

			static void loadScene(int id);
			static void loadScene(std::string name);
			static void loadScene(Scene* scene);

			static Scene* getActiveScene() { return activeScene.get(); }
		
		private:
			static Core::Transform* findTransformByInstanceID(std::string instanceID);
			static void createParentalBonds(Scene* scene);

			static void addScenePath(std::string name, std::string path);
			static void removeScenePath(std::string name) { sceneFilePaths.erase(name); }

			static std::map<std::string,std::string> sceneFilePaths;

			void reset() override;
			static std::unique_ptr<Scene> activeScene;
		};
	}
}
