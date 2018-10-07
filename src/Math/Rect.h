#pragma once

namespace Tristeon
{
	namespace Math
	{
		/**
		 * A 2D rectangle, describe by a position and size.
		 */
		struct Rect
		{
			/**
			 * Creates a new instance of rect with the given paramaters
			 */
			Rect(float x = 0, float y = 0, float width = 0, float height = 0);

			float width;
			float height;
			float x;
			float y;
		};
	}
}
