#pragma once
#include "Core/TObject.h"
#include "Data/Mesh.h"

namespace Tristeon {
	namespace Data {
		struct SubMesh;
	}
}

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class BindingData;

		namespace Rendering
		{
			//Forward decl
			class Renderer;

			/**
			 * \brief The internal renderer is an interface that is used to run 
			 * API specific draw calls without interfering with the automatic component system.
			 * It is generally used by the Renderer class / its subclasses.
			 */
			class InternalRenderer : public TObject
			{
			public:
				InternalRenderer(Renderer* renderer);
				/**
				 * \brief The render call. In this call you can call draw calls to the API in inherited classes.
				 */
				virtual void render() = 0;
				/**
				 * \brief Init allows you to initialize render based information with the given binding data.
				 * \param data Render data used to share global information / classes that the renderer needs to access.
				 */
				virtual void init(BindingData* data) = 0;

				/**
				 * \brief Callback function for when the mesh has been changed
				 * \param mesh The new mesh
				 */
				virtual void onMeshChange(Data::SubMesh mesh) {}
			private:
				/**
				 * \brief The renderer this object is attached to
				 */
				Renderer* renderer;
			};
		}
	}
}
