#pragma once
#include <string>

namespace Tristeon
{
	namespace Data
	{
		class ImageBatch;

		/**
		 * Class for image loading
		 */
		class Image
		{
			friend ImageBatch;
		public:
			/**
			 * Creates a new instance of Image. Loads in a white image
			 */
			Image();
			/**
			 * Creates a new instance of image and loads in the image data based on the filepath.
			 * \param filePath The filepath of the image
			 */
			explicit Image(std::string filePath);
			/**
			 * Sets the filepath of the image
			 * \param filePath The filepath
			 */
			void setFilePath(std::string filePath);
			/**
			 * Gets the pixels of the image
			 * \return An unsigned char* describing the pixels
			 */
			unsigned char* getPixels() const;

			/**
			 * Gets the width of the image
			 * \return Returns an integer describing the width of the image
			 */
			int getWidth() const;
			/**
			 * Gets the height of the image
			 * \return Returns an integer describing the height of the image
			 */
			int getHeight() const;
			/**
			 * Gets the amount of channels of the image
			 * \return Returns an integer describing the amount of channels of the image
			 */
			int getChannels() const;

			int getSize() const;

			/**
			 * \return Returns a string with the current filepath of the image 
			 */
			std::string getFilePath() const;
		private:
			std::string filePath = "";
			int width = 0;
			int height = 0;
			int channels = 0;
			unsigned char* pixels = nullptr;
		};
	}
}
