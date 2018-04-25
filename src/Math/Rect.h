#pragma once

namespace Tristeon
{
	namespace Math
	{
		/**
		 * \brief Rect class interface
		 */
		class Rect
		{
		public:
			/**
			 * \brief Creates a new instance of rect with width, height, x, and y = 0
			 */
			Rect();
			/**
			 * \brief Creates a new instance of rect
			 * \param x the x position of the rect
			 * \param y the y position of the rect
			 * \param width the width of the rect
			 * \param height the height of the rect
			 */
			Rect(float x, float y, float width, float height);

			/**
			 * \brief The width of the rect
			 */
			float width;
			/**
			 * \brief The height of the rect
			 */
			float height;
			/**
			 * \brief The x position of the rect
			 */
			float x;
			/**
			 * \brief The y position of the rect
			 */
			float y;
		};
	}
}
