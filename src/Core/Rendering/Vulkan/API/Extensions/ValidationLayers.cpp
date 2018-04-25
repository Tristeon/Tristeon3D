#include "ValidationLayers.h"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				//Required validation layers
				std::vector<const char*> ValidationLayers::validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
				
#if TRISTEON_EDITOR 
				int ValidationLayers::enableValidationLayers = true;
#else
				int ValidationLayers::enableValidationLayers = false;
#endif
				//Did we check for support?
				bool ValidationLayers::checkedSupport = false;
				bool ValidationLayers::_supported = false;

				bool ValidationLayers::enabled()
				{
					return enableValidationLayers;
				}

				bool ValidationLayers::supported()
				{
					//Supported has been called before, we only need to check for support once
					if (checkedSupport)
						return _supported;

					std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
					
					for (const char* layer : validationLayers)
					{
						bool layerFound = false;

						//Can we find our layer in any of the available layers?
						for (const auto& properties : availableLayers)
							if (strcmp(layer, properties.layerName) == 0)
							{
								layerFound = true;
								break;
							}

						//Couldn't find our layer
						if (!layerFound)
						{
							checkedSupport = true;
							_supported = false;
							return _supported;
						}
					}

					//Found every required layer
					checkedSupport = true;
					_supported = true;
					return _supported;
				}
			}
		}
	}
}
