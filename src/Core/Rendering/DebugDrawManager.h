#pragma once
#include "Data/Mesh.h"
#include "Misc/Color.h"
#include <queue>
#include <unordered_map>

namespace Tristeon
{
	namespace Physics { class AABB; }

	namespace Math { struct Vector3; }

	namespace Core
	{
		namespace Rendering
		{
			/**
			 * DebugDrawManager is a utility class for drawing primitive shapes. This class gets overriden to define API specific rendering behavior
			 */
			class DebugDrawManager
			{
			public:
				/**
				 * Adds a line to the drawlist
				 * \param from The starting point of the line
				 * \param to The end point of the line
				 * \param width The width of the line 
				 * \param color The color of the line
				 */
				static void addLine(const Math::Vector3& from, const Math::Vector3& to, float width, const Misc::Color& color);
				/**
				 * Adds an AABB cube to the drawlist
				 * \param min The smallest point of the cube
				 * \param max The biggest point of a cube
				 * \param lineWidth The width of the lines
				 * \param color The color of the cube
				 */
				static void addCube(const Math::Vector3& min, const Math::Vector3& max, float lineWidth, const Misc::Color& color);

				/**
				 * Adds a OBB cube to the drawlist
				 * \param center The center point of the OBB
				 * \param size The size of the OBB
				 * \param rotation The rotation of the OBB
				 * \param lineWidth The width of the lines
				 * \param color The color of the cube
				 */
				static void addCube(const Vector3& center, const Vector3& size, const Vector3& rotation, float lineWidth, const Misc::Color& color);
				
				/**
				 * Adds a sphere to the drawlist
				 * \param center The center position of the sphere
				 * \param radius The radius of the sphere
				 * \param lineWidth The width of the lines of the sphere
				 * \param color The color of the sphere
				 * \param circles The amount of horizontal and vertical circles used to display the sphere
				 * \param resolution The amount of points in the sphere
				 */
				static void addSphere(const Math::Vector3& center, float radius, float lineWidth, const Misc::Color& color, int circles = 4, int resolution = 15);
				
				/**
				 * Adds an AABB to the drawlist
				 * \param aabb the AABB
				 * \param lineWidth the width of the lines
				 * \param color the color of the AABB
				 */
				static void addAABB(const Physics::AABB& aabb, float lineWidth, const Misc::Color& color);

				/**
				 * Adds a line based mesh to the drawlist
				 * \param mesh the Mesh to be added
				 * \param lineWidth the width of the lines
				 * \param color the color the mesh will be drawn in
				 */
				static void addLineMesh(const Data::SubMesh& mesh, float lineWidth, const Misc::Color& color);

				/**
				 * The deconstructor of DebugDrawManager, has to be virtual to allow for correct destruction
				 */
				virtual ~DebugDrawManager() = default;

				/**
				 * Empties the draw list
				 */
				static void clear();
			protected:
				/**
				 * The line struct describes a single renderable line, with its respective properties
				 */
				struct DebugMesh
				{
					/**
					 * The vertices of the mesh
					 */
					std::vector<Data::Vertex> vertices;
					/**
					 * The width of the mesh
					 */
					float width;
					/**
					 * The color of the mesh
					 */
					Misc::Color color;

					DebugMesh(std::vector<Data::Vertex> vertices, float width, Misc::Color color) : vertices(vertices), width(width), color(color) { /*Empty*/ }
				};

				/**
				 * The instance of the debug draw manager, used to allow static functions like addLine
				 */
				static DebugDrawManager* instance;

				std::unordered_map<Misc::Color, std::unordered_map<float, std::queue<DebugMesh>>> drawList;
				/**
				 * The draw function, renders the queued drawables. Gets overriden by inherited classes for API specific rendering
				 */
				virtual void render() = 0;
			};
		}
	}
}
