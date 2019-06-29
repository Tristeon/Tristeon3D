#include "PartitionTree.h"
#include "Core/Rendering/DebugDrawManager.h"

namespace Tristeon
{
	namespace Physics
	{
		PartitionTree::PartitionTree(float size)
		{
			float min = -size * 0.5f;
			float max = size * 0.5f;
			rootNode = std::make_unique<TreeNode>(AABB(Vector3(min, min, min), Vector3(max, max, max)));
		}

		void PartitionTree::addCollider(BoxCollider* collider)
		{
			rootNode->addCollider(ColliderData(collider));
		}

		void PartitionTree::removeCollider(BoxCollider* collider)
		{
			rootNode->removeCollider(collider);
		}

		void PartitionTree::onGUI()
		{
			drawNode(rootNode.get());
		}

		void PartitionTree::drawNode(TreeNode* node)
		{
			if (!node->isLeaf())
			{
				for (const std::unique_ptr<TreeNode>& subNode : node->subNodes)
					drawNode(subNode.get());
			}
			Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0.5f, 0.5f));
		}
	}
}
