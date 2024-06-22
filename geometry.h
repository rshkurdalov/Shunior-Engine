#pragma once
#include "real.h"
#include "array.h"
#include "vector.h"

vector<real, 2> quadratic_bezier_point(
	vector<real, 2> p1,
	vector<real, 2> p2,
	vector<real, 2> p3,
	real t);

vector<real, 2> elliptic_arc_point(
	vector<real, 2> center,
	real rx,
	real ry,
	real angle);

template<typename value_type> struct rectangle
{
	vector<value_type, 2> position;
	vector<value_type, 2> extent;

	rectangle() {}

	rectangle(vector<value_type, 2> position, vector<value_type, 2> extent) 
		: position(position), extent(extent) {}

	template<typename right_type> rectangle(rectangle<right_type> &rect)
	{
		position.x = value_type(rect.position.x);
		position.y = value_type(rect.position.y);
		extent.x = value_type(rect.extent.x);
		extent.y = value_type(rect.extent.y);
	}
};

template<typename value_type> struct rounded_rectangle
{
	rectangle<value_type> rect;
	value_type rx;
	value_type ry;

	rounded_rectangle() {}

	rounded_rectangle(rectangle<value_type> rect, value_type rx, value_type ry)
		: rect(rect), rx(rx), ry(ry) {}
};

enum struct geometry_path_unit
{
	move,
	line,
	quadratic_arc,
	elliptic_arc
};

struct geometry_path_data
{
	geometry_path_unit type;
	vector<real, 2> p1;
	vector<real, 2> p2;
	real rx;
	real ry;
	real begin_angle;
	real end_angle;
	real rotation;
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
	void push_elliptic_arc(vector<real, 2> point, real radius_ratio, real begin_angle, real end_angle, real rotation);
	void push_rectangle(rectangle<real> rect);
	void push_rounded_rectangle(rounded_rectangle<real> rrect);
};
