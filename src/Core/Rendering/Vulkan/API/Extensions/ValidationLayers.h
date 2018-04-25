#pragma once
#include <vector>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief ValidationLayers is a small static wrapper class that implements basic validation checking
				 */
				class ValidationLayers
				{
				public:
					/**
					 * \brief Checks if the validation layers are enabled
					 * \return Returns true if the validation layers are enabled
					 */
					static bool enabled();
					/**
					 * \brief Checks if the validation layers are supported
					 * \return Returns true if the validation layers are supported
					 */
					static bool supported();
					/**
					 * \brief The required validation layers
					 */
					static std::vector<const char*> validationLayers;
				private:
					/**
					 * \brief Checks if we've checked for support before, if so we can return _supported immediately
					 */
					static bool checkedSupport;
					/**
					 * \brief Checks if the validation layers are supported. Is set the first time supported() is called, and is used as result for supported after that.
					 */
					static bool _supported;

					/**
					 * \brief Is true if the validation layers are enabled
					 */
					static int enableValidationLayers;
				};
			}
		}
	}
}