#include "geometry.h"
#include "linear_algebra.h"

vector<real, 2> quadratic_bezier_point(
	vector<real, 2> p1,
	vector<real, 2> p2,
	vector<real, 2> p3,
	real t)
{
	return vector<real, 2>(
		(1.0r - t) * ((1.0r - t) * p1.x + (2.0r * t) * p2.x) + (t * t) * p3.x,
		(1.0r - t) * ((1.0r - t) * p1.y + (2.0r * t) * p2.y) + (t * t) * p3.y);
}

geometry_path::geometry_path()
{
	orientation = face_orientation::counterclockwise;
}

void geometry_path::move(vector<real, 2> point)
{
	geometry_path_data unit;
	unit.type = geometry_path_unit::move;
	unit.p1 = point;
	data.push(unit);
}

void geometry_path::push_line(vector<real, 2> point)
{
	geometry_path_data unit;
	unit.type = geometry_path_unit::line;
	unit.p1 = point;
	data.push(unit);
}

void geometry_path::push_quadratic_arc(vector<real, 2> point1, vector<real, 2> point2)
{
	geometry_path_data unit;
	unit.type = geometry_path_unit::quadratic_arc;
	unit.p1 = point1;
	unit.p2 = point2;
	data.push(unit);
}
