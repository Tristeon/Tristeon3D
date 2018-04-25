#pragma once
#include <queue>
#include "vector.h"

/**
 * \brief Objectpool is a class that manages a set of initialized objects kept ready to use – a "pool" – rather than allocating and destroying them on demand. 
 * The user of the pool can request an object from the pool using get() and perform operations on the returned object. 
 * When the client has finished, they can return the object to the pool using release(), rather than destroying it. 
 * \tparam Resource The type of resource. Has to be a pointer.
 */
template<typename Resource>
class ObjectPool
{
public:
	//There is no point(er haha) in using value types in a objectpool
	static_assert(std::is_pointer<Resource>::value, "ObjectPool expects a pointer type!");

	/**
	 * \brief Calls reset()
	 */
	~ObjectPool();

	/**
	 * \brief Returns a resource from the pool. Creates a new resource if no resources are available
	 */
	Resource get();

	/**
	 * \brief Puts the given resource back into the pool
	 */
	void release(Resource resource);
	/**
	 * \brief Deallocates all resources that aren't currently in use
	 */
	void clearUnused();
	/**
	 * \brief Deallocates all resources created by the pool. Be careful with using this function as any references to pooled objects will turn invalid!
	 */
	void reset();
private:
	/**
	 * \brief The currently unused (available) pool objects.
	 */
	std::queue<Resource> available;
	/**
	 * \brief A list keeping track of the actively used pool objects.
	 */
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
	//Create a new resource if we're out of available resources
	if (available.size() == 0)
	{
		Resource r = new std::remove_pointer_t<Resource>();
		available.push(r);
	}

	//Return the first available resource
	Resource r = available.front();
	available.pop();
	used.push_back(r);
	return r;
}

template <typename Resource>
void ObjectPool<Resource>::release(Resource resource)
{
	//We don't have null resources
	if (resource == nullptr)
		return;

	used.remove(resource);
	available.push(resource);
}

template <typename Resource>
void ObjectPool<Resource>::clearUnused()
{
	//Clear the available objects queue
	while (!available.empty())
	{
		Resource r = available.front();
		available.pop();
		delete r;
	}
}

template <typename Resource>
void ObjectPool<Resource>::reset()
{
	clearUnused();

	//Clear all the objects currently in use
	for (size_t i = 0; i < used.size(); i++)
		delete used[i];
	used.clear();
}
