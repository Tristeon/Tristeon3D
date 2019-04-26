﻿#pragma once
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

		class OctNode
		{
		public:
			OctNode();
			OctNode(Vector3 position, float size);
			void addCollider(ColliderData data);
			void removeCollider(BoxCollider* data);

			std::vector<ColliderData> colliders;

			AABB getBoundary() const;
			void subDivide();
			bool isLeaf() const;
			bool isRoot() const;
			int getIndexOfPosition(Vector3 position) const;

			std::vector<std::unique_ptr<OctNode>> subNodes;
			OctNode* parent = nullptr;
		private:
			void appendCollider(ColliderData collider);
			Vector3 position;
			float size;
			int colliderCount = 0;
		};
	}
}