#pragma once
#include "Hit.h"
#include "RigidbodyConstraints.h"
#include <vector>
#include "Collision.h"
#include "Core/Components/Component.h"

/**
 * Option for how to apply a force using Rigidbody.addForce
 * \param Force				Add a continuous force to the rigidbody, using its mass.
 * \param Acceleration		Add a continuous acceleration to the rigidbody, ignoring its mass.
 * \param Impulse			Add an instant force impulse to the rigidbody, using its mass.
 * \param VelocityChange	Add an instant velocity change to the rigidbody, ignoring its mass.
 */
enum ForceMode { Force, Acceleration, Impulse, VelocityChange };

namespace Tristeon
{
	namespace Physics
	{
		class RigidBody : public Core::Components::Component
		{
			friend class Physics;
			friend Collision;

		public:
			RigidBody();
			~RigidBody();
			RigidBody(Vector3 gravity, float drag, std::string collisionDetection, Vector3 velocity);

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			void start() override;
			void init() override;
			std::vector<Collision> getCollisions(float timeLeft);

			Hit generateHit(BoxCollider* collider) const;
			/**
			* Adds a force to the Rigidbody.
			* \param force Force vector in world coordinates.
			*/
			void addForce(Vector3 force);
			void addForce(float x, float y, float z);
			/**
			 * Adds a force to the Rigidbody.
			 * \param force Force vector in world coordinates.
			 * \param mode 	Type of force to apply.
			 */
			void addForce(Vector3 force, ForceMode mode);

			std::vector<BoxCollider*> getCollidingObjects() const { return collidingObjects; }
			std::vector<BoxCollider*> getPreviousCollidingObjects() const { return previousCollidingObjects; }

			/**
			 * Controls which degrees of freedom are allowed for the simulation of this Rigidbody.
			 */
			RigidbodyConstraints constraints;
			/**
			 * Different collision check modes for optimization
			 */
			std::string collisionDetection;
			/**
			 * The velocity vector of the rigidbody
			 */
			Vector3 velocity;
			/**
			 *  The mass of the rigidbody.
			 */
			float mass;
			Vector3 gravity;
			float drag;
		private:
			void resolveCollision(Collision col);
			void clearCollidingObjects();
			void move(float timeLeft);
			void Reset();
			void update() override;
			std::vector<BoxCollider*> collidingObjects;
			std::vector<BoxCollider*> previousCollidingObjects;

			Vector3 originalPos;
			Vector3 originalVel;
			BoxCollider* collider;
			Vector3 force;

			REGISTER_TYPE_H(RigidBody)
		};
	}
}