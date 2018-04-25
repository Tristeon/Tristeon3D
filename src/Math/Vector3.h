#pragma once
#include "Core/TObject.h"
#include <Editor/TypeRegister.h>

namespace Tristeon
{
	namespace Math
	{
		#define Vec_Convert3(vec) { vec.x, vec.y, vec.z }
		#define Vec_Ptr_Convert3(vec) { vec->x, vec->y, vec->z }

		/**
		* \brief Vector3 interface, describes a 3D point or movement
		*/
		class Vector3 final : public Core::TObject
		{
		public:
			/**
			 * \brief Creates a (0, 0, 0) Vector3
			 */
			Vector3();
			/**
			 * \brief Creates a (xyz, xyz, xyz) Vector3
			 * \param xyz The x, y and z component of this vector
			 */
			explicit Vector3(float xyz);
			/**
			 * \brief Creates a (x, y, 0) Vector3
			 * \param x The x component of this vector
			 * \param y The y comopnent of this vector
			 */
			Vector3(float x, float y);
			/**
			 * \brief Creates a (x, y, z) Vector3
			 * \param x The x component of this vector
			 * \param y The y component of this vector
			 * \param z The z component of this vector
			 */
			Vector3(float x, float y, float z);

			#pragma region const static vectors
			/**
			(0, 0, -1)
			*/
			const static Vector3 back;
			/**
			(0, -1, 0)
			*/
			const static Vector3 down;
			/**
			(0, 0, 1)
			*/
			const static Vector3 forward;
			/**
			(-1, 0, 0)
			*/
			const static Vector3 left;
			/**
			(1, 0, 0)
			*/
			const static Vector3 right;
			/**
			(0, 1, 0)
			*/
			const static Vector3 up;
			/**
			(1, 1, 1)
			*/
			const static Vector3 one;
			const static Vector3 zero;
			#pragma endregion


			/**
			 * \brief The x component of this vector
			 */
			float x;
			/**
			 * \brief The y component of this vector
			 */
			float y;
			/**
			 * \brief The z component of this vector
			 */
			float z;

			/**
			* \brief Access the x, y, z components using [0], [1], [2] respectively.
			*/
			float& getAxis(const int& axis);
			/**
			* \brief returns normalized vector
			*/
			Vector3 getNormalized() const;
			/**
			* \brief Normalizes the vector into an unit vector
			*/
			void normalize();
			/**
			* \brief Scales vector by the multiplier
			* \param multiplier the amount it multiplies by
			*/
			void scale(float multiplier);
			/**
			Returns the magnitude of the vector
			*/
			float getLength() const;
			/**
			* \brief Returns the magnitude of the vector squared (It's more optimized than normal getLength)
			*/
			float getSqrLength() const;
			/**
			*\brief Returns the distance between the two given vectors
			*/
			static float distance(Vector3 vec, Vector3 vec2);
			/**
			* \brief Dot product of the two given vectors
			*/
			static float dot(Vector3 vec, Vector3 vec2);
			/**
			* \brief Returns the distance between the vector and the given vector
			* \param vec the point to compare with
			*/
			float distance(Vector3 vec) const;
			/**
			* \brief Dot product between the vector and the given vector
			* \param vec the vector to project onto
			*/
			float dot(Vector3 vec) const;
			/**
			* \brief Linearly interpolates between two vectors.
			* \param a The start point
			* \param b The destination
			* \param t interpolate rate (0-1)
			*/
			static Vector3 lerp(Vector3 a, Vector3 b, float t);

			/**
			 * \brief Verifies wether or not this vector is equal with the given vector.
			 * \param vec The vector to compare this vector to.
			 * \return True if this vector is equal to vec.
			 */
			bool operator==(const Vector3& vec) const;

			/**
			 * \brief Verifies wether or not this vector is NOT equal with the given vector
			 * \param vec 
			 * \return 
			 */
			bool operator!=(const Vector3& vec) const;

			/**
			 * \brief Multiplies the x,y,z components with the given multiplier
			 * \param multiplier the value to multiply our components with
			 * \return 
			 */
			Vector3 operator*(const float& multiplier) const;

			/**
			 * \brief Multiplies the x,y,z components with the x,y,z components of the other vector
			 * \param vec the vector to multiply our components with
			 * \return 
			 */
			Vector3 operator*(const Vector3& vec) const;

			/**
			 * \brief Divides the x,y,z components by the given divider
			 * \param divider the value to divide our components with
			 * \return 
			 */
			Vector3 operator/(const float& divider) const;

			/**
			 * \brief Divides the x,y,z components by the x,y,z components of the other vector
			 * \param divider the vector to divide our components with
			 * \return 
			 */
			Vector3 operator/(const Vector3& divider) const;

			/**
			 * \brief Adds the x,y,z components of the given vector to our x,y,z components
			 * \param vec the other vector
			 * \return 
			 */
			Vector3 operator+(const Vector3& vec) const;
			
			/**
			 * \brief Subtracts the x,y,z components of the given vector from our x,y,z components
			 * \param vec the other vector
			 * \return 
			 */
			Vector3 operator-(const Vector3& vec) const;

			/**
			 * \brief Gets the axis with the given index
			 * \param value The index of the axis
			 * \return 
			 */
			float& operator[](const int& value);

			/**
			 * \brief Subtracts the x,y,z components of the given vector from our x,y,z components
			 * \param vector the other vector
			 */
			void operator-=(const Vector3& vector);

			/**
			 * \brief Adds the x,y,z components of the given vector to our x,y,z components
			 * \param vector the other vector
			 */
			void operator+=(const Vector3& vector);

			/**
			 * \brief Multiplies the x,y,z components with the x,y,z components of the other vector
			 * \param vector the vector to multiply our components with
			 */
			void operator *=(const Vector3& vector);

			/**
			 * \brief Multiplies the x,y,z components with the given multiplier
			 * \param value the value to multiply our components with
			 */
			void operator *=(const float& value);

			/**
			 * \brief Convert this instance to a string describing the properties
			 * \return Returns the string
			 */
			std::string toString() const;
			
			/**
			 * \brief Serializes the vector into a json file
			 * \return The json object describing this vector
			 */
			nlohmann::json serialize() override;
			nlohmann::json serialize_const() const;

			std::array<float, 3> toArray() const { return { x, y, z }; }
			/**
			 * \brief Deserializes the vector from a json file
			 * \param json The json file
			 */
			void deserialize(nlohmann::json json) override;

		private:
			REGISTER_TYPE_H(Vector3)
		};

		/**
		* \brief Multiplies the x,y,z components with the given multiplier
		* \param multiplier the value to multiply our components with
		* \return
		*/
		Vector3 operator*(const float& multiplier, Vector3 vector);

		//Override functions for json maps/vectors
		inline void to_json(nlohmann::json& j, const Vector3& p) {
			j = p.serialize_const();
		}

		inline void from_json(const nlohmann::json& j, Vector3& p) {
			p.deserialize(j);
		}
	}
}