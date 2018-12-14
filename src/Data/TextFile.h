#pragma once
#include <vector>
#include <fstream>

namespace Tristeon
{
	namespace Data
	{
		/**
		 * File reading mode used for text files
		 */
		enum FileMode
		{
			FM_Text = 0,
			FM_Binary = std::ios::binary
		};

		/**
		 * Utility class for loading text files.
		 */
		class TextFile final
		{
		public:
			/**
			 * Creates a default instance of TextFile with an empty filepath.
			 */
			explicit TextFile();
			
			/**
			 * Creates a new instance of TextFile with the given filename and mode
			 */
			TextFile(std::string filename, FileMode mode);

			/**
			 * Reads the file and returns it as a string.
			 * \exception std::runtime_error Failed to open file at filepath <path>
			 */
			std::string readAllString() const;
			/**
			 * Reads the file and returns it as a char vector
			 * \exception std::runtime_error Failed to open file at filepath <path>
			 */
			std::vector<char> readAllVector() const;

			/**
			 * The path of the text file
			 */
			std::string filePath;
			/**
			 * The mode of the file (binary/text)
			 */
			FileMode filemode;
		};
	}
}
