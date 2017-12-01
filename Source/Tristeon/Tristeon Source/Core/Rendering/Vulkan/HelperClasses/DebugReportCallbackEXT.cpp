#include "DebugReportCallbackEXT.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				DebugReportCallbackEXT::DebugReportCallbackEXT(vk::Instance instance)
				{
					//Store instance as reference
					this->instance = instance;

					//Create debug report callback
					vk::DebugReportCallbackCreateInfoEXT ci = vk::DebugReportCallbackCreateInfoEXT(
						vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
						callbackFunction);
					result = instance.createDebugReportCallbackEXT(&ci, nullptr, &debug);
				}

				DebugReportCallbackEXT::~DebugReportCallbackEXT()
				{
					instance.destroyDebugReportCallbackEXT(debug);
				}

				vk::Result DebugReportCallbackEXT::getResult() const
				{
					return result;
				}

				VkBool32 DebugReportCallbackEXT::callbackFunction(
					VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
				    uint64_t obj, size_t location, int32_t code, const char* layerPrefix,
				    const char* msg, void* userData)
				{
					//Error
					if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) == VK_DEBUG_REPORT_ERROR_BIT_EXT)
						Misc::Console::error("Vulkan validation: " + std::string(msg));
					//Warning
					else if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) == VK_DEBUG_REPORT_WARNING_BIT_EXT)
						Misc::Console::warning("Vulkan validation: " + std::string(msg));

					//Applications should always return VK_FALSE so that they see the same behavior with and without validation layers enabled.
					return VK_FALSE;
				}
			}
		}
	}
}
