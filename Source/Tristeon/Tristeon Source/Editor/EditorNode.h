#pragma once
#include "Selectable.h"
#include "json.hpp"
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Editor
	{
		class EditorNode : public Selectable
		{
			friend class EditorSelection;
		public:
			explicit EditorNode(std::shared_ptr<Core::GameObject> gameObj);
			~EditorNode() override;

			/**
			 * \brief Applies the json data to the connecetedGameObject
			 */
			void applyChanges();

			/**
			 * \brief Sets the data as the new specified json
			 */
			void load(nlohmann::json json);

			/**
			 * \brief Returns gameobject data in json format
			 */
			nlohmann::json* getData();

			/**
			 * \brief Sets a new parent
			 */
			void move(EditorNode* parent);
			
			/**
			 * \brief Checks if the given node is a child of this one
			 */
			bool hasChild(EditorNode* node);

			/**
			 * \brief Unique typeid of the connectedGameObject
			 */
			std::string typeID;
			EditorNode* parent = nullptr;
			std::vector<EditorNode*> children;
		private:
			/**
			 * \brief The gameobject the editornode is bound to
			 */
			std::shared_ptr<Core::GameObject> connectedGameObject;
			/**
			 * \brief The serialized data of the connectedGameObject
			 */
			nlohmann::json data;
		};
	}
}
