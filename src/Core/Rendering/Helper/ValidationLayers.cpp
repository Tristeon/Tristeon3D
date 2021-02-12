#include "ValidationLayers.h"
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Misc/Console.h"

namespace Tristeon::Core::Rendering
{
	//Did we check for support?
	bool ValidationLayers::_checked = false;
	bool ValidationLayers::_supported = false;

	bool ValidationLayers::supported()
	{
		//Supported has been called before, we only need to check for support once
		if (_checked)
			return _supported;

		auto availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char* layer : layers)
		{
			auto layerFound = false;

			//Can we find our layer in any of the available layers?
			for (const auto& properties : availableLayers)
			{
				if (strcmp(layer, properties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			//Couldn't find our layer
			if (!layerFound)
			{
				_checked = true;
				_supported = false;
				return _supported;
			}
		}

		//Found every required layer
		_checked = true;
		_supported = true;
		return _supported;
	}

	VkBool32 ValidationLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
	{
		if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT || severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			return VK_FALSE;
		
		std::string output = "[RENDERER] [VULKAN] ";

		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			output += "[VERBOSE] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			output += "[INFO] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			output += "[WARNING] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			output += "[ERROR] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
		default:;
		}

		switch (type)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			output += "[GENERAL] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			output += "[VALIDATION] ";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			output += "[PERFORMANCE] ";
			break;
		default: ;
		}

		output += callback_data->pMessage;
		if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT || severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			Misc::Console::warning(output);
		else
			Misc::Console::write(output);
		return VK_FALSE;
	}

	vk::Result ValidationLayers::createDebugUtilsMessenger(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, vk::DebugUtilsMessengerEXT* pDebugMessenger)
	{
		const auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return (vk::Result)func(instance, (VkDebugUtilsMessengerCreateInfoEXT*)pCreateInfo, nullptr, (VkDebugUtilsMessengerEXT*)pDebugMessenger);
		return vk::Result::eErrorExtensionNotPresent;
	}

	void ValidationLayers::destroyDebugUtilsMessenger(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger)
	{
		const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func((VkInstance)instance, (VkDebugUtilsMessengerEXT)debugMessenger, nullptr);
	}
}
