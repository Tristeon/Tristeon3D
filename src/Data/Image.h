#pragma once
#include <string>

namespace Tristeon
{
	namespace Data
	{
		class ImageBatch;

		/**
		 * \brief Class for image handling.
		 */
		class Image
		{
			friend ImageBatch;
		public:
			/**
			 * \brief Creates a new instance of Image. Loads in a white image
			 */
			Image();
			/**
			 * \brief Creates a new instance of image and loads in the image data based on the filepath.
			 * \param filePath The filepath of the image
			 */
			explicit Image(std::string filePath);
			/**
			 * \brief Sets the filepath of the image
			 * \param filePath The filepath
			 */
			void setFilePath(std::string filePath);
			/**
			 * \brief Gets the pixels of the image
			 * \return An unsigned char* describing the pixels
			 */
			unsigned char* getPixels() const;

			/**
			 * \brief Gets the width of the image
			 * \return Returns an integer describing the width of the image
			 */
			int getWidth() const;
			/**
			 * \brief Gets the height of the image
			 * \return Returns an integer describing the height of the image
			 */
			int getHeight() const;
			/**
			 * \brief Gets the amount of channels of the image
			 * \return Returns an integer describing the amount of channels of the image
			 */
			int getChannels() const;

			int getSize();

			/**
			 * \return Returns a string with the current filepath of the image 
			 */
			std::string getFilePath() const;
		private:
			/**
			 * \brief The filepath of the image
			 */
			std::string filePath = "";
			/**
			 * \brief The width of the image
			 */
			int width = 0;
			/**
			 * \brief The height of the image
			 */
			int height = 0;
			/**
			 * \brief The channel count of the image
			 */
			int channels = 0;

			/**
			 * \brief The pixels of the image
			 */
			unsigned char* pixels = nullptr;
		};
	}
}
