#pragma once
#include <string>

namespace Tristeon
{
	namespace Data
	{
		/**
		 * \brief Class for image handling.
		 */
		class Image
		{
		public:
			/**
			 * \brief Creates a new instance of Image, filepath still has to be set
			 */
			Image();
			/**
			 * \brief Creates a new instance of image with the given filepath
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
			unsigned char* getPixels();
			/**
			 * \brief Deallocates the memory occupied by the loaded image;
			 * \param pixels The pixels obtained by getPixels()
			 */
			void freePixels(unsigned char* pixels);

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
		};
	}
}
