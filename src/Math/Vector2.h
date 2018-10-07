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
		 * Vector2 interface, describes a 2D point or movement
		 */
		class Vector2 : public Core::TObject
		{
		public:
			/**
			 * Creates a (xy, xy) Vector2
			 * \param xy The x and y of this vector
			 */
			Vector2(float xy);
			/**
			 * Creates a (x, y) Vector2
			 * \param x The x of this vector
			 * \param y The y of this vector
			 */
			Vector2(float x = 0, float y = 0);

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

			float x;
			float y;

			/**
			* Access the x, y, z components using [0], [1], [2] respectively.
			*/
			float getAxis(const int& axis) const;
			/**
			* returns normalized vector
			*/
			Vector2 getNormalized() const;
			/**
			* Normalizes the vector into an unit vector
			*/
			void normalize();
			/**
			* Scales vector by the multiplier
			* \param multiplier the amount it multiplies by
			*/
			void scale(float multiplier);
			/**
			Returns the magnitude of the vector
			*/
			float getLength() const;
			/**
			* Returns the magnitude of the vector squared (It's more optimized than normal getLength)
			*/
			float getSqrLength() const;
			/**
			*Returns the distance between the two given vectors
			*/
			static float distance(Vector2 vec, Vector2 vec2);
			/**
			* Dot product of the two given vectors
			*/
			static float dot(Vector2 vec, Vector2 vec2);
			/**
			* Returns the distance between the vector and the given vector
			*/
			float distance(Vector2 vec) const;
			/**
			* Dot product between the vector and the given vector
			*/
			float dot(Vector2 vec) const;
			/**
			* Linearly interpolates between two vectors.
			* \param a The start point
			* \param b The destination
			* \param t interpolate rate (0-1)
			*/
			static Vector2 lerp(Vector2 a, Vector2 b, float t);

			/**
			* Gets the axis with the given index
			*/
			float operator[](const int& value) const;

			bool operator==(const Vector2& vec) const;
			bool operator!=(const Vector2& vec) const;
			Vector2 operator*(const float& multiplier) const;
			Vector2 operator*(const Vector2& vec) const;
			Vector2 operator/(const float& divider) const;
			Vector2 operator/(const Vector2& divider) const;
			Vector2 operator+(const Vector2& vec) const;
			Vector2 operator-(const Vector2& vec) const;
			void operator-=(const Vector2& vector);
			void operator+=(const Vector2& vector);
			void operator *=(const Vector2& vector);
			void operator *=(const float& value);

			/**
			* Convert this instance to a string describing the properties
			*/
			std::string toString() const;

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
		private:
			REGISTER_TYPE_H(Vector2)
		};
	}
}