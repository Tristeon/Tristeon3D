#include "Quaternion.h"
#include "Vector3.h"
#include "Misc/Console.h"
#include <glm/gtc/matrix_transform.inl>

namespace Tristeon
{
	namespace Math
	{
		REGISTER_TYPE_CPP(Quaternion)

		Quaternion::Quaternion()
		{
			quaternion = glm::quat();
		}

		Quaternion::Quaternion(glm::quat glmQuat)
		{
			quaternion = glmQuat;
		}

		Quaternion::Quaternion(Vector3 vector)
		{
			quaternion = glm::quat(glm::radians(glm::vec3(Vec_Convert3(vector))));
		}

		Quaternion::Quaternion(float x, float y, float z, float w)
		{
			quaternion = glm::quat(x, y, z, w);
		}

		float Quaternion::operator[](int index) const
		{
			Misc::Console::t_assert(index <= 3, "Quaternion [] operator tried to access a value higher than 3");
			return quaternion[index];
		}

		bool Quaternion::operator!=(Quaternion other) const
		{
			return !(*this == other);
		}

		bool Quaternion::operator==(Quaternion other) const
		{
			return
				x == other.x &&
				y == other.y &&
				z == other.z &&
				w == other.w;
		}

		Quaternion Quaternion::operator*(Quaternion other) const
		{
			return Quaternion(quaternion * other.quaternion);
		}

		void Quaternion::operator*=(Quaternion other)
		{
			quaternion *= other.quaternion;
		}

		Quaternion Quaternion::euler(Vector3 angles)
		{
			return Quaternion(glm::quat(radians(glm::vec3(Vec_Convert3(angles)))));
		}

		Quaternion Quaternion::euler(float x, float y, float z)
		{
			return euler({ x, y, z });
		}

		Quaternion Quaternion::slerp(Quaternion start, Quaternion end, float interval)
		{
			return Quaternion(glm::slerp(start.quaternion, end.quaternion, interval));
		}

		Quaternion Quaternion::lerp(Quaternion start, Quaternion end, float interval)
		{
			return Quaternion(glm::lerp(start.quaternion, end.quaternion, interval));
		}

		Quaternion Quaternion::lookRotation(Vector3 position, Vector3 target)
		{
			return Quaternion(glm::lookAt(Vec_Convert3(position), Vec_Convert3(target), glm::vec3(0, 1, 0)));
		}

		Quaternion Quaternion::inverse(Quaternion quat)
		{
			return Quaternion(glm::inverse(quat.quaternion));
		}

		Quaternion Quaternion::rotate(Vector3 axis, float amount)
		{
			quaternion = glm::rotate(quaternion, glm::radians(amount), Vec_Convert3(axis));
			return *this;
		}

		void Quaternion::lookAt(Vector3 eye, Vector3 target)
		{
			quaternion = glm::quat(glm::lookAt(Vec_Convert3(eye), Vec_Convert3(target), glm::vec3(0, 1, 0)));
		}

		Vector3 Quaternion::eulerAngles() const
		{
			return Vec_Convert3(degrees(glm::eulerAngles(quaternion)));
		}

		glm::quat Quaternion::getGLMQuat() const
		{
			return quaternion;
		}

		nlohmann::json Quaternion::serialize()
		{
			nlohmann::json j;
			j["typeID"] = typeid(Quaternion).name();
			j["x"] = x;
			j["y"] = y;
			j["z"] = z;
			j["w"] = w;
			return j;
		}

		void Quaternion::deserialize(nlohmann::json json)
		{
			x = json["x"];
			y = json["y"];
			z = json["z"];
			w = json["w"];
		}

		Vector3 operator*(Quaternion quaternion, Vector3 vec)
		{
			const glm::vec3 output = quaternion.getGLMQuat() * glm::vec3(vec.x, vec.y, vec.z);
			return Vector3(output.x, output.y, output.z);
		}

		Vector3 operator*(Vector3 vec, Quaternion quaternion)
		{
			return quaternion * vec;
		}
	}
}
