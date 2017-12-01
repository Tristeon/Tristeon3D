#include "Vector3.h"
#include "cmath"
#include <string>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Math
	{
		REGISTER_TYPE_CPP(Vector3)

		Vector3::Vector3() : x(0), y(0), z(0) {}

		Vector3::Vector3(float xyz) : x(xyz), y(xyz), z(xyz) {}

		Vector3::Vector3(float x, float y) : x(x), y(y), z(0) {}

		Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		float Vector3::getAxis(const int& axis) const
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
				return 0;
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
			{
				Misc::Console::warning("You cannot normalize Vector3::zero");
				return;
			}

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
			return sqrt(x*x + y*y + z*z);
		}

		float Vector3::getSqrLength() const
		{
			return x*x + y*y + z*z;
		}

		float Vector3::dot(Vector3 a, Vector3 b)
		{
			Vector3 const c = a * b;
			return c.x + c.y + c.z;
		}

		float Vector3::distance(Vector3 a, Vector3 b)
		{
			Vector3 difference = b - a;
			return difference.getLength();
		}

		float Vector3::distance(Vector3 vec) const
		{
			Vector3 difference = vec - Vector3(x, y, z);
			return difference.getLength();
		}

		float Vector3::dot(Vector3 vec) const
		{
			return x*vec.x + y*vec.y + z*vec.z;
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

		Vector3 Vector3::operator*(const Vector3& vec) const
		{
			Vector3 result(x * vec.x, y * vec.y, z*vec.z);
			return result;
		}

		Vector3 Vector3::operator*(const float& multiplier) const
		{
			Vector3 result(x * multiplier, y * multiplier, z*multiplier);
			return result;
		}

		Vector3 Vector3::operator/(const float& divider) const
		{
			return Vector3(x / divider, y / divider, z / divider);
		}

		Vector3 Vector3::operator/(const Vector3& divider) const
		{
			return Vector3(x / divider.x, y / divider.y, z / divider.z);
		}

		Vector3 Vector3::operator+(const Vector3& vec) const
		{
			return Vector3(x + vec.x, y + vec.y, z + vec.z);
		}

		Vector3 Vector3::operator-(const Vector3& vec) const
		{
			return Vector3(x - vec.x, y - vec.y, z - vec.z);
		}

		float Vector3::operator[](const int& value) const
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
			nlohmann::json output;
			output["typeID"] = typeid(Vector3).name();
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
	}
}
