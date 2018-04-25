#pragma once
#include <memory>
#include <vector>

namespace Tristeon
{
	/**
	 * \brief Tristeon::Vector is an extension of the standard std::vector, implementing functions such as contains() and remove()
	 * \tparam _Ty
	 * \tparam _Alloc
	 */
	template<class _Ty, class _Alloc = std::allocator<_Ty>>
	class vector : public std::vector<_Ty, _Alloc>
	{
		/**
		 * \brief A shortcut reference to the base class
		 */
		typedef std::vector<_Ty, _Alloc> base;

	public:
		/**
		 * \brief Initializes an empty vector
		 * \param _Al 
		 */
		explicit vector(const _Alloc& _Al = _Alloc());
		/**
		 * \brief Initializes a vector with the given initializer list
		 * \param _Ilist The initializer list
		 * \param _Al 
		 */
		vector(std::initializer_list<_Ty> _Ilist, const _Alloc& _Al = _Alloc());
		/**
		* \brief Checks if the given parameter _Ty is contained inside of this vector
		* \param t The item to be tested
		* \return Returns true or false, based on if t is contained in this vector or not
		*/
		bool contains(_Ty t);
		/**
		* \brief Removes item t from the vector
		* \param t The item to be removed
		*/
		void remove(_Ty t);
	};

	template <class _Ty, class _Alloc>
	void vector<_Ty, _Alloc>::remove(_Ty t)
	{
		base::erase(std::remove(base::begin(), base::end(), t), base::end());
	}

	template <class _Ty, class _Alloc>
	vector<_Ty, _Alloc>::vector(const _Alloc& _Al) : base(_Al)
	{
		//Empty
	}

	template <class _Ty, class _Alloc>
	vector<_Ty, _Alloc>::vector(std::initializer_list<_Ty> _Ilist, const _Alloc& _Al) : base(_Ilist, _Al)
	{
		//Empty
	}

	template <class _Ty, class _Alloc>
	bool vector<_Ty, _Alloc>::contains(_Ty t)
	{
		return std::find(base::begin(), base::end(), t) != base::end();
	}
}