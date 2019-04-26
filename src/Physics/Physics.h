#pragma once
#include <vector>
#include "Collision.h"
#include "OctTree.h"
#include "Misc/WorkQueue.h"

namespace Tristeon
{
	namespace Physics
	{
		class Ray;

		class Physics
		{
			friend RigidBody;
			friend BoxCollider;
		public:
			static Physics* instance;

			Physics();
			~Physics();

			/**
			 * Call update for physics calculations and collision checks
			 */
			void resetRigidBodies();
			void update();
			bool raycast(Ray ray);
			bool raycast(Ray ray, float raylength);
			bool raycast(Ray ray, Core::GameObject*& obj);
			bool raycast(Ray ray, Core::GameObject*& obj, float rayLength);
			bool raycast(Ray ray, Core::GameObject*& obj, Vector3& hit);
			bool raycast(Ray ray, Vector3& hit, float raylength);
			bool raycast(Ray ray, Vector3& hit, Core::GameObject*& obj, float raylength);

			void addRigidbody(RigidBody* rb);
			void addCollider(BoxCollider* collider);

			std::vector<ColliderData> getCollidersAlongVelocity(RigidBody* rb);

			bool enableTimeStep = false;

			std::unique_ptr<Misc::WorkQueue> workQueue;
		private:
			void reset();

			std::vector<OctNode*> getCollidingPartitions(RigidBody* rb, OctNode* node);

			static bool compareCollisionsByTimeStep(Collision col1, Collision col2);
			void remove(RigidBody* rb);
			void remove(BoxCollider* collider);

			std::unique_ptr<OctTree> octTree;
			std::vector<RigidBody*> rigidBodies;
			std::vector<BoxCollider*> colliders;
		};
	}
}
