#include "Rect.h"

namespace Tristeon
{
	namespace Math
	{
		Rect::Rect(): width(0), height(0), x(0), y(0)
		{
			//Empty
		}

		Rect::Rect(float x, float y, float width, float height): width(width), height(height), x(x), y(y)
		{
			//Empty
		}
	}
}