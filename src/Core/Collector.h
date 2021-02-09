#pragma once
#include <vector>

namespace Tristeon
{
	/**
	 * Collector is a handy tool for collecting classes of a certain type.
	 *
	 * Classes looking to be collected should add themselves in the constructor using Collector<Type>::add(this) and remove themselves in the deconstructor Collector<Type>::remove(this).
	 * You may then use Collector<Type>::all() to receive all collected objects.
	 */
	template<typename T>
	class Collector
	{
	public:
		/**
		 * Adds an object to the collector.
		 */
		static void add(T* t);

		/**
		 * Removes the object from the collector.
		 */
		static void remove(T* t);

		/**
		 * Returns a list with all the collected objects.
		 */
		static std::vector<T*> all();

	private:
		static std::vector<T*> _collection;
	};

	template<typename T>
	std::vector<T*> Collector<T>::_collection;

	template <typename T>
	void Collector<T>::add(T* t)
	{
		_collection.push_back(t);
	}

	template <typename T>
	void Collector<T>::remove(T* t)
	{
		_collection.erase(std::remove(_collection.begin(), _collection.end(), t));
	}

	template <typename T>
	std::vector<T*> Collector<T>::all()
	{
		return _collection;
	}
}