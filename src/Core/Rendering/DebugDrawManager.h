#pragma once
#include "Data/Mesh.h"
#include "Misc/Color.h"
#include <queue>

namespace Tristeon
{
	namespace Math { struct Vector3; }

	namespace Core
	{
		namespace Rendering
		{
			/**
			 * \brief DebugDrawManager is a utility class for drawing primitive shapes. This class gets overriden to define API specific rendering behavior
			 */
			class DebugDrawManager
			{
			public:
				/**
				 * \brief Adds a line to the drawlist
				 * \param from The starting point of the line
				 * \param to The end point of the line
				 * \param width The width of the line 
				 * \param color The color of the line
				 */
				static void addLine(const Math::Vector3& from, const Math::Vector3& to, float width, const Misc::Color& color);
				/**
				 * \brief Adds a cube to the drawlist
				 * \param min The smallest point of the cube
				 * \param max The biggest point of a cube
				 * \param lineWidth The width of the lines
				 * \param color The color of the cube
				 */
				static void addCube(const Math::Vector3& min, const Math::Vector3& max, float lineWidth, const Misc::Color& color);
				/**
				 * \brief Adds a sphere to the drawlist
				 * \param center The center position of the sphere
				 * \param radius The radius of the sphere
				 * \param lineWidth The width of the lines of the sphere
				 * \param color The color of the sphere
				 * \param circles The amount of horizontal and vertical circles used to display the sphere
				 * \param resolution The amount of points in the sphere
				 */
				static void addSphere(const Math::Vector3& center, float radius, float lineWidth, const Misc::Color& color, int circles = 4, int resolution = 15);
				
				/**
				 * \brief The deconstructor of DebugDrawManager, has to be virtual to allow for correct destruction
				 */
				virtual ~DebugDrawManager() = default;

				static void clear();
			protected:
				/**
				 * \brief The line struct describes a single renderable line, with its respective properties
				 */
				struct Line
				{
					/**
					 * \brief The starting point of the line
					 */
					Data::Vertex start;
					/**
					 * \brief The end point of the line
					 */
					Data::Vertex end;
					/**
					 * \brief The width of the line
					 */
					float width;
					/**
					 * \brief The color of the line
					 */
					Misc::Color color;

					/**
					 * \brief Creates a new line with the given properties
					 * \param start The starting point of the line
					 * \param end The end point of the line
					 * \param width The width of the line
					 * \param color The color of the line
					 */
					Line(Data::Vertex start, Data::Vertex end, float width, Misc::Color color) : start(start), end(end), width(width), color(color) { /*Empty*/ }
				};

				/**
				 * \brief The instance of the debug draw manager, used to allow static functions like addLine
				 */
				static DebugDrawManager* instance;
				
				/**
				 * \brief The drawlist, resets every frame
				 */
				std::queue<Line> drawList;
				/**
				 * \brief The draw function, renders the queued drawables. Gets overriden by inherited classes for API specific rendering
				 */
				virtual void draw() = 0;
			};
		}
	}
}