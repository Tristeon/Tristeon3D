#pragma once
#include <vector>
#include "EditorNode.h"


namespace Tristeon
{
	namespace Editor
	{
		//Subject to change
		class EditorNodeTree
		{
		public:
			~EditorNodeTree() {}
			std::vector<std::unique_ptr<EditorNode>> nodes;
			void load(nlohmann::json nodeTree);
			nlohmann::json getData();
			EditorNode* findNodeByInstanceID(std::string nodeInstanceID);
			void createParentalBonds();
			void removeNode(EditorNode* node);
		};
	}
}
