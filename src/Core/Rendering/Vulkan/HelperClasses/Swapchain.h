#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <misc/Property.h>
namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief A helper struct for obtaining and selecting supported swapchain components
				 */
				struct SwapChainSupportDetails
				{
					/**
					 * \brief Creates a new instance of SwapChainSupportDetails and loads in the capabilities, formats and presentModes.
					 * \param device The vulkan logical device
					 * \param surface The vulkan window surface
					 */
					SwapChainSupportDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface);
					
					/**
					 * \brief The capabilities of the window surface
					 */
					vk::SurfaceCapabilitiesKHR capabilities;
					/**
					 * \brief The rendering formats that the window surface supports
					 */
					std::vector<vk::SurfaceFormatKHR> formats;
					/**
					 * \brief The supported present modes
					 */
					std::vector<vk::PresentModeKHR> presentModes;

					/**
					 * \brief Chooses the most ideal surface format
					 * \return Returns a supported surface format
					 */
					vk::SurfaceFormatKHR chooseFormat();
					/**
					 * \brief Chooses the most ideal present mode
					 * \return Returns a supported present mode
					 */
					vk::PresentModeKHR chooseMode();
					/**
					 * \brief Chooses the most ideal extent
					 * \return Returns a supported swapchain extent
					 */
					vk::Extent2D chooseExtent(int, int) const;
					/**
					 * \return Returns the amount of supported swapchain images
					 */
					uint32_t getImageCount() const;
				};

				/**
				 * \brief Swapchain manages the creation and destruction of the vulkan swapchain and its respective components
				 */
				class Swapchain
				{
					friend class RenderManager;
				public:

					Swapchain(vk::Device device, vk::PhysicalDevice gpu, vk::SurfaceKHR surface, int width, int height);

					/**
					 * \brief Deallocates all resources created by swapchain
					 */
					~Swapchain();

					/**
					 * \brief Rebuild the swapchain with a new width and height
					 * \param newWidth The width of the window
					 * \param newHeight The height of the window
					 */
					void rebuild(int newWidth, int newHeight);

					/**
					* \brief The extent of the swapchain (width, height)
					*/
					ReadOnlyProperty(vk::Extent2D, extent2D);
					GetProperty(extent2D) { return extent; }

					/**
					* \brief The format of the swapchain
					*/
					ReadOnlyProperty(vk::Format, format);
					GetProperty(format) { return _format; }

					/**
					* \brief The main render pass
					*/
					ReadOnlyProperty(vk::RenderPass, renderpass);
					GetProperty(renderpass) { return _renderPass; }

					/**
					 * \return Returns the amount of framebuffers of this swapchain 
					 */
					size_t getFramebufferCount() const { return framebuffers.size(); }
					/**
					 * \return Returns the vulkan swapchain object 
					 */
					vk::SwapchainKHR getSwapchain() const { return swapChain; }
					/**
					 * \return Returns the vulkan window surface object 
					 */
					vk::SurfaceKHR getSurface() const { return surface; }

					/**
					 * \brief Gets the framebuffer at the given index
					 * \param index The index of the framebuffer
					 * \return Returns the framebuffer
					 */
					vk::Framebuffer getFramebufferAt(int index) { return framebuffers[index]; }
					/**
					 * \brief Create swapchain framebuffers
					 */
					void createFramebuffers();
				private:
					/**
					 * \brief Creates a swapchain and its components:
					 * - Create swapchain, load swapchain images
					 * - Create swapchain image views
					 * - Create renderpass
					 * \param width The width of the window
					 * \param height The height of the window
					 */
					void create(int width, int height);
					/**
					 * \brief Creates the swapchain with the given width and height
					 * \param width The width of the window
					 * \param height The height of the window
					 */
					void createSwapchain(int width, int height);
					/**
					 * \brief Creates the image views for the swapchain images
					 */
					void createImageViews();
					/**
					 * \brief Creates the main renderpass
					 */
					void createRenderPass();

					/**
					 * \brief Cleans up the resources allocated by swapchain
					 */
					void cleanup();

					vk::SwapchainKHR swapChain = nullptr;
					vk::RenderPass _renderPass;

					std::vector<vk::Image> images;
					std::vector<vk::ImageView> imageViews;

					std::vector<vk::Framebuffer> framebuffers;

					vk::Format _format;
					vk::Extent2D extent;
					
					vk::Device device;
					vk::PhysicalDevice physicalDevice;
					vk::SurfaceKHR surface;
				};
			}
		}
	}
}
