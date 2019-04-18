#include "Line.h"

namespace Tristeon
{
	namespace Physics
	{
		Line::Line() {}

		Line::Line(Vector3 start, Vector3 end) : startPoint(start), endPoint(end) {}

		Line::Line(float startX, float startY, float startZ, float endX, float endY, float endZ)
		{
			startPoint = Vector3(startX, startY, startZ);
			endPoint = Vector3(endX, endY, endZ);
		}
	}
}