#pragma once
#include "Rendering/Window.h"
#include <memory>

#include <vulkan/vulkan.hpp>
#include "Rendering/Vulkan/HelperClasses/Swapchain.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief BindingData is used to share rendering data between engine subsystems. API specific binding data can inherit from this class.
		 */
		class BindingData
		{
			friend std::unique_ptr<BindingData>::deleter_type;
		public:
			Rendering::Window* tristeonWindow;
			GLFWwindow* window = nullptr;

			/**
			 * \brief Default getInstance function for BindingData. Will return the base class, is not guaranteed to return an instance.
			 */
			static BindingData* getInstance()
			{
				return instance.get();
			}
		protected: 
			BindingData() = default;
			virtual ~BindingData() = default;

			static std::unique_ptr<BindingData> instance;
		};

		class VulkanBindingData : public BindingData
		{
		public:
			/**
			 * \brief Creates a VulkanBindingData instance if there isn't any, otherwise it'll simply just return instance.
			 * Might return nullptr if the instance is of a different type than VulkanBindingData.
			 */
			static VulkanBindingData* getInstance()
			{
				if (!instance)
					instance = std::unique_ptr<BindingData>(new VulkanBindingData());
				return dynamic_cast<VulkanBindingData*>(instance.get());
			}

			vk::PhysicalDevice physicalDevice;
			vk::Device device;
			vk::RenderPass renderPass;
			vk::DescriptorPool descriptorPool;
			vk::CommandPool commandPool;
			Rendering::Vulkan::Swapchain* swapchain;
			vk::Queue graphicsQueue;
			vk::Queue presentQueue;
		protected:
			VulkanBindingData() = default;
		};
	}
}
