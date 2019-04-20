#pragma once
#include "OctNode.h"
#include "BoxCollider.h"
#include "Hit.h"


namespace Tristeon
{
	namespace Physics
	{
		class OctTree
		{
		public:
			OctTree(float size, int depth);

			void addCollider(BoxCollider* collider);
			void removeCollider(BoxCollider* collider);
			void onGUI();
			int getDepth() const { return depth; }

			std::unique_ptr<OctNode> rootNode = nullptr;

		private:
			void drawNode(OctNode* node, int depth);

			Vector3 position;
			float size;
			int depth;
		};
	}
}