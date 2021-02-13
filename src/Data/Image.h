#pragma once
#include <string>

namespace Tristeon::Data
{
	/**
	 * Class for image loading
	 */
	class Image
	{
	public:
		/**
		 * Creates a new instance of image and loads in the image data based on the filepath.
		 * \param filePath The filepath of the image
		 */
		explicit Image(const std::string& filePath);

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
	private:
		int _width = 0;
		int _height = 0;
		int _channels = 0;
		unsigned char* _pixels = nullptr;
	};
}
