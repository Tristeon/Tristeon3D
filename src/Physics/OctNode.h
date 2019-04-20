#pragma once
#include "AABB.h"

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

		class OctNode
		{
		public:
			OctNode();
			OctNode(Vector3 position, float size);
			void addCollider(BoxCollider* collider);
			void removeCollider(BoxCollider* collider);

			std::vector<BoxCollider*> colliders;

			AABB getBoundary() const;
			void subDivide(int depth = 0);
			bool isLeaf() const;
			bool isRoot() const;
			int getIndexOfPosition(Vector3 position) const;

			std::vector<std::unique_ptr<OctNode>> subNodes;
			OctNode* parent = nullptr;
		private:
			Vector3 position;
			float size;
		};
	}
}