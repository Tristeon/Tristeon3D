#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Misc/Console.h"

namespace Tristeon
{
	namespace Data
	{
		Image::Image()
		{
		}

		Image::Image(std::string filePath)
		{
			this->filePath = filePath;
		}

		void Image::setFilePath(std::string name)
		{
			filePath = name;
		}

		unsigned char* Image::getPixels()
		{
			//Load image
			stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

			//Replace pixels with white pixels instead
			if (!pixels)
			{
				stbi_image_free(pixels);
				pixels = stbi_load("Files/Textures/white.jpg", &width, &height, &channels, STBI_rgb_alpha);
			}

			return pixels;
		}

		void Image::freePixels(stbi_uc* pixels)
		{
			stbi_image_free(pixels);
		}

		int Image::getWidth() const
		{
			return width;
		}

		int Image::getHeight() const
		{
			return height;
		}

		int Image::getChannels() const
		{
			return channels;
		}
	}
}
