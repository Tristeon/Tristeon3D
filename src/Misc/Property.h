#pragma once

/**
 * RWProperty provides a [READ AND WRITE] interface for classes to define public fields with custom get/set functionality.
 * RWProperty is usually not used directly, but rather through the <Property>, <Get_Property> and <Set_Property> macros defined in this header.
 */
template <typename C, typename T>
class RWProperty
{
	friend C;
public:
	typedef T(C::*Getter_t)();
	typedef void (C::*Setter_t)(T);

	RWProperty(C* instance, Getter_t getter, Setter_t setter) : m_instance(instance), m_getter(getter), m_setter(setter) {}

	T get() { return (m_instance->*m_getter)(); }
	operator T() const { return (this->m_instance->*this->m_getter)(); }

	void set(T value) { (m_instance->*m_setter)(value); }
	RWProperty<C, T>& operator=(T value) { (this->m_instance->*this->m_setter)(value); return *this; }

private:
	C* const m_instance;
	const Getter_t m_getter;
	const Setter_t m_setter;
};

/**
 * RProperty provides a [READONLY] interface for classes to define public fields with custom get functionality.
 * RProperty is usually not used directly, but rather through the <Property> and <Get_Property> macros defined in this header.
 */
template <typename C, typename T>
class RProperty
{
	friend C;
public:
	typedef T(C::*Getter_t)();
	typedef void (C::*Setter_t)(T);

	RProperty(C* instance, Getter_t getter) : m_instance(instance), m_getter(getter) {}

	T get() { return (m_instance->*m_getter)(); }
	operator T() const { return (this->m_instance->*this->m_getter)(); }

private:
	C* const m_instance;
	const Getter_t m_getter;
};

/**
 * SimpleRProperty is a value wrapper with public get functionality. 
 * It befriends the first template parameter and wraps a value of the second.
 */
template <typename C, typename T>
class SimpleRProperty
{
	friend C;
public:
	T get() { return value; }
	operator T() const { return value; }

private:
	T value;
};

/**
 * WProperty provides a [WRITEONLY] interface for classes to define public fields with custom set functionality.
 * WProperty is usually not used directly, but rather through the <Property> and <Set_Property> macros defined in this header.
 */
template <typename C, typename T>
class WProperty
{
	friend C;
public:
	typedef T(C::*Getter_t)();
	typedef void (C::*Setter_t)(T);

	WProperty(C* instance, Setter_t setter) : m_instance(instance), m_setter(setter) {}

	void set(T value) { (m_instance->*m_setter)(value); }
	RWProperty<C, T>& operator=(T value) { (this->m_instance->*this->m_setter)(value); return *this; }

private:
	C* const m_instance;
	const Setter_t m_setter;
};


//Macros wrapping the property classes for ease of use

#define SimpleReadOnlyProperty(CLASS, NAME, TYPE) SimpleRProperty<CLASS, TYPE> NAME = {};

#define Property(CLASS, NAME, TYPE)	RWProperty<CLASS, TYPE> NAME = { this, &CLASS::get_##NAME, &CLASS::set_##NAME }; \
    typedef TYPE property__tmp_type_##NAME;

#define PropertyNestedValue(CLASS, NAME, TYPE, VALUE) Property(CLASS, NAME, TYPE); GetProperty(NAME) { return VALUE; } SetProperty(NAME) { ##VALUE = value; } 

#define ReadOnlyProperty(CLASS, NAME, TYPE) RProperty<CLASS, TYPE> NAME = { this, &CLASS::get_##NAME }; \
    typedef TYPE property__tmp_type_##NAME;

#define WriteOnlyProperty(CLASS, NAME, TYPE) WProperty<CLASS, TYPE> NAME() = { this, &CLASS::set_##NAME }; \
    typedef TYPE property__tmp_type_##NAME;

#define GetProperty(NAME) property__tmp_type_##NAME get_##NAME()
#define SetProperty(NAME) void set_##NAME(property__tmp_type_##NAME value)
