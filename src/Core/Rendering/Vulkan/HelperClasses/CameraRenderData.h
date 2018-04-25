#pragma once
#include <vulkan/vulkan.hpp>
#include "Core/TObject.h"
#include "Core/Rendering/Vulkan/API/BufferVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class VulkanBindingData;

		namespace Rendering
		{
			namespace Vulkan
			{
				//Forward decl
				class Skybox;
				class Pipeline;
				class RenderManager;

				/**
				 * \brief Struct used for storing basic framebuffer data
				 */
				struct FramebufferAttachment
				{
					/**
					 * \brief The framebuffer image
					 */
					vk::Image img;
					/**
					 * \brief The memory the image is mapped to
					 */
					vk::DeviceMemory mem;
					/**
					 * \brief The image view
					 */
					vk::ImageView view;

					/**
					 * \brief Cleans up all the resources used by this FramebufferAttachment
					 * \param device Used for cleaning up the resources
					 */
					void destroy(vk::Device device) const
					{
						device.destroyImage(img);
						device.freeMemory(mem);
						device.destroyImageView(view);
					}
				};

				/**
				 * \brief CameraRenderData is the interface that wraps vulkan render info and is generally bound to a Camera component. 
				 * It defines both onscreen and offscreen data
				 */
				class CameraRenderData : public TObject
				{
				public:
					/**
					 * \brief Creates a new instance of CameraRenderData. Creates all resources
					 * \param rm Reference to the rendermanager for rendering data
					 * \param binding BindingData for rendering information
					 * \param offscreenPass Used for offscreen rendering
					 * \param onscreenPipeline Onscreen shader/rendering pipeline
					 * \param isEditorCamera Will create descriptorsets from ImGui if true
					 */
					void init(RenderManager* rm, VulkanBindingData* binding, vk::RenderPass offscreenPass,
					                 Pipeline* onscreenPipeline, bool isEditorCamera = false);
					/**
					 * \brief Cleans up all the resources used by this CameraRenderData
					 */
					~CameraRenderData();
					/**
					 * \brief Rebuilds the render data. Generally used when the window needs to be resized
					 * \param rm Reference to the rendermanager for rendering data
					 * \param offscreenPass Used for offscreen rendering
					 * \param onscreenPipeline Onscreen shader/rendering pipeline
					 */
					void rebuild(RenderManager* rm, vk::RenderPass offscreenPass, Pipeline* onscreenPipeline);

					/**
					 * \brief Offscreen interface wrapping all the offscreen data
					 */
					struct Offscreen
					{
						/**
						 * \brief The color image attachment
						 */
						FramebufferAttachment color;
						/**
						 * \brief The depth image attachment
						 */
						FramebufferAttachment depth;
						/**
						 * \brief A reference to the offscreen renderpass
						 */
						vk::RenderPass pass;
						/**
						 * \brief The framebuffer we're rendering to
						 */
						vk::Framebuffer buffer = nullptr;

						/**
						 * \brief The sampler that will pass our final screen texture to the shader
						 */
						vk::Sampler sampler;
						/**
						 * \brief The primary command buffer for this camera
						 */
						vk::CommandBuffer cmd;
						/**
						 * \brief The wait semaphore, used to wait for previous cameras
						 */
						vk::Semaphore sema;

						vk::DescriptorSet lightingSet;

						/**
						 * \brief Initializes the offscreen camera data
						 */
						void init(RenderManager*, vk::RenderPass);
						/**
						 * \brief Destroys the offscreen camera data
						 * \param device Used to destroy the offscreen camera data
						 */
						void destroy(vk::Device device) const;

						/**
						 * \brief Create framebuffer attachments
						 */
						void createImages(RenderManager*);
						/**
						 * \brief Create the sampler that's gonna pass the offscreen data to the onscreen pass
						 */
						void createSampler(RenderManager*);
						/**
						 * \brief Create the offscreen framebuffer
						 */
						void createFramebuffer(RenderManager*);
					} offscreen;

					/**
					 * \brief Onscreen interface used to wrap all the onscreen camera data
					 */
					struct Onscreen
					{
						/**
						 * \brief Used for rendering this camera to the screen
						 */
						vk::CommandBuffer secondary;
						/**
						 * \brief Used to pass our screen texture to the screen shader
						 */
						std::array<vk::DescriptorSet, 2> sets;

						std::unique_ptr<BufferVulkan> tempBuf;

						/**
						 * \brief Allocates the offscreen commandbuffer and creates the descriptorset
						 * \param binding Bindingdata used for rendering information
						 * \param offscreen Used to get some information from the offscreen pass
						 * \param isEditorCam Creates descriptorsets that fit the editor pass if true, renderpass if false
						 * \param onscreenPipeline Used to get the descriptor layouts
						 */
						void init(VulkanBindingData* binding, Offscreen offscreen, bool isEditorCam, Pipeline* onscreenPipeline);
						/**
						 * \brief Destroys the resources that have been created
						 * \param device Used to destroy the resources that were created
						 * \param pool Used to deallocate the descriptorpool info
						 */
						void destroy(vk::Device device, vk::DescriptorPool pool) const;

					} onscreen;

					bool getIsPrepared() const { return isPrepared; }
					bool isValid() const;

					std::string tempName;
				private:
					vk::Extent2D lastExtent;
					bool isPrepared = false;
					/**
					 * \brief Describes wether or not the camera is an editor camera
					 */
					bool isEditorCam = false;
					/**
					 * \brief A reference ot VulkanBindingData, used to receive rendering data from other systems
					 */
					VulkanBindingData* binding = nullptr;
					/**
					 * \brief Initializes both the offscreen and onscreen pass
					 * \param rm Reference to the rendermanager for render info
					 * \param offscreenPass The offscreen renderpass
					 * \param onscreenPipeline The shader pipeline that is used to render the camera to a quad
					 */
					void setup(RenderManager* rm, vk::RenderPass offscreenPass, Pipeline* onscreenPipeline);
				};
			}
		}
	}
}
