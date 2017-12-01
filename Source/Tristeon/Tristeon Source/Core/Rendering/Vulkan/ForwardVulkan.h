#pragma once
#include "Core/Rendering/RenderTechniques/RenderTechnique.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				//Forward decl
				class CameraRenderData;
				class RenderManager;

				/**
				 * \brief Vulkan::Forward is the Vulkan implementation of the Forward rendering technique. 
				 */
				class Forward final : public RenderTechnique
				{
				public:
					/**
					 * \brief Creates a new instance of Forward.
					 * \param renderManager Stored as a reference for rendering information
					 */
					explicit Forward(Rendering::RenderManager* renderManager);
					
					/**
					* \brief Renders the scene from the given camera's perspective
					* \param view The view matrix of the camera
					* \param proj The projection matrix of the camera
					* \param info Userdata
					*/
					void renderScene(glm::mat4 view, glm::mat4 proj, TObject* info = nullptr) override;
					/**
					* \brief Renders all the cameras to the screen
					*/
					void renderCameras() override;

				private:
					/**
					 * \brief A reference to Vulkan::RenderManager, for rendering info
					 */
					RenderManager* vkRenderManager;
				};
			}
		}
	}
}
