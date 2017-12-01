#include "TextFile.h"
#include <vector>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Data
	{
		TextFile::TextFile()
		{
			//Standard is text
			filemode = FM_Text;
			filePath = "";
		}

		TextFile::TextFile(std::string filename, FileMode mode)
		{
			this->filemode = mode;
			this->filePath = filename;
		}

		std::string TextFile::readAllString() const
		{
			return readAllVector().data();
		}

		std::vector<char> TextFile::readAllVector() const
		{
			//Open file
			std::ifstream file = std::ifstream(filePath, std::ios::ate | filemode);

			//Failure
			if (!file.is_open())
				Misc::Console::error("Failed to open file " + filePath);

			//Get filesize and create a vector with that given size
			const size_t fileSize = static_cast<size_t>(file.tellg());
			std::vector<char> data = std::vector<char>(fileSize);

			//move to the end, read backwards
			file.seekg(0);
			file.read(data.data(), fileSize);

			//Close file
			file.close();

			//Return
			return data;
		}
	}
}
