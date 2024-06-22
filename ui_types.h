#pragma once
#include "real.h"
#include "vector.h"

enum struct adaptive_size_type
{
	absolute,
	relative,
	autosize
};

template<typename value_type> struct adaptive_size
{
	adaptive_size_type type;
	value_type value;

	adaptive_size() {}

	adaptive_size(adaptive_size_type type, value_type value)
		: type(type), value(value) {}
};

enum struct horizontal_align
{
	left,
	center,
	right
};

enum struct vertical_align
{
	top,
	center,
	bottom
};

enum struct flow_axis
{
	x,
	y
};

enum struct flow_offset
{
	left,
	right
};
