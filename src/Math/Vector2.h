#pragma once
#include <string>
#include "Core/TObject.h"
#include "Editor/TypeRegister.h"

namespace Tristeon
{
	namespace Math
	{
		#define Vec_Convert2(vec) { vec.x, vec.y }
		#define Vec_Ptr_Convert2(vec) { vec->x, vec->y}

		/**
		 * \brief Vector2 interface, describes a 2D point or movement
		 */
		class Vector2 : public Core::TObject
		{
		public:
			/**
			 * \brief Creates a (0, 0) Vector2
			 */
			Vector2();
			/**
			 * \brief Creates a (xy, xy) Vector2
			 * \param xy The x and y of this vector
			 */
			explicit Vector2(float xy);
			/**
			 * \brief Creates a (x, y) Vector2
			 * \param x The x of this vector
			 * \param y The y of this vector
			 */
			Vector2(float x, float y);

#pragma region quick vectors

			/**
			* (0, -1)
			*/
			static Vector2 down() { return Vector2(0, -1); }
			/**
			* (0, 1)
			*/
			static Vector2 up() { return Vector2(0, 1); }
			/**
			* (1, 0)
			*/
			static Vector2 right() { return Vector2(1, 0); }

			/**
			* (-1, 0)
			*/
			static Vector2 left() { return Vector2(-1, 0); }

			/**
			* (0, 0)
			*/
			static Vector2 zero() { return Vector2(0, 0); }
			/**
			* (1, 1)
			*/
			static Vector2 one() { return Vector2(1, 1); }
#pragma endregion

			/**
			 * \brief The x value of this vector
			 */
			float x;
			/**
			 * \brief The y value of this vector
			 */
			float y;

			/**
			* \brief Access the x, y, z components using [0], [1], [2] respectively.
			*/
			float getAxis(const int& axis) const;
			/**
			* \brief returns normalized vector
			*/
			Vector2 getNormalized() const;
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
			static float distance(Vector2 vec, Vector2 vec2);
			/**
			* \brief Dot product of the two given vectors
			*/
			static float dot(Vector2 vec, Vector2 vec2);
			/**
			* \brief Returns the distance between the vector and the given vector
			* \param vec the point to compare with
			*/
			float distance(Vector2 vec) const;
			/**
			* \brief Dot product between the vector and the given vector
			* \param vec the vector to project onto
			*/
			float dot(Vector2 vec) const;
			/**
			* \brief Linearly interpolates between two vectors.
			* \param a The start point
			* \param b The destination
			* \param t interpolate rate (0-1)
			*/
			static Vector2 lerp(Vector2 a, Vector2 b, float t);

			/**
			* \brief Verifies wether or not this vector is equal with the given vector.
			* \param vec The vector to compare this vector to.
			* \return True if this vector is equal to vec.
			*/
			bool operator==(const Vector2& vec) const;

			/**
			* \brief Verifies wether or not this vector is NOT equal with the given vector
			* \param vec
			* \return
			*/
			bool operator!=(const Vector2& vec) const;

			/**
			* \brief Multiplies the x,y,z components with the given multiplier
			* \param multiplier the value to multiply our components with
			* \return
			*/
			Vector2 operator*(const float& multiplier) const;

			/**
			* \brief Multiplies the x,y,z components with the x,y,z components of the other vector
			* \param vec the vector to multiply our components with
			* \return
			*/
			Vector2 operator*(const Vector2& vec) const;

			/**
			* \brief Divides the x,y,z components by the given divider
			* \param divider the value to divide our components with
			* \return
			*/
			Vector2 operator/(const float& divider) const;

			/**
			* \brief Divides the x,y,z components by the x,y,z components of the other vector
			* \param divider the vector to divide our components with
			* \return
			*/
			Vector2 operator/(const Vector2& divider) const;

			/**
			* \brief Adds the x,y,z components of the given vector to our x,y,z components
			* \param vec the other vector
			* \return
			*/
			Vector2 operator+(const Vector2& vec) const;

			/**
			* \brief Subtracts the x,y,z components of the given vector from our x,y,z components
			* \param vec the other vector
			* \return
			*/
			Vector2 operator-(const Vector2& vec) const;

			/**
			* \brief Gets the axis with the given index
			* \param value The index of the axis
			* \return
			*/
			float operator[](const int& value) const;

			/**
			* \brief Subtracts the x,y,z components of the given vector from our x,y,z components
			* \param vector the other vector
			*/
			void operator-=(const Vector2& vector);

			/**
			* \brief Adds the x,y,z components of the given vector to our x,y,z components
			* \param vector the other vector
			*/
			void operator+=(const Vector2& vector);

			/**
			* \brief Multiplies the x,y,z components with the x,y,z components of the other vector
			* \param vector the vector to multiply our components with
			*/
			void operator *=(const Vector2& vector);

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
			 * \brief Serializes this Vector2 into a json file
			 * \return Returns the json object containing our data
			 */
			nlohmann::json serialize() override;
			/**
			 * \brief Deserializes this Vector2 from a json file
			 * \param json The json file
			 */
			void deserialize(nlohmann::json json) override;
		private:
			REGISTER_TYPE_H(Vector2)
		};
	}
}