#pragma once
#include <map>
#include "Core/TObject.h"
#include <Core/JsonSerializer.h>
#include <filesystem>

namespace Tristeon::Data
{
	class Resources
	{
	public:
		enum class CacheBehaviour
		{
			SceneBased,
			Persistent
		};
		
		template<typename T>
		static T* jsonLoad(const std::string& path, CacheBehaviour behaviour = CacheBehaviour::Persistent)
		{
			if (path.empty())
				return nullptr;

			if (!std::filesystem::exists(path))
				return nullptr;

			auto* cachedResult = findInCache<T>(path, behaviour);
			if (cachedResult)
				return cachedResult;

			if (behaviour == CacheBehaviour::Persistent)
			{
				auto resource = Core::JsonSerializer::deserialize<T>(path);
				_persistentResources[path] = std::unique_ptr<T>(resource);
				return (T*)_persistentResources[path].get();
			}
			else
			{
				auto resource = Core::JsonSerializer::deserialize<T>(path);
				_sceneResources[path] = std::unique_ptr<T>(resource);
				return (T*)_sceneResources[path].get();
			}
		}

		///<summary>Expects T to contain a constructor that takes a string without having other parameters</summary>
		template<typename T>
		static T* assetLoad(const std::string& path, CacheBehaviour behaviour = CacheBehaviour::Persistent)
		{
			if (path.empty())
				return nullptr;

			if (!std::filesystem::exists(path))
				return nullptr;

			auto* cachedResult = findInCache<T>(path, behaviour);
			if (cachedResult)
				return cachedResult;

			if (behaviour == CacheBehaviour::Persistent)
			{
				_persistentResources[path] = std::make_unique<T>(path);
				return (T*)_persistentResources[path].get();
			}
			else
			{
				_persistentResources[path] = std::make_unique<T>(path);
				return (T*)_sceneResources[path].get();
			}
		}

		///<summary> Destroys all scene resources
		static void clearSceneCache()
		{
			_sceneResources.clear();
		}

		static void clearCache()
		{
			_sceneResources.clear();
			_persistentResources.clear();
		}
		

		static bool loaded(const std::string& path)
		{
			if (path.empty())
				return false;

			if (_persistentResources.find(path) != _persistentResources.end())
				return true;
			if (_sceneResources.find(path) != _sceneResources.end())
				return true;
			
			return false;
		}

	private:
		template<typename T>
		static T* findInCache(const std::string& path, CacheBehaviour behaviour)
		{
			if (_persistentResources.find(path) != _persistentResources.end())
				return (T*)_persistentResources[path].get();

			if (_sceneResources.find(path) != _sceneResources.end())
			{
				if (behaviour == CacheBehaviour::SceneBased)
					return (T*)_sceneResources[path].get();

				//Move cached object to persistent cache
				if (behaviour == CacheBehaviour::Persistent)
				{
					_persistentResources[path] = std::move(_sceneResources[path]);
					_sceneResources.erase(path);
					return (T*)_persistentResources[path].get();
				}
			}

			return nullptr;
		}
		
		static std::map<std::string, std::unique_ptr<Core::TObject>> _persistentResources;
		static std::map<std::string, std::unique_ptr<Core::TObject>> _sceneResources;
	};
}