#include "DebugDrawManager.h"
#include <cmath>
#include <glm/gtc/constants.inl>

namespace Tristeon::Core::Rendering
{
	DebugDrawManager* DebugDrawManager::instance;

	void DebugDrawManager::addLine(const Math::Vector3& from, const Math::Vector3& to, float width, const Misc::Color& color)
	{
		instance->drawList.push(Line(Data::Vertex(from), Data::Vertex(to), width, color));
	}

	void DebugDrawManager::addCube(const Math::Vector3& min, const Math::Vector3& max, float lineWidth, const Misc::Color& color)
	{
		//Rect coords 1
		auto const bl1 = min;
		auto const tl1 = Math::Vector3(min.x, max.y, min.z);
		auto const br1 = Math::Vector3(max.x, min.y, min.z);
		auto const tr1 = Math::Vector3(max.x, max.y, min.z);

		//Rect coords 2
		auto const bl2 = Math::Vector3(min.x, min.y, max.z);
		auto const tl2 = Math::Vector3(min.x, max.y, max.z);
		auto const br2 = Math::Vector3(max.x, min.y, max.z);
		auto const tr2 = max;

		//Rect 1
		instance->drawList.push(Line(bl1, tl1, lineWidth, color));
		instance->drawList.push(Line(tl1, tr1, lineWidth, color));
		instance->drawList.push(Line(tr1, br1, lineWidth, color));
		instance->drawList.push(Line(br1, bl1, lineWidth, color));

		//Rect 2
		instance->drawList.push(Line(bl2, tl2, lineWidth, color));
		instance->drawList.push(Line(tl2, tr2, lineWidth, color));
		instance->drawList.push(Line(tr2, br2, lineWidth, color));
		instance->drawList.push(Line(br2, bl2, lineWidth, color));

		//Connecting corners
		instance->drawList.push(Line(bl1, bl2, lineWidth, color));
		instance->drawList.push(Line(tl1, tl2, lineWidth, color));
		instance->drawList.push(Line(tr1, tr2, lineWidth, color));
		instance->drawList.push(Line(br1, br2, lineWidth, color));
	}

	void DebugDrawManager::addSphere(const Math::Vector3& center, float r, float lineWidth, const Misc::Color& color, int circles, int resolution)
	{
		auto const PI = 3.14159265f;
		std::vector<Math::Vector3> positions;

		//Horizontal lines
		for (float j = 0; j < PI; j += PI / (circles + 1)) 
		{
			//Store first pos to make sure that we'll finish the circle
			Math::Vector3 firstPos;
			auto setFirstPos = false;

			auto const y = r * cos(j);
			for (float i = 0; i < 2 * PI; i += PI / resolution) 
			{
				auto const x = r * cos(i) * sin(j);
				auto const z = r * sin(i) * sin(j);
				if (!setFirstPos)
				{
					setFirstPos = true;
					firstPos = center + Math::Vector3(x, y, z);
				}
				positions.push_back(center + Math::Vector3(x, y, z));
			}
			//Finish circle
			positions.push_back(firstPos);

			//Apply
			for (auto i = 0; i < positions.size(); i += 1)
			{
				auto start = positions[i];
				auto end = (i + 1) >= positions.size() ? positions[0] : positions[i + 1];
				auto const l = Line(start, end, lineWidth, color);
				instance->drawList.push(l);
			}
			positions.clear();
		}

		//Vertical lines
		for (float j = 0; j < PI; j += PI / circles) 
		{
			for (float i = 0; i < 2 * PI; i += PI / resolution) 
			{
				auto x = r*sin(i)*cos(j);
				auto y = r*cos(i);
				auto z = r*sin(j)*sin(i);
				positions.push_back(center + Math::Vector3(x, y, z));
			}
		}

		//Apply
		for (auto i = 0; i < positions.size(); i += 1)
		{
			auto start = positions[i];
			auto end = (i + 1) >= positions.size() ? positions[0] : positions[i + 1];
			auto const l = Line(start, end, lineWidth, color);
			instance->drawList.push(l);
		}
	}
}
