#pragma once
#include <vector>
#include <functional>
#include <algorithm> //Required in GNU GCC

namespace Tristeon
{
	namespace Misc
	{
		/**
		* Delegate is a callback class that wraps around a vector containing functions. 
		* You can add or remove functions from this list, or call invoke() to call all subscribed functions.
		*/
		template <typename ... P>
		class Delegate
		{
		public:
			/**
			 * Calls all the functions added to this delegate.
			 */
			void invoke(P... params);

			/**
			* Calls all the functions added to this delegate.
			*/
			void operator()(std::function<void(P...)> f);

			/**
			 * Adds a function to the delegate. The function must have the same template as the Delegate.
			 */
			void operator+=(std::function<void(P...)> f);

			/**
			 * Removes the given function from the delegate.
			 * The function must have the same parameters as the delegate.
			 * If this delegate doesn't contain the given function this will be ignored.
			 */
			void operator-=(std::function<void(P...)> f);

			/**
			 * Resets the delegate and assigns it to the specific given function.
			 */
			void operator=(std::function<void(P...)> f);

			/**
			 * Removes all functions from the delegate.
			 */
			void clear();

		private:
			std::vector<std::function<void(P...)>> events;
		};

		template <typename ... P>
		void Delegate<P...>::invoke(P... params)
		{
			for (unsigned int i = 0; i < events.size(); i++)
				events[i](params...);
		}

		template <typename ... P>
		void Delegate<P...>::operator()(std::function<void(P...)> f)
		{
			this->invoke(f);
		}

		template <typename ... P>
		void Delegate<P...>::operator+=(std::function<void(P...)> f)
		{
			events.push_back(f);
		}

		template <typename ... P>
		void Delegate<P...>::operator-=(std::function<void(P...)> f)
		{
			//Try to find the given function
			auto itr = std::find(events.begin(), events.end(), events);
			if (itr != std::end(events))
				events.erase(itr);
		}

		template <typename ... P>
		void Delegate<P...>::operator=(std::function<void(P...)> f)
		{
			events.clear();
			events.push_back(f);
		}

		template <typename ... P>
		void Delegate<P...>::clear()
		{
			events.clear();
		}
	}
}
