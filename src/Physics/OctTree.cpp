#include "OctTree.h"
#include "Physics.h"
#include "Core/Rendering/DebugDrawManager.h"

namespace Tristeon
{
	namespace Physics
	{
		OctTree::OctTree(float size, int depth) : size(size), depth(depth)
		{
			rootNode = std::make_unique<OctNode>(Vector3::zero, size);
			rootNode->subDivide(depth - 1);
		}

		void OctTree::addCollider(BoxCollider* collider)
		{
			rootNode->addCollider(collider);
		}

		void OctTree::removeCollider(BoxCollider* collider)
		{
			rootNode->removeCollider(collider);
		}

		void OctTree::onGUI()
		{
			drawNode(rootNode.get(), depth);
		}

		void OctTree::drawNode(OctNode* node, int depth)
		{
			if (!node->isLeaf())
			{
				for (const std::unique_ptr<OctNode>& subNode : node->subNodes)
					drawNode(subNode.get(), depth - 1);
			}

			if (node->colliders.size() == 0)
			{
				//Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0, 1));
			}
			else
			{
				Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0.5f, 0.5f));
			}
		}
	}
}
