#include "TextFile.h"
#include <vector>
#include "Misc/Console.h"

namespace Tristeon::Data
{
	TextFile::TextFile()
	{
		//Standard is text
		filemode = FileMode::Text;
		filePath = "";
	}

	TextFile::TextFile(const std::string& filename, const FileMode& mode)
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
		auto file = std::ifstream(filePath, std::ios::ate | (std::ios::openmode)filemode);

		//Failure
		if (!file.is_open())
			Misc::Console::error("Failed to open file " + filePath);

		//Get filesize and create a vector with that given size
		const auto fileSize = static_cast<size_t>(file.tellg());
		auto data = std::vector<char>(fileSize);

		//move to the end, read backwards
		file.seekg(0);
		file.read(data.data(), fileSize);

		//Close file
		file.close();

		//Return
		return data;
	}
}
