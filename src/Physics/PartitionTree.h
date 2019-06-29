#pragma once
#include "TreeNode.h"
#include "BoxCollider.h"
#include "Hit.h"


namespace Tristeon
{
	namespace Physics
	{
		class PartitionTree
		{
		public:
			PartitionTree(float size);

			void addCollider(BoxCollider* collider);
			void removeCollider(BoxCollider* collider);
			void onGUI();

			std::unique_ptr<TreeNode> rootNode = nullptr;

		private:
			void drawNode(TreeNode* node);

			Vector3 position;
		};
	}
}