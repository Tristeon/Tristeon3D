#pragma once
#ifdef TRISTEON_EDITOR

#include "Editor/EditorNodeTree.h"
#include "Editor/EditorWindow.h"
#include "Scenes/Scene.h"

namespace Tristeon
{
	namespace Editor
	{
		class GameObjectHierarchy : public EditorWindow
		{
		public:
			GameObjectHierarchy();
			~GameObjectHierarchy();

			EditorNodeTree editorNodeTree;

			/**
			 * \brief Manages events of the gameobject hierarchy and also loads all the gameobjects
			 * in the current scene including all their parental relations
			 */
			void drawNode(EditorNode* node);

			/**
			 * \brief Loads the scene into the editor
			 */
			void loadScene(Scenes::Scene& scene);

			/**
			* \brief onGUI is called for rendering and handling GUI events.
			*		Currently onGUI is called per-frame however that might be subject to change
			*/
			void onGui() override;
		private:
			/**
			 * \brief Will load update the current scene if a new scene has been loaded
			 */
			void checkSceneChanges();

			void handleDroppedItems();

			void createGameObject(std::string name);
			void createGameObject(Core::GameObject* gameObject);

			void handleInput();

			void changeNodeName();

			char createdGameObjectName[255] = "";
			bool sceneChanged = false;
			Scenes::Scene* currentScene = nullptr;
			EditorNode* selectedNode = nullptr;
			EditorNode* hoveredNode = nullptr;
		};
	}
}

#endif