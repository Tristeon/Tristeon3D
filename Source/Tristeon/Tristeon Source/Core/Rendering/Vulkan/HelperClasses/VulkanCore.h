#pragma once
#include <vulkan/vulkan.hpp>
#include "DebugReportCallbackEXT.h"

#include <Misc/Property.h>
#include "Editor/Scene editor/SceneWindow.h"

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
				class Device;

				/**
				 * \brief Defines functions for the creation of some of the core Vulkan objects. 
				 */
				class VulkanCore
				{
				public:
					/**
					 * \brief Creates a new instance of VulkanCore, initializing the core vulkan objects
					 * \param data Rendering data
					 */
					explicit VulkanCore(VulkanBindingData* data);
					/**
					 * \brief Destroys the core vulkan objects
					 */
					~VulkanCore();

					/**
					* \brief The vulkan logical device
					*/
					ReadOnlyProperty(vk::Device, device);
					GetPropertyConst(device);

					/**
					* \brief The vulkan physical device / gpu
					*/
					ReadOnlyProperty(vk::PhysicalDevice, physicalDevice);
					GetPropertyConst(physicalDevice);

					/**
					* \brief The vulkan present queue
					*/
					ReadOnlyProperty(vk::Queue, presentQueue);
					GetPropertyConst(presentQueue);

					/**
					* \brief The vulkan graphics queue
					*/
					ReadOnlyProperty(vk::Queue, graphicsQueue);
					GetPropertyConst(graphicsQueue);

					/**
					 * \brief The vulkan instance
					 */
					vk::Instance instance = nullptr;
					/**
					 * \brief The vulkan surface
					 */
					vk::SurfaceKHR surface = nullptr;
					/**
					 * \brief Tristeon device object, wraps around the logical and graphical device
					 */
					Device* dev = nullptr;

					/**
					 * \brief The debug report callback object
					 */
					DebugReportCallbackEXT* debug = nullptr;

				private:
					/**
					 * \brief Creates the vulkan instance
					 */
					void createInstance();
					/**
					 * \brief Creates the debug callback
					 */
					void setupDebugCallback();
					/**
					 * \brief Creates the window surface
					 */
					void createWindowSurface();
					/**
					 * \brief Stores our created data into the VulkanBindingData class
					 */
					void storeBindingData() const;
					
					/**
					 * \brief Rendering data, stored to share information with other classes
					 */
					VulkanBindingData* binding;
					/**
					 * \brief The GLFW window, stored for window surface creation
					 */
					GLFWwindow* window;
				};
			}
		}
	}
}
