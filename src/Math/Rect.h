#pragma once

namespace Tristeon
{
	namespace Math
	{
		/**
		 * A 2D rectangle, described by a position and size.
		 */
		struct Rect
		{
			/**
			 * Creates a new instance of rect with a position and size
			 */
			Rect(float x = 0, float y = 0, float width = 0, float height = 0);

			float width;
			float height;
			float x;
			float y;
		};
	}
}
