#include "Vector2.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Math
	{
		REGISTER_TYPE_CPP(Vector2)

		Vector2::Vector2() : x(0), y(0) {}

		Vector2::Vector2(float xy) : x(xy), y(xy) {}

		Vector2::Vector2(float x, float y) : x(x), y(y) {}

		float Vector2::getAxis(const int& axis) const
		{
			switch (axis)
			{
			case 0:
				return x;
			case 1:
				return y;
			default:
				return 0;
			}
		}

		Vector2 Vector2::getNormalized() const
		{
			//Normalize a copy of this vector
			Vector2 output(x, y);
			output.normalize();
			return output;
		}

		void Vector2::normalize()
		{
			if (x == 0 && y == 0)
			{
				Misc::Console::warning("You cannot normalize Vector2.zero!");
				return;
			}

			//Normalize
			const float magnitude = getLength();
			x /= magnitude;
			y /= magnitude;
		}

		void Vector2::scale(float multiplier)
		{
			x *= multiplier;
			y *= multiplier;
		}

		float Vector2::getLength() const
		{
			return sqrt(x*x + y*y);
		}

		float Vector2::getSqrLength() const
		{
			return x*x + y*y;
		}

		float Vector2::dot(Vector2 a, Vector2 b)
		{
			return a.x*b.x + a.y*b.y;
		}

		float Vector2::distance(Vector2 a, Vector2 b)
		{
			Vector2 difference = b - a;
			return difference.getLength();
		}

		float Vector2::distance(Vector2 vec) const
		{
			Vector2 difference = vec - Vector2(x, y);
			return difference.getLength();
		}

		float Vector2::dot(Vector2 vec) const
		{
			return x*vec.x + y*vec.y;
		}

		Vector2 Vector2::lerp(Vector2 a, Vector2 b, float t)
		{
			if (b - a == zero()) return b; //A and B are the same vector
			const float interpolation = a.distance(b) * t; //Get interpolation distance
			const Vector2 linearDirection = (b - a).getNormalized(); //Get direction vector
			return linearDirection * interpolation + a; //direction * interpolation + start
		}

		bool Vector2::operator==(const Vector2& vec) const
		{
			return x == vec.x && y == vec.y;
		}

		bool Vector2::operator!=(const Vector2& vec) const
		{
			return x != vec.x || y != vec.y;
		}

		Vector2 Vector2::operator*(const Vector2& vec) const
		{
			return { x * vec.x, y * vec.y };
		}

		Vector2 Vector2::operator*(const float& multiplier) const
		{
			return { x * multiplier, y * multiplier };
		}

		Vector2 Vector2::operator/(const float& divider) const
		{
			return { x / divider, y / divider };
		}

		Vector2 Vector2::operator/(const Vector2& divider) const
		{
			return { x / divider.x, y / divider.y };
		}

		Vector2 Vector2::operator+(const Vector2& vec) const
		{
			return { x + vec.x, y + vec.y };
		}

		Vector2 Vector2::operator-(const Vector2& vec) const
		{
			return { x - vec.x, y - vec.y };
		}

		float Vector2::operator[](const int& value) const
		{
			return getAxis(value);
		}

		void Vector2::operator-=(const Vector2& vector)
		{
			x -= vector.x;
			y -= vector.y;
		}

		void Vector2::operator+=(const Vector2& vector)
		{
			x += vector.x;
			y += vector.y;
		}

		void Vector2::operator*=(const Vector2& vector)
		{
			x *= vector.x;
			y *= vector.y;
		}

		void Vector2::operator*=(const float& value)
		{
			x *= value;
			y *= value;
		}

		std::string Vector2::toString() const
		{
			return "{ " + std::to_string(x) + ", " + std::to_string(y) + " }";
		}

		nlohmann::json Vector2::serialize()
		{
			nlohmann::json j;
			j["typeID"] = typeid(Vector2).name();
			j["x"] = x;
			j["y"] = y;
			return j;
		}

		void Vector2::deserialize(nlohmann::json json)
		{
			x = json["x"];
			y = json["y"];
		}
	}
}
