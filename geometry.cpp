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

vector<real, 2> elliptic_arc_point(
	vector<real, 2> center,
	real rx,
	real ry,
	real angle)
{
	vector<real, 2> n = angle_normal(angle);
	return center + vector<real, 2>(rx * n.x, ry * n.y);
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

void geometry_path::push_elliptic_arc(vector<real, 2> point, real radius_ratio, real begin_angle, real end_angle, real rotation)
{
	if(data.size == 0) return;
	vector<real, 2> n1 = angle_normal(begin_angle), n2 = angle_normal(end_angle),
		nr = angle_normal(rotation), p1;
	if(data.addr[data.size - 1].type == geometry_path_unit::quadratic_arc)
		p1 = data.addr[data.size - 1].p2;
	else p1 = data.addr[data.size - 1].p1;
	geometry_path_data unit;
	unit.type = geometry_path_unit::elliptic_arc;
	real d1 = (nr.x + nr.y * nr.y / nr.x) * (n1.x - n2.x),
		d2 = nr.x * (n2.y - n1.y) - nr.y * nr.y / nr.x * (n1.y - n2.y);
	unit.rx = (p1.y * nr.y / nr.x - point.y * nr.y / nr.x - point.x + p1.x) / d1;
	unit.ry = (point.y - p1.y - point.x * nr.y / nr.x + p1.x * nr.y / nr.x) / d2;
	if(abs(d1) < 0.0001r)
		unit.rx = unit.ry / radius_ratio;
	else if(abs(d2) < 0.0001r)
		unit.ry = unit.rx * radius_ratio;
	unit.p2.x = p1.x - unit.rx * n1.x * nr.x + unit.ry * n1.y * nr.y;
	unit.p2.y = p1.y - unit.rx * n1.x * nr.y - unit.ry * n1.y * nr.x;
	unit.begin_angle = begin_angle;
	unit.end_angle = end_angle;
	unit.rotation = rotation;
	matrix<real, 3, 3> transform = rotate_matrix(rotation, unit.p2);
	unit.p1 = elliptic_arc_point(unit.p2, unit.rx, unit.ry, unit.end_angle);
	matrix<real, 1, 3> p = vector<real, 3>(unit.p1.x, unit.p1.y, 1.0r) * transform;
	unit.p1 = vector<real, 2>(p.m[0][0], p.m[0][1]);
	data.push(unit);
}

void geometry_path::push_rectangle(rectangle<real> rect)
{
	move(vector<real, 2>(rect.position.x, rect.position.y));
	push_line(vector<real, 2>(rect.position.x + rect.extent.x, rect.position.y));
	push_line(vector<real, 2>(rect.position.x + rect.extent.x, rect.position.y + rect.extent.y));
	push_line(vector<real, 2>(rect.position.x, rect.position.y + rect.extent.y));
	push_line(vector<real, 2>(rect.position.x, rect.position.y));
}

void geometry_path::push_rounded_rectangle(rounded_rectangle<real> rrect)
{
	move(vector<real, 2>(rrect.rect.position.x + rrect.rx, rrect.rect.position.y));
	push_line(vector<real, 2>(rrect.rect.position.x + rrect.rect.extent.x - rrect.rx, rrect.rect.position.y));
	push_elliptic_arc(vector<real, 2>(rrect.rect.position.x + rrect.rect.extent.x, rrect.rect.position.y + rrect.ry),
		rrect.ry / rrect.rx, 0.75r, 0.0r, 0.0r);
	push_line(vector<real, 2>(rrect.rect.position.x + rrect.rect.extent.x, rrect.rect.position.y + rrect.rect.extent.y - rrect.ry));
	push_elliptic_arc(vector<real, 2>(rrect.rect.position.x + rrect.rect.extent.x - rrect.rx, rrect.rect.position.y + rrect.rect.extent.y),
		rrect.ry / rrect.rx, 0.0r, 0.25r, 0.0r);
	push_line(vector<real, 2>(rrect.rect.position.x + rrect.rx, rrect.rect.position.y + rrect.rect.extent.y));
	push_elliptic_arc(vector<real, 2>(rrect.rect.position.x, rrect.rect.position.y + rrect.rect.extent.y - rrect.ry),
		rrect.ry / rrect.rx, 0.25r, 0.5r, 0.0r);
	push_line(vector<real, 2>(rrect.rect.position.x, rrect.rect.position.y + rrect.ry));
	push_elliptic_arc(vector<real, 2>(rrect.rect.position.x + rrect.rx, rrect.rect.position.y),
		rrect.ry / rrect.rx, 0.5r, 0.75r, 0.0r);
}
