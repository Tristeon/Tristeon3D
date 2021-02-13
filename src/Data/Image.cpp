#include "Image.h"
#include <stb_image.h>

namespace Tristeon::Data
{
	Image::Image(const std::string& filePath)
	{
		_pixels = stbi_load(filePath.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
	}

	unsigned char* Image::pixels() const
	{
		return _pixels;
	}

	int Image::width() const
	{
		return _width;
	}

	int Image::height() const
	{
		return _height;
	}

	int Image::channels() const
	{
		return _channels;
	}

	int Image::size() const
	{
		return _width * _height * _channels;
	}
}