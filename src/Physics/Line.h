#pragma once
#include "Math/Vector3.h"

namespace Tristeon
{
	namespace Physics
	{
		class Line
		{
		public:
			Line();
			Line(Vector3 start, Vector3 end);
			Line(float startX, float startY, float startZ, float endX, float endY, float endZ);
			Vector3 startPoint;
			Vector3 endPoint;
		};
	}
}