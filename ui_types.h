#pragma once
#include "real.h"
#include "vector.h"

enum struct ui_size_type
{
	absolute,
	relative,
	autosize
};

struct ui_size
{
	ui_size_type type;
	real value;

	ui_size() {}

	ui_size(ui_size_type type, real value)
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
