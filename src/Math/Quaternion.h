#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <misc/Property.h>
#include "Core/TObject.h"
#include <Core/TypeRegister.h>
#include "Vector3.h"

namespace Tristeon
{
	namespace Math
	{
		class Vector3;

		/**
		 *  Quaternion dsescribes a 3D rotation. This way of describing 3D rotation prevents issues that are experienced with a euler approach like gimbal lock.
		 *
		 *  Unless if you are well experienced with Quaternions, it is recommended to use the functionality provided through class and static methods,
		 *  rather than modifying the components of the quaternion directly.
		 */
		class Quaternion : public Core::TObject
		{
		public:
			/**
			 * Creates an identity quaternion
			 */
			Quaternion();
			/**
			 * Creates a quaternion from the given glm quat
			 */
			Quaternion(glm::quat glmQuat);
			/**
			 * Creates a quaternion from the given euler angles
			 * \param vector The euler angles described as vector
			 */
			Quaternion(Vector3 vector);
			/**
			 * Creates a quaternion with the given values
			 */
			Quaternion(float x, float y, float z, float w);

			/**
			 * Gets the quaternion axis based on the given index
			 */
			float operator[](int index) const;
			bool operator!=(Quaternion other) const;
			bool operator==(Quaternion other) const;
			Quaternion operator*(Quaternion other) const;
			Quaternion operator=(Quaternion other);
			void operator*=(Quaternion other);

			PropertyNestedValue(Quaternion, w, float, quaternion.w);
			PropertyNestedValue(Quaternion, x, float, quaternion.x);
			PropertyNestedValue(Quaternion, y, float, quaternion.y);
			PropertyNestedValue(Quaternion, z, float, quaternion.z);

			/**
			 * Creates a new quaternion based on the given euler angles (degrees)
			 */
			static Quaternion euler(Vector3 eulerAngles);
			/**
			* Creates a new quaternion based on the given euler angles (degrees)
			* \param x The pitch
			* \param y The yaw
			* \param z The roll
			*/
			static Quaternion euler(float x, float y, float z);
			/**
			 * Spherically interpolates between start and end by t
			 * \param start The from quaternion
			 * \param end The to quaternion
			 * \param interval The interval [0..1]
			 */
			static Quaternion slerp(Quaternion start, Quaternion end, float interval);
			/**
			 * Interpolates between start and end by t
			 * \param start The from quaternion
			 * \param end The to quaterion
			 * \param interval The interval [0..1]
			 */
			static Quaternion lerp(Quaternion start, Quaternion end, float interval);
			/**
			 * Creates a quaternion based on a looking direction
			 * \param position The eye position
			 * \param target The target to look at
			 */
			static Quaternion lookRotation(Vector3 position, Vector3 target);
			/**
			 * Inverses the given quaternion
			 */
			static Quaternion inverse(Quaternion quat);
			/**
			 * Rotates this quaternion by [amount] around [axis]
			 */
			Quaternion rotate(Vector3 axis, float amount);
			/**
			 * Makes this quaternion look at something
			 * \param eye The position of this quaternion
			 * \param target The position of the target
			 */
			void lookAt(Vector3 eye, Vector3 target);
			/**
			 * Gets the euler angles derived off of this quaternion
			 */
			Vector3 eulerAngles() const;
			/**
			 * Gets the GLM quaternion
			 */
			glm::quat getGLMQuat() const;

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
		private:
			glm::quat quaternion;
			REGISTER_TYPE_H(Quaternion)
		};

		Vector3 operator*(Quaternion quaternion, Vector3 vec);
		Vector3 operator*(Vector3 vec, Quaternion quaternion);
	}
}