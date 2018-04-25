#pragma once
#include <vector>
#include <functional>

namespace Tristeon
{
	namespace Misc
	{
		/**
		* \brief Simple delegate class which supports infinite parameters.
		* \tparam P All the parameters
		*/
		template <typename ... P>
		class Delegate
		{
		public:
			/**
			 * Calls all the functions added to this delegate.
			 * Requires the same parameters as this delegate has been defined with.
			 */
			void invoke(P... params);

			/**
			* Calls all the functions added to this delegate.
			* Requires the same parameters as this delegate has been defined with.
			*/
			void operator()(std::function<void(P...)> f);
			
			/**
			 * Adds a function to the delegate.
			 * The function must have the same parameters as this delegate has been defined with.
			 */
			void operator+=(std::function<void(P...)> f);

			/**
			 * Removes the given function from the delegate.
			 * The function must have the same parameters as this delegate.
			 * If this delegate doesn't contain the given function this will be ignored.
			 */
			void operator-=(std::function<void(P...)> f);

			/**
			 * Resets the delegate to only use the specific given function.
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
			//Call all the events
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
			//Add an event to our vector
			events.push_back(f);
		}

		template <typename ... P>
		void Delegate<P...>::operator-=(std::function<void(P...)> f)
		{
			//Try to find the given function
			auto itr = std::find(events.begin(), events.end(), events);

			//If itr == std::end, then we couldn't find our function. Otherwise: erase it.
			if (itr != std::end(events))
				events.erase(itr);
		}

		template <typename ... P>
		void Delegate<P...>::operator=(std::function<void(P...)> f)
		{
			//Reset our events
			events.clear();

			//Add the new event
			events.push_back(f);
		}

		template <typename ... P>
		void Delegate<P...>::clear()
		{
			//Reset our events
			events.clear();
		}
	}
}
