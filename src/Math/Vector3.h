#pragma once
#include "Core/TObject.h"
#include <Editor/TypeRegister.h>

namespace Tristeon
{
	inline namespace Math
	{
#define Vec_Convert3(vec) { vec.x, vec.y, vec.z }
#define Vec_Ptr_Convert3(vec) { vec->x, vec->y, vec->z }

		/**
		* Vector3 interface, describes a 3D point or movement and implements math operations to modify said point/movement.
		*/
		struct Vector3 final : public Core::TObject
		{
		public:
			/**
			 * Creates a (xyz, xyz, xyz) Vector3
			 * \param xyz The x, y and z component of this vector
			 */
			explicit Vector3(float xyz);
			/**
			 * Creates a (x, y, z) Vector3
			 */
			Vector3(float x = 0, float y = 0, float z = 0);

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

			float x;
			float y;
			float z;

			/**
			* Access the x, y, z components using [0], [1], [2] respectively.
			*
			* \exception invalid_argument If the axis is higher than the dimensions supported by Vector3, or if it's below 0.
			*/
			float getAxis(const int& axis) const;
			/**
			* Returns a unit (length = 1) vector with the same direction as this vector.
			*/
			Vector3 getNormalized() const;
			/**
			* Normalizes the vector into a unit vector
			*/
			void normalize();
			/**
			* Scales vector by the multiplier
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
			*Returns the distance between two vectors
			*/
			static float distance(Vector3 a, Vector3 b);
			/**
			* Dot product of the two given vectors
			*/
			static float dot(Vector3 a, Vector3 b);
			/**
			 * Cross product of the two given vectors
			 */
			static Vector3 cross(Vector3 a, Vector3 b)
			{
				Vector3 output;
				output.x = a.y * b.z - b.y * a.z;
				output.y = a.z * b.x - b.z * a.x;
				output.z = a.x * b.y - b.x * a.y;
				return output;
			}
			/**
			* Returns the distance to the given vector
			*/
			float distance(Vector3 vec) const;
			/**
			* Dot product between the vector and the given vector
			*/
			float dot(Vector3 vec) const;
			/**
			 * Cross product between the vector and the given vector
			 */
			void cross(Vector3 vec);

			/**
			* Linearly interpolates between two vectors.
			* \param a The start point
			* \param b The destination
			* \param t interpolate rate (0-1)
			*/
			static Vector3 lerp(Vector3 a, Vector3 b, float t);

			/**
			* Access the x, y, z components using [0], [1], [2] respectively.
			 */
			float operator[](int value) const;
			bool operator==(const Vector3& vec) const;
			bool operator!=(const Vector3& vec) const;
			Vector3 operator*(float multiplier) const;
			Vector3 operator*(const Vector3& vec) const;
			Vector3 operator/(const float& divider) const;
			Vector3 operator/(const Vector3& divider) const;
			Vector3 operator+(const Vector3& vec) const;
			Vector3 operator-(const Vector3& vec) const;
			Vector3 operator-() const;
			void operator-=(const Vector3& vector);
			void operator+=(const Vector3& vector);
			void operator *=(const Vector3& vector);
			void operator *=(const float& value);

			/**
			 * Convert this instance to a string describing the properties
			 */
			std::string toString() const;

			nlohmann::json serialize() override;
			nlohmann::json serialize_const() const;
			void deserialize(nlohmann::json json) override;

			std::array<float, 3> toArray() const { return { x, y, z }; }

		private:
			REGISTER_TYPE_H(Vector3)
		};

		/**
		* Multiplies the x,y,z components with the given multiplier
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