#pragma once
#include "real.h"
#include "array.h"
#include "vector.h"

vector<real, 2> quadratic_bezier_point(
	vector<real, 2> p1,
	vector<real, 2> p2,
	vector<real, 2> p3,
	real t);

enum struct geometry_path_unit
{
	move,
	line,
	quadratic_arc
};

struct geometry_path_data
{
	geometry_path_unit type;
	vector<real, 2> p1;
	vector<real, 2> p2;
};

enum struct face_orientation
{
	counterclockwise,
	clockwise
};

struct geometry_path
{
	array<geometry_path_data> data;
	face_orientation orientation;

	geometry_path();
	void move(vector<real, 2> point);
	void push_line(vector<real, 2> point);
	void push_quadratic_arc(vector<real, 2> point1, vector<real, 2> point2);
};

template<typename value_type> struct rectangle
{
	vector<value_type, 2> position;
	vector<value_type, 2> extent;

	rectangle() {}

	rectangle(vector<value_type, 2> position, vector<value_type, 2> extent) 
		: position(position), extent(extent) {}

	void push_path(geometry_path *path)
	{
		path->move(vector<real, 2>(real(position.x), real(position.y)));
		path->push_line(vector<real, 2>(real(position.x + extent.x), real(position.y)));
		path->push_line(vector<real, 2>(real(position.x + extent.x), real(position.y + extent.y)));
		path->push_line(vector<real, 2>(real(position.x), real(position.y + extent.y)));
		path->push_line(vector<real, 2>(real(position.x), real(position.y)));
	}
};
