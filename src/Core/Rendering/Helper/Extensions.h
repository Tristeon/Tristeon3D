#pragma once
#include <iostream>
#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	class Extensions
	{
	public:
		Extensions()
		{
			available_extensions = vk::enumerateInstanceExtensionProperties();
			std::cout << "[RENDERER] [INIT] [VULKAN] Available Vulkan extensions:\n";
			for (auto ext : available_extensions)
				std::cout << "\t" << ext.extensionName << "\n";
		}

		void add_required(const char* ext)
		{
			if (!extension_supported(ext))
				throw std::runtime_error("[RENDERER] [ERROR] [VULKAN] Mandatory extension " + std::string(ext) + " requested but not supported");

			requested_extensions.push_back(ext);
		}

		bool add_optional(const char* ext)
		{
			if (extension_supported(ext))
			{
				requested_extensions.push_back(ext);
				return true;
			}

			std::cout << "[RENDERER] [WARNING] [VULKAN] Optional extension " << ext << " requested but not supported\n";
			return false;
		}

		std::vector<const char*> get()
		{
			std::cout << "[RENDERER] [INIT] [VULKAN] Requested extensions:\n";
			for (auto ext : requested_extensions)
				std::cout << "\t" << ext << "\n";

			return requested_extensions;
		}

		static bool extension_supported(const char* ext)
		{
			auto availableExtensions = vk::enumerateInstanceExtensionProperties();
			return std::find_if(availableExtensions.begin(), availableExtensions.end(),
				[&](vk::ExtensionProperties properties) { return strcmp(properties.extensionName, ext); }) != availableExtensions.end();
		}

	private:
		std::vector<vk::ExtensionProperties> available_extensions;
		std::vector<const char*> requested_extensions;
	};
}