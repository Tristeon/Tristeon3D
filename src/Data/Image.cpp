#include "Image.h"
#include "ImageBatch.h"

namespace Tristeon
{
	namespace Data
	{
		Image::Image()
		{
		}

		Image::Image(std::string filePath)
		{
			Image const img = ImageBatch::getImage(filePath);
			pixels = img.pixels;
			height = img.height;
			width = img.width;
			channels = img.channels;
			filePath = img.filePath;
		}

		void Image::setFilePath(std::string name)
		{
			filePath = name;
		}

		unsigned char* Image::getPixels() const
		{
			return pixels;
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

		int Image::getSize() const
		{
			return width * height * channels;
		}

		std::string Image::getFilePath() const
		{
			return filePath;
		}
	}
}
