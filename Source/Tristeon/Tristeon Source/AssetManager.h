#pragma once
#include <map>
namespace Tristeon
{
	class AssetManager
	{
	public:
		static std::string getPath(std::string GUID)
		{
			return assetMap[GUID];
		}

		static void setGUID(std::string GUID, std::string filepath)
		{
			assetMap[GUID] = filepath;
		}

	private:
		static std::map<std::string,std::string> assetMap;
	};
}