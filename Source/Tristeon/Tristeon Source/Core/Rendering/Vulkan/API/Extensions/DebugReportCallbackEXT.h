#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief DebugReportCallbackEXT is a helper class for the validation layer debug callback in Vulkan.
				 */
				class DebugReportCallbackEXT
				{
				public:
					/**
					 * \brief Creates a new instance of DebugReportCallbackEXT and sets the callback to DebugReportCallback::callbackFunction()
					 * \param instance A reference to the main vulkan instance
					 */
					DebugReportCallbackEXT(vk::Instance instance);
					/**
					 * \brief Destroys the debug report callback
					 */
					~DebugReportCallbackEXT();
					/**
					 * \brief Gets the result value based on the DebugReportCallback create function
					 * \return A vk::Result value describing the result
					 */
					vk::Result getResult() const;
				private:
					/**
					 * \brief The result of the DebugReportCallback create function
					 */
					vk::Result result;
					/**
					 * \brief The debug callback instance
					 */
					vk::DebugReportCallbackEXT debug = nullptr;

					/**
					 * \brief A reference to the vk::Instance, used for creating/destroying debug
					 */
					vk::Instance instance;

					/**
					 * \brief The vulkan error callback function
					 * \param flags Describes the type of error
					 * \param objType A VkDebugReportObjectTypeEXT specifying the type of object being used or created at the time the event was triggered.
					 * \param obj Object where the issue was detected. object may be VK_NULL_OBJECT if there is no object associated with the event.
					 * \param location A component (layer, driver, loader) defined value that indicates the location of the trigger. This is an optional value.
					 * \param code A layer-defined value indicating what test triggered this callback
					 * \param layerPrefix The abbreviation of the component making the callback.
					 * \param msg A null-terminated string detailing the trigger conditions.
					 * \param userData The user data given when the DebugReportCallback was created.
					 * \return The callback returns a VkBool32 that indicates to the calling layer if the Vulkan call should be aborted or not.
					 */
					static VKAPI_ATTR VkBool32 VKAPI_CALL callbackFunction(
						VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
						uint64_t obj, size_t location, int32_t code,
						const char* layerPrefix, const char* msg, void* userData);
				};
			}
		}
	}
}