#pragma once
#include <string>
#include <vulkan/vulkan.hpp>

#include "Core/TObject.h"

namespace Tristeon::Data
{
	/**
	 * Class for image loading
	 */
	class Image : public Core::TObject
	{
	public:
		/**
		 * Creates a new instance of image and loads in the image data based on the filepath.
		 * \param filePath The filepath of the image
		 */
		explicit Image(const std::string& filePath);
		~Image();
		
		/**
		 * Gets the pixels of the image
		 */
		[[nodiscard]] unsigned char* pixels() const;
		/**
		 * Gets the width of the image
		 */
		[[nodiscard]] int width() const;
		/**
		 * Gets the height of the image
		 */
		[[nodiscard]] int height() const;
		/**
		 * Gets the amount of channels of the image
		 */
		[[nodiscard]] int channels() const;

		[[nodiscard]] int size() const;

		[[nodiscard]] vk::Image image() const;
		[[nodiscard]] vk::ImageView view() const;
		[[nodiscard]] vk::DeviceMemory memory() const;
		[[nodiscard]] vk::Sampler sampler() const;
	private:
		int _width = 0;
		int _height = 0;
		int _channels = 0;
		unsigned char* _pixels = nullptr;

		vk::Image _image = nullptr;
		vk::ImageView _view = nullptr;
		vk::DeviceMemory _memory = nullptr;
		vk::Sampler _sampler = nullptr;

		void createImage();
	};
}
