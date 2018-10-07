#pragma once
#include <memory>

/**
 * Should always be called in global namespace. Declares the make_unique function for unique pointers.
 */
#define TRISTEON_UNIQUE_ACCESS_DECL() \
	namespace std \
	{ \
		template<class _Ty, class... _Types, enable_if_t<!is_array_v<_Ty>, int>> \
		_NODISCARD unique_ptr<_Ty> make_unique(_Types&&... _Args); \
	}

/**
 * Declares friendship to unique_ptr, its make_unique function and the core engine class. 
 * Requires TRISTEON_UNIQUE_ACCESS_DECL() to be called before the class declaration, and in global namespace.
 */
#define TRISTEON_UNIQUE_ACCESS(t) \
		friend std::unique_ptr<t>; \
		friend std::unique_ptr<t>::deleter_type; \
		friend Core::Engine; \
		\
		template<class _Ty, class... _Types, std::enable_if_t<!std::is_array_v<_Ty>, int>> \
		_NODISCARD friend std::unique_ptr<_Ty> std::make_unique(_Types&&... _Args);