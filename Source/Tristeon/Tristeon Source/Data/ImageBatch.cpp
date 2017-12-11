#include "ImageBatch.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Data
	{
		//Static
		std::map<std::string, Image> ImageBatch::cachedImages;

		Image ImageBatch::getImage(std::string path)
		{
			//Try to return a cached image
			if (cachedImages.find(path) != cachedImages.end())
				return cachedImages[path];

			//Load new
			if (!load(path))
			{
				Misc::Console::warning("ImageBatch: Failed to load image [" + path + "]. Returning an image filled with white pixels");
				return getImage("Files/Textures/white.jpg");
			}

			//We did load, return loaded
			return cachedImages[path];
		}

		bool ImageBatch::load(std::string path)
		{
			//Clear old cached image
			if (cachedImages.find(path) != cachedImages.end())
				stbi_image_free(cachedImages[path].pixels);

			//Load image
			Image img;
			img.filePath = path;
			img.pixels = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, STBI_rgb_alpha);

			if (img.pixels && img.width != 0 && img.height != 0)
			{
				//Store if loading was succesful
				cachedImages[path] = img;
				return true;
			}
			return false;
		}

		void ImageBatch::unload(std::string path)
		{
			//Can't unload an image we don't know
			if (cachedImages.find(path) == cachedImages.end())
				return;

			//Free, and remove
			stbi_image_free(cachedImages[path].pixels);
			cachedImages.erase(path);
		}

		void ImageBatch::unloadAll()
		{
			//Unload all
			for (const auto p : cachedImages)
			{
				stbi_image_free(p.second.pixels);
			}
		}
	}
}
