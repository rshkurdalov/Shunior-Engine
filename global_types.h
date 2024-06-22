#pragma once
#include <new>

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef char char8;
typedef char16_t char16;
typedef char32_t char32;
typedef unsigned char byte;

template<typename value_type> struct key
{
	value_type key_value;

	key(value_type value)
	{
		key_value = value;
	}
	
	bool operator<(const key &value) const
	{
		return key_value < value.key_value;
	}
};

template<typename base_type> struct indefinite
{
	void *addr;

	indefinite() {}
	indefinite(void *addr) : addr(addr) {}
};

template<typename base_type> struct handleable
{
	indefinite<base_type> object;
	base_type *core;

	handleable() {}
	handleable(void *object, base_type *core)
		: object(object), core(core) {}
};

template<typename application> struct utility
{
	void copy(const application &source, application *target)
	{
		*target = source;
	}
};
