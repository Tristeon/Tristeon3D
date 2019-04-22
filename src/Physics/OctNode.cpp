#include "OctNode.h"
#include "BoxCollider.h"

namespace Tristeon
{
	namespace Physics
	{
		OctNode::OctNode() : size(-1)
		{
		}

		OctNode::OctNode(Vector3 position, float size) : position(position), size(size)
		{

		}

		void OctNode::addCollider(BoxCollider* collider)
		{
			OctNode* foundNode = nullptr;
			for (const std::unique_ptr<OctNode>& node : subNodes)
			{
				AABB const nodeAABB = node->getBoundary();
				
				//Continue if the collider's AABB does not fit within the node
				if (!AABB::AABBvsPoint(nodeAABB, collider->getAABB().min) || !AABB::AABBvsPoint(nodeAABB, collider->getAABB().max)) 
					continue; 
				foundNode = node.get();
				break;
			}

			if (foundNode == nullptr) appendCollider(collider);
			else foundNode->addCollider(collider);
		}

		void OctNode::removeCollider(BoxCollider* collider)
		{
			std::vector<BoxCollider*>::iterator const itr = find(colliders.begin(), colliders.end(), collider);
			if (itr != std::end(colliders))
			{
				colliders.erase(itr);
				colliderCount--;
			}
			else
			{
				for (int i = 0; i < subNodes.size(); ++i)
					subNodes[i]->removeCollider(collider);
			}
		}

		AABB OctNode::getBoundary() const
		{
			AABB output;
			output.min = position - Vector3(size, size, size) / 2.0f;
			output.max = position + Vector3(size, size, size) / 2.0f;
			return output;
		}

		void OctNode::subDivide()
		{
			subNodes = std::vector<std::unique_ptr<OctNode>>(8);
			for (int i = 0; i < 8; ++i)
			{
				Vector3 newPos = position;

				if (i & 4)
					newPos.x += size * 0.25f;
				else
					newPos.x -= size * 0.25f;

				if (i & 2)
					newPos.y += size * 0.25f;
				else
					newPos.y -= size * 0.25f;

				if (i & 1)
					newPos.z += size * 0.25f;
				else
					newPos.z -= size * 0.25f;

				subNodes[i] = std::make_unique<OctNode>(newPos, size * 0.5f);
				subNodes[i]->parent = this;
			}
		}

		bool OctNode::isLeaf() const
		{
			return subNodes.size() == 0;
		}

		bool OctNode::isRoot() const
		{
			return parent == nullptr;
		}

		int OctNode::getIndexOfPosition(Vector3 position) const
		{
			int index = 0;

			index |= position.y > this->position.y ? 4 : 0;
			index |= position.x > this->position.x ? 2 : 0;
			index |= position.z > this->position.z ? 1 : 0;

			return index;
		}

		void OctNode::appendCollider(BoxCollider* collider)
		{
			colliders.push_back(collider);
			colliderCount++;

			//Subdivide if needed
			if (subNodes.size() == 0)
			{
				if (colliders.size() > 2)
				{
					//Clear and cache current colliders
					std::vector<BoxCollider*> colliders = std::vector<BoxCollider*>(this->colliders);
					this->colliders = {};
					colliderCount = 0;

					//Subdivide this node
					subDivide();

					//Add colliders
					for (BoxCollider* col : colliders)
						addCollider(col);
				}
			}
		}
	}
}