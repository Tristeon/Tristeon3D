#include "DebugDrawManager.h"
#include <cmath>
#include "Physics/AABB.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			DebugDrawManager* DebugDrawManager::instance = nullptr;

			void DebugDrawManager::addLine(const Vector3& from, const Vector3& to, float width, const Misc::Color& color)
			{
				if (instance == nullptr)
					return;

				instance->drawList[color][width].push(DebugMesh(std::vector<Data::Vertex>{ Data::Vertex(from), Data::Vertex(to)}, width, color));
			}

			void DebugDrawManager::addCube(const Vector3& min, const Vector3& max, float lineWidth, const Misc::Color& color)
			{
				if (instance == nullptr)
					return;

				//Rect coords 1
				Vector3 const bl1 = min;
				Vector3 const tl1 = Vector3(min.x, max.y, min.z);
				Vector3 const br1 = Vector3(max.x, min.y, min.z);
				Vector3 const tr1 = Vector3(max.x, max.y, min.z);

				//Rect coords 2
				Vector3 const bl2 = Vector3(min.x, min.y, max.z);
				Vector3 const tl2 = Vector3(min.x, max.y, max.z);
				Vector3 const br2 = Vector3(max.x, min.y, max.z);
				Vector3 const tr2 = max;

				//Rect 1
				DebugMesh mesh = DebugMesh({}, lineWidth, color);
				mesh.vertices.emplace_back(bl1);
				mesh.vertices.emplace_back(tl1);

				mesh.vertices.emplace_back(tl1);
				mesh.vertices.emplace_back(tr1);

				mesh.vertices.emplace_back(tr1);
				mesh.vertices.emplace_back(br1);

				mesh.vertices.emplace_back(br1);
				mesh.vertices.emplace_back(bl1);

				//Rect 2
				mesh.vertices.emplace_back(bl2);
				mesh.vertices.emplace_back(tl2);

				mesh.vertices.emplace_back(tl2);
				mesh.vertices.emplace_back(tr2);

				mesh.vertices.emplace_back(tr2);
				mesh.vertices.emplace_back(br2);

				mesh.vertices.emplace_back(br2);
				mesh.vertices.emplace_back(bl2);

				//Connecting corners
				mesh.vertices.emplace_back(bl1);
				mesh.vertices.emplace_back(bl2);

				mesh.vertices.emplace_back(tl1);
				mesh.vertices.emplace_back(tl2);

				mesh.vertices.emplace_back(tr1);
				mesh.vertices.emplace_back(tr2);

				mesh.vertices.emplace_back(br1);
				mesh.vertices.emplace_back(br2);

				instance->drawList[color][lineWidth].push(mesh);
			}

			void DebugDrawManager::addCube(const Vector3& center, const Vector3& size, const Vector3& rotation,
				float lineWidth, const Misc::Color& color)
			{
				if (instance == nullptr)
					return;

				if (size == Vector3::zero)
				{
					Misc::Console::warning("Trying to draw debug cube with size 0");
					return;
				}

				glm::mat4x4 rotationMatrix = glm::mat4x4(glm::quat(glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z))));

				//TODO Implement Matrix4x4 and stop using glm types here
				Vector3 min = center - size / 2;
				Vector3 max = center + size / 2;

				glm::vec4 glmMin = glm::vec4(min.x, min.y, min.z, 1);
				glm::vec4 glmMax = glm::vec4(max.x, max.y, max.z, 1);

				glm::vec4 point = glm::vec4(size.x/2, size.y/2, size.z/2, 1);

				//Rect coords 1
				glm::vec3 const bl1 = rotationMatrix * (point * glm::vec4(-1, -1, -1, 1));
				glm::vec3 const tl1 = rotationMatrix * (point * glm::vec4(-1, 1, -1, 1));
				glm::vec3 const br1 = rotationMatrix * (point * glm::vec4(1, -1, -1, 1));
				glm::vec3 const tr1 = rotationMatrix * (point * glm::vec4(1, 1, -1, 1))
					;
				//Rect coords 2
				glm::vec3 const bl2 = rotationMatrix * (point * glm::vec4(-1, -1, 1, 1));
				glm::vec3 const tl2 = rotationMatrix * (point * glm::vec4(-1, 1, 1, 1));
				glm::vec3 const br2 = rotationMatrix * (point * glm::vec4(1, -1, 1, 1));
				glm::vec3 const tr2 = rotationMatrix * point;

				//Rect 1
				DebugMesh mesh = DebugMesh({}, lineWidth, color);
				mesh.vertices.emplace_back(bl1);
				mesh.vertices.emplace_back(tl1);

				mesh.vertices.emplace_back(tl1);
				mesh.vertices.emplace_back(tr1);

				mesh.vertices.emplace_back(tr1);
				mesh.vertices.emplace_back(br1);

				mesh.vertices.emplace_back(br1);
				mesh.vertices.emplace_back(bl1);

				//Rect 2
				mesh.vertices.emplace_back(bl2);
				mesh.vertices.emplace_back(tl2);

				mesh.vertices.emplace_back(tl2);
				mesh.vertices.emplace_back(tr2);

				mesh.vertices.emplace_back(tr2);
				mesh.vertices.emplace_back(br2);

				mesh.vertices.emplace_back(br2);
				mesh.vertices.emplace_back(bl2);

				//Connecting corners
				mesh.vertices.emplace_back(bl1);
				mesh.vertices.emplace_back(bl2);

				mesh.vertices.emplace_back(tl1);
				mesh.vertices.emplace_back(tl2);

				mesh.vertices.emplace_back(tr1);
				mesh.vertices.emplace_back(tr2);

				mesh.vertices.emplace_back(br1);
				mesh.vertices.emplace_back(br2);

				for (auto& vertice : mesh.vertices)
				{
					vertice.pos += glm::vec3(center.x, center.y, center.z);
				}

				instance->drawList[color][lineWidth].push(mesh);
			}

			void DebugDrawManager::addSphere(const Vector3& center, float r, float lineWidth, const Misc::Color& color, int circles, int resolution)
			{
				if (instance == nullptr)
					return;

				float const PI = 3.14159265f;
				std::vector<Vector3> positions;

				DebugMesh mesh = DebugMesh({}, lineWidth, color);

				//Horizontal lines
				for (float j = 0; j < PI; j += PI / (circles + 1))
				{
					//Store first pos to make sure that we'll finish the circle
					Vector3 firstPos;
					bool setFirstPos = false;

					float const y = r * cos(j);
					for (float i = 0; i < 2 * PI; i += PI / resolution)
					{
						float const x = r * cos(i) * sin(j);
						float const z = r * sin(i) * sin(j);
						if (!setFirstPos)
						{
							setFirstPos = true;
							firstPos = center + Vector3(x, y, z);
						}
						positions.push_back(center + Vector3(x, y, z));
					}
					//Finish circle
					positions.push_back(firstPos);

					//Apply
					for (int i = 0; i < positions.size(); i += 1)
					{
						Vector3 const start = positions[i];
						Vector3 const end = (i + 1) >= positions.size() ? positions[0] : positions[i + 1];
						mesh.vertices.emplace_back(start);
						mesh.vertices.emplace_back(end);
					}
					positions.clear();
				}

				//Vertical lines
				for (float j = 0; j < PI; j += PI / circles)
				{
					for (float i = 0; i < 2 * PI; i += PI / resolution)
					{
						float x = r * sin(i)*cos(j);
						float y = r * cos(i);
						float z = r * sin(j)*sin(i);
						positions.push_back(center + Vector3(x, y, z));
					}
				}

				//Apply
				for (int i = 0; i < positions.size(); i += 1)
				{
					Vector3 const start = positions[i];
					Vector3 const end = (i + 1) >= positions.size() ? positions[0] : positions[i + 1];
					mesh.vertices.emplace_back(start);
					mesh.vertices.emplace_back(end);
				}

				instance->drawList[color][lineWidth].push(mesh);
			}

			void DebugDrawManager::addAABB(const Physics::AABB& aabb, float lineWidth, const Misc::Color& color)
			{
				addCube(aabb.min, aabb.max, lineWidth, color);
			}

			void DebugDrawManager::addLineMesh(const Data::SubMesh& mesh, float lineWidth, const Misc::Color& color)
			{
				if (instance == nullptr)
					return;

				instance->drawList[color][lineWidth].push(DebugMesh(mesh.vertices, lineWidth, color));
			}

			void DebugDrawManager::clear()
			{
				if (instance == nullptr)
					return;

				instance->drawList = {};
			}
		}
	}
}
