#include "DebugDrawManager.h"
#include <cmath>
#include <glm/gtc/constants.inl>
#include "Physics/AABB.h"

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
				mesh.vertices.push_back(Data::Vertex(bl1));
				mesh.vertices.push_back(Data::Vertex(tl1));

				mesh.vertices.push_back(Data::Vertex(tl1));
				mesh.vertices.push_back(Data::Vertex(tr1));

				mesh.vertices.push_back(Data::Vertex(tr1));
				mesh.vertices.push_back(Data::Vertex(br1));

				mesh.vertices.push_back(Data::Vertex(br1));
				mesh.vertices.push_back(Data::Vertex(bl1));

				//Rect 2
				mesh.vertices.push_back(Data::Vertex(bl2));
				mesh.vertices.push_back(Data::Vertex(tl2));

				mesh.vertices.push_back(Data::Vertex(tl2));
				mesh.vertices.push_back(Data::Vertex(tr2));

				mesh.vertices.push_back(Data::Vertex(tr2));
				mesh.vertices.push_back(Data::Vertex(br2));

				mesh.vertices.push_back(Data::Vertex(br2));
				mesh.vertices.push_back(Data::Vertex(bl2));

				//Connecting corners
				mesh.vertices.push_back(Data::Vertex(bl1));
				mesh.vertices.push_back(Data::Vertex(bl2));

				mesh.vertices.push_back(Data::Vertex(tl1));
				mesh.vertices.push_back(Data::Vertex(tl2));

				mesh.vertices.push_back(Data::Vertex(tr1));
				mesh.vertices.push_back(Data::Vertex(tr2));

				mesh.vertices.push_back(Data::Vertex(br1));
				mesh.vertices.push_back(Data::Vertex(br2));

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
						mesh.vertices.push_back(start);
						mesh.vertices.push_back(end);
					}
					positions.clear();
				}

				//Vertical lines
				for (float j = 0; j < PI; j += PI / circles) 
				{
					for (float i = 0; i < 2 * PI; i += PI / resolution) 
					{
						float x = r*sin(i)*cos(j);
						float y = r*cos(i);
						float z = r*sin(j)*sin(i);
						positions.push_back(center + Vector3(x, y, z));
					}
				}

				//Apply
				for (int i = 0; i < positions.size(); i += 1)
				{
					Vector3 const start = positions[i];
					Vector3 const end = (i + 1) >= positions.size() ? positions[0] : positions[i + 1];
					mesh.vertices.push_back(start);
					mesh.vertices.push_back(end);
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
