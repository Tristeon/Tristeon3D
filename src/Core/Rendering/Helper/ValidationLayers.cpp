#include "ValidationLayers.h"
#include <vector>
#include <vulkan/vulkan.hpp>

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
}
