#pragma once
#include "AABB.h"
#include "BoxCollider.h"

namespace Tristeon
{
	namespace Physics
	{
		//Binary values map to xyz
		enum OctTreeIndex
		{
			BottomLeftFront = 0, //000,
			BottomRightFront = 2, //010,
			BottomRightBack = 3, //011,
			BottomLeftBack = 1, //001,
			TopLeftFront = 4,	//100,
			TopRightFront = 6,	//110,
			TopRightBack = 7,	//111,
			TopLeftBack = 5,	//101,
		};

		class BoxCollider;

		class TreeNode
		{
		public:
			TreeNode();
			TreeNode(AABB const& boundary);
			void addCollider(ColliderData data, bool subdivideEnabled = true);
			void removeCollider(BoxCollider* data);

			std::vector<ColliderData> colliders;

			AABB getBoundary() const;
			void subDivideOctree();
			void attemptSubdivideKD();
			struct KDAxis
			{
				float rating = 0;

				AABB subA = {};
				AABB subB = {};
				KDAxis() = default;
				KDAxis(float const& rating, AABB const& subA, AABB const& subB) : rating(rating), subA(subA), subB(subB) { }
			};

			KDAxis getKDAxis(int axis);
			bool isLeaf() const;
			bool isRoot() const;
			int getIndexOfPosition(Vector3 position) const;

			std::vector<std::unique_ptr<TreeNode>> subNodes;
			TreeNode* parent = nullptr;
		private:
			void appendCollider(ColliderData collider, bool subdivideEnabled = true);
			AABB boundary;
			int colliderCount = 0;
		};
	}
}
