#include "TreeNode.h"
#include "BoxCollider.h"

namespace Tristeon
{
	namespace Physics
	{
		TreeNode::TreeNode() : boundary(AABB())
		{
		}

		TreeNode::TreeNode(AABB const& boundary) : boundary(boundary)
		{

		}

		void TreeNode::addCollider(ColliderData data, bool subdivideEnabled)
		{
			TreeNode* foundNode = nullptr;
			for (const std::unique_ptr<TreeNode>& node : subNodes)
			{
				AABB const nodeAABB = node->getBoundary();

				//Continue if the collider's AABB does not fit within the node
				if (!AABB::AABBvsPoint(nodeAABB, data.aabb.min) || !AABB::AABBvsPoint(nodeAABB, data.aabb.max))
					continue;

				foundNode = node.get();
				break;
			}

			if (foundNode == nullptr) appendCollider(data, subdivideEnabled);
			else foundNode->addCollider(data, subdivideEnabled);
		}

		void TreeNode::removeCollider(BoxCollider* collider)
		{
			std::vector<ColliderData>::iterator const itr = find_if(colliders.begin(), colliders.end(),
				[&collider](const ColliderData& entry) { return entry.collider == collider; });

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

		AABB TreeNode::getBoundary() const
		{
			return boundary;
		}

		void TreeNode::subDivideOctree()
		{
			Vector3 delta = boundary.max - boundary.min;
			Vector3 position = boundary.getPosition();

			subNodes = std::vector<std::unique_ptr<TreeNode>>(8);
			for (int i = 0; i < 8; ++i)
			{
				Vector3 newPos = position;

				if (i & 4)
					newPos.x += delta.x * 0.25f;
				else
					newPos.x -= delta.x * 0.25f;

				if (i & 2)
					newPos.y += delta.y * 0.25f;
				else
					newPos.y -= delta.y * 0.25f;

				if (i & 1)
					newPos.z += delta.z * 0.25f;
				else
					newPos.z -= delta.z * 0.25f;

				subNodes[i] = std::make_unique<TreeNode>(AABB(newPos - delta * 0.25f, newPos + delta * 0.25f));
				subNodes[i]->parent = this;
			}
		}

		void TreeNode::attemptSubdivideKD()
		{
			//Tuple axisvalue, rating
			auto x = getKDAxis(0);
			auto y = getKDAxis(1);
			auto z = getKDAxis(2);

			if (x.rating <= 0 && y.rating <= 0 && z.rating <= 0)
				return;

			KDAxis result;
			if (x.rating > y.rating && x.rating > z.rating)
				result = x;
			else if (y.rating > x.rating && y.rating > z.rating)
				result = y;
			else if (z.rating > 0)
				result = z;

			subNodes = std::vector<std::unique_ptr<TreeNode>>(2);
			subNodes[0] = std::make_unique<TreeNode>(result.subA);
			subNodes[1] = std::make_unique<TreeNode>(result.subB);
			subNodes[0]->parent = this;
			subNodes[1]->parent = this;
		}

		TreeNode::KDAxis TreeNode::getKDAxis(int axis)
		{
			//Get median
			std::sort(colliders.begin(), colliders.end(), [&](ColliderData const& a, ColliderData const& b) { return a.aabb.min.getAxis(axis) < b.aabb.min.getAxis(axis); });
			bool isEven = colliders.size() % 2 == 0;

			float median = 0;
			if (isEven)
			{
				unsigned int indexA = colliders.size() / 2;
				unsigned int indexB = colliders.size() / 2 - 1;

				float axisA = (colliders.begin() + indexA)->aabb.min.getAxis(axis);
				float axisB = (colliders.begin() + indexB)->aabb.min.getAxis(axis);

				median = (axisA + axisB) / 2.0f;
			}
			else
			{
				int index = floor(colliders.size() / 2.0f);
				median = (colliders.begin() + index)->aabb.min.getAxis(axis);
			}

			//Make subdivisions
			AABB boundary = getBoundary();

			Vector3 splitA = boundary.max;
			if (axis == 0) splitA.x = median;
			if (axis == 1) splitA.y = median;
			if (axis == 2) splitA.z = median;

			Vector3 splitB = boundary.min;
			if (axis == 0) splitB.x = median;
			if (axis == 1) splitB.y = median;
			if (axis == 2) splitB.z = median;

			AABB subA, subB;
			subA = AABB(boundary.min, splitA);
			subB = AABB(splitB, boundary.max);

			//Rate subdivision quality
			int fit = 0;
			int fitACount = 0, fitBCount = 0;
			for (ColliderData const& collider : colliders)
			{
				bool fitsA = AABB::AABBvsPoint(subA, collider.aabb.min) && AABB::AABBvsPoint(subA, collider.aabb.max);
				bool fitsB = AABB::AABBvsPoint(subB, collider.aabb.min) && AABB::AABBvsPoint(subB, collider.aabb.max);

				if (fitsA || fitsB)
					fit++;

				if (fitsA)
					fitACount++;
				if (fitsB)
					fitBCount++;
			}

			if (fit == 0)
				return { -1, subA, subB };

			if (fitACount == colliders.size() || fitBCount == colliders.size())
				return { -1, subA, subB };

			float rating = (float)fit / colliders.size();
			return { rating, subA, subB };
		}

		bool TreeNode::isLeaf() const
		{
			return subNodes.empty();
		}

		bool TreeNode::isRoot() const
		{
			return parent == nullptr;
		}

		int TreeNode::getIndexOfPosition(Vector3 position) const
		{
			int index = 0;

			Vector3 pos = boundary.getPosition();

			index |= position.y > pos.y ? 4 : 0;
			index |= position.x > pos.x ? 2 : 0;
			index |= position.z > pos.z ? 1 : 0;

			return index;
		}

		void TreeNode::appendCollider(ColliderData data, bool subdivideEnabled)
		{
			colliders.push_back(data);
			colliderCount++;

			//Subdivide if needed
			if (subNodes.empty())
			{
				if (colliders.size() > 100 && subdivideEnabled)
				{
					//subDivide();
					attemptSubdivideKD();

					//Re-Add colliders
					std::vector<ColliderData> temp = std::vector<ColliderData>(this->colliders);
					this->colliders.clear();
					colliderCount = 0;
					for (ColliderData const& d : temp)
						addCollider(d, !subNodes.empty());
				}
			}
		}
	}
}