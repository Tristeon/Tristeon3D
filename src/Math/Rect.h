#pragma once
#include "Misc/Property.h"
#include "Core/Components/Camera.h"

namespace Tristeon
{
	namespace Math
	{
		struct Vector2;

		/**
		 * A 2D rectangle, described by a position and size.
		 */
		struct Rect
		{
			/**
			 * Creates a new instance of rect with a position and size
			 */
			Rect(float x = 0, float y = 0, float width = 0, float height = 0);
			Rect& operator=(Rect const& rect);

			float width;
			float height;
			float minX;
			float minY;

			ReadOnlyProperty(Rect, maxX, float);
			GetProperty(maxX) { return minX + width; }

			ReadOnlyProperty(Rect, maxY, float);
			GetProperty(maxY) { return minY + height; }

			bool intersects(Vector2 point) const;
		};
	}
}
