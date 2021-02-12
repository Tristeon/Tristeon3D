#pragma once
#include <array>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_platform.h>

namespace Tristeon::Core::Rendering
{
	/**
	 * \brief ValidationLayers is a small static wrapper class that implements basic validation checking
	 */
	class ValidationLayers
	{
	public:
		/**
		 * \brief Is true if the validation layers are enabled
		 */
#ifdef TRISTEON_LOGENABLED
		constexpr static bool enabled = true;
		constexpr static std::array<const char*, 1> layers{ "VK_LAYER_KHRONOS_validation" };
#else
		constexpr static bool enabled = false;
		constexpr static std::array<char*, 0> layers { };
#endif
		
		/**
		 * \brief Checks if the validation layers are supported
		 * \return Returns true if the validation layers are supported
		 */
		static bool supported();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

		static vk::Result createDebugUtilsMessenger(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, vk::DebugUtilsMessengerEXT* pDebugMessenger);

		static void destroyDebugUtilsMessenger(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger);
	private:
		/**
		 * \brief Checks if we've checked for support before, if so we can return _supported immediately
		 */
		static bool _checked;
		/**
		 * \brief Checks if the validation layers are supported. Is set the first time supported() is called, and is used as result for supported after that.
		 */
		static bool _supported;
	};
}
