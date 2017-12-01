#pragma once

namespace Tristeon
{
	namespace Misc
	{
		/**
		 * Includes multiple defines which wrap around the C++ property to make its usage more like C# properties.
		 */

		//Standard property
		#define Property(t,n)  __declspec( property (put = property__set_##n, get = property__get_##n)) t n; \
			typedef t property__tmp_type_##n

		//Read Only
		#define ReadOnlyProperty(t,n) __declspec( property (get = property__get_##n) ) t n;\
			typedef t property__tmp_type_##n

		//Write Only
		#define WriteOnlyProperty(t,n) __declspec( property (put = property__set_##n) ) t n;\
			typedef t property__tmp_type_##n

		//Get function
		#define GetProperty(n) property__tmp_type_##n property__get_##n() 
		#define GetPropertyConst(n) property__tmp_type_##n property__get_##n() const

		//Set function
		#define SetProperty(n) void property__set_##n(const property__tmp_type_##n& value)
	}
}