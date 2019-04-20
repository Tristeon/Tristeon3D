#include "Vector3.h"
#include "cmath"
#include <string>
#include "XPlatform/typename.h"

namespace Tristeon
{
	namespace Math
	{
		REGISTER_TYPE_CPP(Vector3)

		Vector3::Vector3(float xyz) : x(xyz), y(xyz), z(xyz) {}

		Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		const Vector3 Vector3::forward = Vector3(0, 0, 1);
		const Vector3 Vector3::back = Vector3(0, 0, -1);
		const Vector3 Vector3::right = Vector3(1, 0, 0);
		const Vector3 Vector3::left = Vector3(-1, 0, 0);
		const Vector3 Vector3::up = Vector3(0, 1, 0);
		const Vector3 Vector3::down = Vector3(0, -1, 0);
		const Vector3 Vector3::one = Vector3(1, 1, 1);
		const Vector3 Vector3::zero = Vector3(0, 0, 0);

		float& Vector3::getAxis(const int& axis)
		{
			switch (axis)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw std::invalid_argument("Axis is out of range, using vector3's index 2 is the maximum");
			}
		}

		Vector3 Vector3::getNormalized() const
		{
			//Normalize a copy of our vector
			Vector3 output(x, y, z);
			output.normalize();
			return output;
		}

		void Vector3::normalize()
		{
			if (x == 0 && y == 0 && z == 0)
				return;

			//Normalize
			float const magnitude = getLength();
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}

		void Vector3::scale(float multiplier)
		{
			x *= multiplier;
			y *= multiplier;
			z *= multiplier;
		}

		float Vector3::getLength() const
		{
			return sqrt(x*x + y * y + z * z);
		}

		float Vector3::getSqrLength() const
		{
			return x * x + y * y + z * z;
		}

		float Vector3::dot(Vector3 a, Vector3 b)
		{
			Vector3 const c = a * b;
			return c.x + c.y + c.z;
		}

		float Vector3::distance(Vector3 a, Vector3 b)
		{
			return (b - a).getLength();
		}

		float Vector3::distance(Vector3 vec) const
		{
			return (vec - Vector3(x, y, z)).getLength();
		}

		float Vector3::dot(Vector3 vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		Vector3 Vector3::lerp(Vector3 a, Vector3 b, float t)
		{
			if (b - a == Vector3(0, 0, 0)) return b; //Positions are equal

			float const interpolation = a.distance(b) * t; //Get interpolation value
			Vector3 const linearDirection = (b - a).getNormalized(); //Get direction
			return linearDirection * interpolation + a; //Pos = direction * distance + start
		}

		bool Vector3::operator==(const Vector3& vec) const
		{
			return x == vec.x && y == vec.y && z == vec.z;
		}

		bool Vector3::operator!=(const Vector3& vec) const
		{
			return x != vec.x || y != vec.y || z != vec.z;
		}

		Vector3 Vector3::operator*(const float& multiplier) const
		{
			return { x * multiplier, y * multiplier, z * multiplier };
		}

		Vector3 Vector3::operator*(const Vector3& vec) const
		{
			return { x * vec.x, y * vec.y, z * vec.z };
		}

		Vector3 Vector3::operator/(const float& divider) const
		{
			return { x / divider, y / divider, z / divider };
		}

		Vector3 Vector3::operator/(const Vector3& divider) const
		{
			return { x / divider.x, y / divider.y, z / divider.z };
		}

		Vector3 Vector3::operator+(const Vector3& vec) const
		{
			return { x + vec.x, y + vec.y, z + vec.z };
		}

		Vector3 Vector3::operator-(const Vector3& vec) const
		{
			return { x - vec.x, y - vec.y, z - vec.z };
		}

		float& Vector3::operator[](const int& value)
		{
			return getAxis(value);
		}

		void Vector3::operator-=(const Vector3& vector)
		{
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
		}

		void Vector3::operator+=(const Vector3& vector)
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
		}

		void Vector3::operator*=(const Vector3& vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
		}

		void Vector3::operator*=(const float& value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		std::string Vector3::toString() const
		{
			return "{ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " }";
		}

		nlohmann::json Vector3::serialize()
		{
			return serialize_const();
		}

		nlohmann::json Vector3::serialize_const() const
		{
			nlohmann::json output;
			output["typeID"] = TRISTEON_TYPENAME(Vector3);
			output["x"] = x;
			output["y"] = y;
			output["z"] = z;
			return output;
		}

		void Vector3::deserialize(nlohmann::json json)
		{
			x = json["x"];
			y = json["y"];
			z = json["z"];
		}

		Vector3 operator*(const float& multiplier, Vector3 vector)
		{
			return vector * multiplier;
		}
	}
}
