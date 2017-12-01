#pragma once
#include "Core/Components/Camera.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			//Forward decl
			class RenderManager;

			/**
			 * \brief RenderTechnique defines basic behavior that should be overriden by inherited classes. 
			 * RenderTechnique is used to store forward/deferred rendering techniques without explicitly knowing about them.
			 * RenderTechnique is owned by RenderManager
			 */
			class RenderTechnique
			{
			public:
				/**
				 * \brief Creates a new instance of RenderTechnique.
				 * \param renderManager RenderManager gets stored as reference to get rendering information
				 */
				explicit RenderTechnique(RenderManager* renderManager);

				/**
				 * \brief Renders the scene from the given camera's perspective
				 * \param view The view matrix of the camera
				 * \param proj The projection matrix of the camera
				 * \param info Userdata
				 */
				virtual void renderScene(glm::mat4 view, glm::mat4 proj, TObject* info = nullptr) = 0;
				/**
				 * \brief Renders all the cameras to the screen
				 */
				virtual void renderCameras() = 0;

			protected:
				/**
				 * \brief A reference to rendermanager, is used to get rendering information
				 */
				RenderManager* manager = nullptr;
			};
		}
	}
}
