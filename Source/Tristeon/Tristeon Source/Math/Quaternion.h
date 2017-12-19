#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <misc/Property.h>
#include "Core/TObject.h"
#include <Editor/TypeRegister.h>
#include "Vector3.h"

namespace Tristeon
{
	namespace Math
	{
		//Forward decl
		class Vector3;

		/**
		 * \brief Quaternion interface. Used for rotation
		 */
		class Quaternion : public Core::TObject
		{
		public:
			/**
			 * \brief Creates an identity quaternion
			 */
			Quaternion();
			/**
			 * \brief Creates a quaternion with the given glm quat
			 * \param glmQuat The glm quat
			 */
			Quaternion(glm::quat glmQuat);
			/**
			 * \brief Creates a quaternion with the given euler angles
			 * \param vector The euler angles described as vector
			 */
			Quaternion(Vector3 vector);
			/**
			 * \brief Creates a quaternion with the given values
			 * \param x The x value of the quaternion
			 * \param y The y value of the quaternion
			 * \param z The z value of the quaternion
			 * \param w The w value of the quaternion
			 */
			Quaternion(float x, float y, float z, float w);

			/**
			 * \brief Gets the quaternion value based on the given index 
			 * \param index The index of the value
			 * \return A float describing the value
			 */
			float operator[](int index) const;
			/**
			 * \brief Checks if this quaternion is NOT equal to the given quaternion
			 * \param other The other quaternion
			 * \return True or false
			 */
			bool operator!=(Quaternion other) const;
			/**
			 * \brief Checks if this quaternion is equal to the given quaternion
			 * \param other The other quaternion
			 * \return True or false
			 */
			bool operator==(Quaternion other) const;
			/**
			 * \brief Multiplies this quaternion with the given quaternion
			 * \param other The other quaternion
			 * \return Returns the result of this multiplication
			 */
			Quaternion operator*(Quaternion other) const;

			/**
			* \brief Multiplies this quaternion with the given quaternion
			* \param other The other quaternion
			*/
			void operator*=(Quaternion other);

			/**
			* \brief The w component of this Quaternion
			*/
			Property(float, w);
			GetPropertyConst(w) { return quaternion.w; }
			SetProperty(w) { quaternion.w = value; }

			/**
			* \brief The x component of this Quaternion
			*/
			Property(float, x);
			GetPropertyConst(x) { return quaternion.x; }
			SetProperty(x) { quaternion.x = value; }

			/**
			* \brief The t component of this Quaternion
			*/
			Property(float, y);
			GetPropertyConst(y) { return quaternion.y; }
			SetProperty(y) { quaternion.y = value; }

			/**
			* \brief The z component of this Quaternion
			*/
			Property(float, z);
			GetPropertyConst(z) { return quaternion.z; }
			SetProperty(z) { quaternion.z = value; }

			/**
			 * \brief Creates a new quaternion based on the given euler angles (degrees)
			 * \param eulerAngles The euler angles
			 * \return Returns the resulting quaternion
			 */
			static Quaternion euler(Vector3 eulerAngles);
			/**
			* \brief Creates a new quaternion based on the given euler angles (degrees)
			* \param x The pitch
			* \param y The yaw
			* \param z The roll
			* \return Returns the resulting quaternion
			*/
			static Quaternion euler(float x, float y, float z);
			/**
			 * \brief Spherically interpolates between start and end by t
			 * \param start The from quaternion
			 * \param end The to quaternion
			 * \param interval The interval [0..1]
			 * \return Returns the slerped quaternion
			 */
			static Quaternion slerp(Quaternion start, Quaternion end, float interval);
			/**
			 * \brief Interpolates between start and end by t
			 * \param start The from quaternion
			 * \param end The to quaterion
			 * \param interval The interval [0..1]
			 * \return Returns the lerped quaternion
			 */
			static Quaternion lerp(Quaternion start, Quaternion end, float interval);
			/**
			 * \brief Creates a quaternion based on a looking direction
			 * \param position The eye position
			 * \param target The target to look at
			 * \return Returns the look quaternion
			 */
			static Quaternion lookRotation(Vector3 position, Vector3 target);
			/**
			 * \brief Inverses the given quaternion
			 * \param quat The quaternion to be inversed
			 * \return Returns the inversed quaternion
			 */
			static Quaternion inverse(Quaternion quat);
			/**
			 * \brief Rotates this quaternion by [amount] around [axis]
			 * \param axis The axis to be rotated around
			 * \param amount The amount of rotation
			 * \return Returns the resulting quaternion
			 */
			Quaternion rotate(Vector3 axis, float amount);
			/**
			 * \brief Makes this quaternion look at something
			 * \param eye The position of this quaternion
			 * \param target The position of the target
			 */
			void lookAt(Vector3 eye, Vector3 target);
			/**
			 * \brief Gets the euler angles derived off of this quaternion
			 * \return The euler angles
			 */
			Vector3 eulerAngles() const;

			/**
			 * \brief Gets the GLM quaternion
			 * \return Returns a glm quaternion
			 */
			glm::quat getGLMQuat() const;

			/**
			 * \brief Serializes this quaternion into a json file
			 * \return Returns the json object
			 */
			nlohmann::json serialize() override;
			/**
			 * \brief Deserializes this quaternion from a json file
			 * \param json The json file
			 */
			void deserialize(nlohmann::json json) override;
		private:
			/**
			 * \brief The glm quaternion
			 */
			glm::quat quaternion;
			
			//Register
			REGISTER_TYPE_H(Quaternion)
		};

		Vector3 operator*(Quaternion quaternion, Vector3 vec);
		Vector3 operator*(Vector3 vec, Quaternion quaternion);
	}
}
