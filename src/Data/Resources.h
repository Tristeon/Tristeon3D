#pragma once
#include <map>
#include "Core/TObject.h"
#include <Core/JsonSerializer.h>
#include <filesystem>

namespace Tristeon
{
	class Resources
	{
	public:
		template<typename T>
		static T* jsonLoad(const std::string& path)
		{
			if (path.empty())
				return nullptr;

			if (!std::filesystem::exists(path))
				return nullptr;

			if (_loadedResources.find(path) != _loadedResources.end())
				return (T*)_loadedResources[path].get();

			auto resource = JsonSerializer::deserialize<T>(path);
			_loadedResources[path] = std::move(resource);
			return (T*)_loadedResources[path].get();
		}

		///<summary>Expects T to contain a constructor that takes a string without having other parameters</summary>
		template<typename T>
		static T* assetLoad(const std::string& path)
		{
			if (path.empty())
				return nullptr;

			if (!std::filesystem::exists(path))
				return nullptr;

			if (_loadedResources.find(path) != _loadedResources.end())
				return static_cast<T*>(_loadedResources[path].get());

			_loadedResources[path] = std::make_unique<T>(path);
			return static_cast<T*>(_loadedResources[path].get());
		}

		static bool loaded(const std::string& path)
		{
			if (path.empty())
				return false;

			if (_loadedResources.find(path) != _loadedResources.end())
				return true;

			return false;
		}

	private:
		static std::map<std::string, std::unique_ptr<Core::TObject>> _loadedResources;
	};
}