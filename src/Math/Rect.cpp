#include "Rect.h"
#include "Vector2.h"

namespace Tristeon
{
	namespace Math
	{
		Rect::Rect(float x, float y, float width, float height): width(width), height(height), minX(x), minY(y) { }

		bool Rect::intersects(Vector2 point) const
		{
			return point.x > minX && 
				point.x < minX + width &&
				point.y > minY && 
				point.y < minY + height;
		}
	}
}
