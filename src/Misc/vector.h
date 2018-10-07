#pragma once
#include <memory>
#include <vector>

namespace Tristeon
{
	/**
	 * Tristeon::Vector is an extension of the standard std::vector, simplifying functionality such as contains() and remove()
	 */
	template<class _Ty, class _Alloc = std::allocator<_Ty>>
	class vector : public std::vector<_Ty, _Alloc>
	{
		typedef std::vector<_Ty, _Alloc> base;
	public:
		explicit vector(const _Alloc& _Al = _Alloc());
		vector(std::initializer_list<_Ty> _Ilist, const _Alloc& _Al = _Alloc());

		/**
		* Checks if the given parameter _Ty is contained inside of this vector
		*/
		bool contains(_Ty t);
		/**
		* Removes item t from the vector
		*/
		void remove(_Ty t);
	};

	template <class _Ty, class _Alloc>
	void vector<_Ty, _Alloc>::remove(_Ty t)
	{
		base::erase(std::remove(base::begin(), base::end(), t), base::end());
	}

	template <class _Ty, class _Alloc>
	vector<_Ty, _Alloc>::vector(const _Alloc& _Al) : base(_Al) { }

	template <class _Ty, class _Alloc>
	vector<_Ty, _Alloc>::vector(std::initializer_list<_Ty> _Ilist, const _Alloc& _Al) : base(_Ilist, _Al) { }

	template <class _Ty, class _Alloc>
	bool vector<_Ty, _Alloc>::contains(_Ty t)
	{
		return std::find(base::begin(), base::end(), t) != base::end();
	}
}