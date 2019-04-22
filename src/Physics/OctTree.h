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
			OctTree(float size);

			void addCollider(BoxCollider* collider);
			void removeCollider(BoxCollider* collider);
			void onGUI();

			std::unique_ptr<OctNode> rootNode = nullptr;

		private:
			void drawNode(OctNode* node);

			Vector3 position;
			float size;
		};
	}
}