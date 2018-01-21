#pragma once
#include <queue>
#include "vector.h"

template<typename Resource>
class ObjectPool
{
public:
	//There is no point(er haha) in using value types in a objectpool as they'll be copied around anyways
	static_assert(std::is_pointer<Resource>::value, "ObjectPool expects a pointer type!");
	~ObjectPool();

	Resource get();
	void release(Resource resource);
	void clearUnused();
	void reset();
private:
	std::queue<Resource> available;
	Tristeon::vector<Resource> used;
};

template <typename Resource>
ObjectPool<Resource>::~ObjectPool()
{
	reset();
}

template <typename Resource>
Resource ObjectPool<Resource>::get()
{
	if (available.size() == 0)
	{
		Resource r = new std::remove_pointer_t<Resource>();
		available.push(r);
		Tristeon::Misc::Console::warning("Allocating new resource!");
	}

	Resource r = available.front();
	available.pop();
	used.push_back(r);
	std::cout << "Returning resource. In use: " + std::to_string(used.size()) + ", Available: " + std::to_string(available.size()) << std::endl;
	return r;
}

template <typename Resource>
void ObjectPool<Resource>::release(Resource resource)
{
	if (resource == nullptr)
		return;

	used.remove(resource);
	available.push(resource);
	std::cout << "Released resource. In use: " + std::to_string(used.size()) + ", Available: " + std::to_string(available.size()) << std::endl;
}

template <typename Resource>
void ObjectPool<Resource>::clearUnused()
{
	while (!available.empty())
	{
		Resource r = available.front();
		available.pop();
		std::cout << "Removing unused resource. " + std::to_string(available.size()) + " remaining." << std::endl;
		delete r;
	}
}

template <typename Resource>
void ObjectPool<Resource>::reset()
{
	clearUnused();
	for (size_t i = 0; i < used.size(); i++)
		delete used[i];
	used.clear();
}
