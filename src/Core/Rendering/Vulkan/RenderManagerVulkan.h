#pragma once
#include "Core/Rendering/RenderManager.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Math/Vector2.h"
#include "Misc/ObjectPool.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class BindingData;
		class VulkanBindingData;
		class Transform;

		namespace Rendering
		{
			class Skybox;
			namespace Vulkan
			{
				//Forward decl
				class CameraRenderData;
				class InternalMeshRenderer;
				class Pipeline;
				class Forward;
				class Device;
				class Swapchain;
				class EditorGrid;
				class DebugReportCallbackEXT;
				class VulkanCore;
				class Skybox;
				class WindowContextVulkan;

				/**
				 * \brief EditorData is a small struct wrapping around  
				 */
				struct EditorData : TObject
				{
					/**
					 * \brief The editor camera render data
					 */
					CameraRenderData* cam = nullptr;
					/**
					 * \brief The editor camera transform
					 */
					Transform* trans = nullptr;
					/**
					 * \brief The editor window size
					 */
					Math::Vector2 size;
				};

				/**
				 * \brief RenderData is a small struct for sharing rendering information with rendererss
				 */
				struct RenderData : TObject
				{
					/**
					 * \brief The Inheritance info any state that will be inherited from the primary command buffer
					 */
					vk::CommandBufferInheritanceInfo inheritance;
					/**
					 * \brief The camera viewport
					 */
					vk::Viewport viewport;
					/**
					 * \brief The camera scissor
					 */
					vk::Rect2D scissor;

					/**
					 * \brief The priamry command buffer
					 */
					vk::CommandBuffer primary;

					/**
					 * \brief The camera view matrix
					 */
					glm::mat4 view;
					/**
					 * \brief The camera projection matrix
					 */
					glm::mat4 projection;

					/**
					 * \brief The last used secondary buffer, set by the renderer
					 */
					vk::CommandBuffer lastUsedSecondaryBuffer = nullptr;

					vk::DescriptorSet skyboxSet;
				};

				/**
				 * \brief Vulkan::RenderManager is the vulkan implementation of the RenderManager
				 */
				class RenderManager : public Rendering::RenderManager
				{
					//Friends
					friend CameraRenderData;
					friend RenderTechnique;
					friend Forward;

				public:
					/**
					 * \brief Creates a new instance of RenderManager
					 * \param data The render binding data
					 */
					explicit RenderManager(BindingData* data);
					/**
					 * \brief Deallocates all resources allocated by render manager
					 */
					~RenderManager() override;

					/**
					 * \brief Initializes everything that is required for the rendermanager to function
					 */
					void init() override;
					/**
					 * \brief Renders the current scene to the screen
					 */
					void render() override;
					
					static Pipeline* getPipeline(ShaderFile file);
				protected:
					/**
					 * \brief Returns a material serialized from the given filepath
					 * \param filePath The filepath of the material
					 * \return A material serialized from the given filepath, or from the cached materials
					 */
					Rendering::Material* getmaterial(std::string filePath) override;
					/**
					 * \brief Releases all the leftover cameradata
					 */
					void reset() override;

					/**
					 * \brief Registers a renderer and its internal renderer. Used as callback function for the manager protocol only.
					 * \param msg A message containing the renderer in msg.userData
					 * \return Returns the renderer as TObject so inherited classes can access the newly added renderer when overriding this function
					 */
					TObject* registerRenderer(Message msg) override;
					/**
					 * \brief Deregisters a renderer and its internal renderer. Used as callback function for the manager protocol only.
					 * \param msg A message containing the renderer in msg.userData
					 * \return Returns the renderer as TObject so inherited classes can access the removed renderer when overriding this function
					 */
					TObject* deregisterRenderer(Message msg) override;

					/**
					 * \brief Registers a camera and creates renderdata for said camera. Used as callback function for the manager protocol only.
					 * \param msg A message containing the camera in msg.userData
					 * \return Returns the camera so inherited classes can access the newly added camera when overriding this function
					 */
					Components::Camera* registerCamera(Message msg) override;
					/**
					 * \brief Deregisters a camera and destroys the respective renderdata. Used as callback function for the manager protocol only.
					 * \param msg A message containing the camera in msg.userData
					 * \return Returns the camera so inherited classes can access the camera that has been removed when overriding this function
					 */
					Components::Camera* deregisterCamera(Message msg) override;
				private:
					/**
					 * \brief Initializes vulkan
					 */
					void setupVulkan();
					/**
					 * \brief Creates a descriptor pool, used to allocate descriptorsets
					 */
					void createDescriptorPool();
					/**
					 * \brief Creates a command pool, used to allocate commandbuffers
					 */
					void createCommandPool();
					/**
					 * \brief Creates the main command buffers, used to render to the main screen
					 */
					void createCommandBuffer();

					/**
					 * \brief Renders the scene for each camera
					 */
					void renderScene();

					/**
					 * \brief Rebuilds all resources that are window size related. Used as callback function for the ManagerProtocol
					 * \param newWidth The new width of the window
					 * \param newHeight The new height of the window
					 */
					void resizeWindow(int newWidth, int newHeight);

					/**
					 * \return Returns the current active frame buffer 
					 */
					vk::Framebuffer getActiveFrameBuffer() const;
					
					/**
					 * \brief Submits the cameras' commands to the present queue
					 */
					void submitCameras();

					/**
					 * \brief Reference to the binding data struct
					 */
					VulkanBindingData* data = nullptr;

					//Non-owning pointer
					WindowContextVulkan* vkContext = nullptr;

					/**
					 * \brief Reference to the window, used to bind the rendering to the GLFW window
					 */
					GLFWwindow* window = nullptr;

					/**
					 * \brief Shader pipelines defining the rendering pipeline for a shader combination
					 */
					std::vector<Pipeline*> pipelines;

					vk::CommandPool commandPool;
					vk::DescriptorPool descriptorPool;

					vk::CommandBuffer primaryCmd;

					vector<InternalMeshRenderer*> internalRenderers;
					std::map<Components::Camera*, CameraRenderData*> cameraData;
					ObjectPool<CameraRenderData*> cameraDataPool;
#ifdef TRISTEON_EDITOR
					/**
					 * \brief EditorData, stores the information we need for the editor camera to render
					 */
					EditorData editor;
					/**
					 * \brief A wrapper around an unregistered meshrenderer, used to render a grid to the scene view
					 */
					EditorGrid* grid = nullptr;

					Vulkan::Skybox* editorSkybox = nullptr;
#endif
					/**
					 * \brief The offscreen pass, used to render the scene to an image
					 */
					vk::RenderPass offscreenPass;
					/**
					 * \brief The onscreen pipeline, used to render camera images to the screen
					 */
					Pipeline* onscreenPipeline = nullptr;

					/**
					 * \brief Creates the onscreen pipeline, which is used to render camera images to the screen
					 */
					void prepareOnscreenPipeline();
					/**
					 * \brief Creates the offscreen renderpass, used to render the scene to an image
					 */
					void prepareOffscreenPass();
				protected:
					Rendering::Skybox* _getSkybox(std::string filePath) override;
				};
			}
		}
	}
}
