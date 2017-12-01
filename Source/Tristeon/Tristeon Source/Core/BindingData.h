#pragma once
#include "TObject.h"
#include "Rendering/Window.h"
#include "Rendering/Vulkan/HelperClasses/Swapchain.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief BindingData is a class that is used to share rendering data inbetween engine subsystems. API specific binding data can inherit from this class.
		 */
		class BindingData : public TObject
		{
		public:
			/**
			 * \brief The tristeon window class
			 */
			Rendering::Window* tristeonWindow;

			/**
			 * \brief The GLFW window
			 */
			GLFWwindow* window = nullptr;
		};

		/**
		 * \brief Vulkan specific binding data
		 */
		class VulkanBindingData : public BindingData
		{
		public:
			/**
			 * \brief The selected GPU
			 */
			vk::PhysicalDevice physicalDevice;
			/**
			 * \brief The logical device
			 */
			vk::Device device;
			/**
			 * \brief The main render pass
			 */
			vk::RenderPass renderPass;
			/**
			 * \brief The descriptor pool, for allocation of descriptorsets
			 */
			vk::DescriptorPool descriptorPool;

			/**
			 * \brief The command pool, for allocation of commandbuffers
			 */
			vk::CommandPool commandPool;
			/**
			 * \brief The swapchain
			 */
			Rendering::Vulkan::Swapchain* swapchain;

			/**
			 * \brief The graphics queue
			 */
			vk::Queue graphicsQueue;
			/**
			 * \brief The present queue
			 */
			vk::Queue presentQueue;
		};
	}
}
