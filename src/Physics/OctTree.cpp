#include "OctTree.h"
#include "Core/Rendering/DebugDrawManager.h"

namespace Tristeon
{
	namespace Physics
	{
		OctTree::OctTree(float size) : size(size)
		{
			rootNode = std::make_unique<OctNode>(Vector3::zero, size);
		}

		void OctTree::addCollider(BoxCollider* collider)
		{
			rootNode->addCollider(ColliderData(collider));
		}

		void OctTree::removeCollider(BoxCollider* collider)
		{
			rootNode->removeCollider(collider);
		}

		void OctTree::onGUI()
		{
			drawNode(rootNode.get());
		}

		void OctTree::drawNode(OctNode* node)
		{
			if (!node->isLeaf())
			{
				for (const std::unique_ptr<OctNode>& subNode : node->subNodes)
					drawNode(subNode.get());
			}

			//Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0.5f, 0.5f));

			//if (node->colliders.size() == 0)
			//{
			//	//Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0, 1));
			//}
			//else
			//{
			//	Core::Rendering::DebugDrawManager::addAABB(node->getBoundary(), 2, Misc::Color(0, 0.5f, 0.5f));
			//}
		}
	}
}
