#pragma once
#include <vector>
#include <fstream>

namespace Tristeon
{
	namespace Data
	{
		/**
		 * \brief File reading mode used for text files
		 */
		enum FileMode
		{
			FM_Text = 0,
			FM_Binary = std::ios::binary
		};

		/**
		 * \brief Utility class for loading text files.
		 */
		class TextFile final
		{
		public:
			/**
			 * \brief Creates a new instance of TextFile. Filetype still needs to be set, filemode is set to Text.
			 */
			explicit TextFile();
			
			/**
			 * \brief Creates a new instance of TextFile with the given filename and mode
			 */
			TextFile(std::string filename, FileMode mode);

			/**
			 * \brief Reads the file and returns it as a string
			 * \return A string with the text data
			 */
			std::string readAllString() const;
			/**
			 * \brief Reads the file and returns it as a char vector
			 * \return A char vector with the text data
			 */
			std::vector<char> readAllVector() const;

			/**
			 * \brief The path of the text file
			 */
			std::string filePath;
			/**
			 * \brief The mode of the file (binary/text)
			 */
			FileMode filemode;
		};
	}
}
