#include "linear_algebra.h"

vector<real, 2> angle_normal(real value)
{
	if(value >= 1.0r) value.integer = 0;
	else if(value < 0.0r)
	{
		if(value.fraction == 0) value = 0.0r;
		else value = 1.0r - real(0, value.fraction);
	}
	if(value == 0.0r) return vector<real, 2>(1.0r, 0.0r);
	else if(value == 0.25r) return vector<real, 2>(0.0r, 1.0r);
	else if(value == 0.5r) return vector<real, 2>(-1.0r, 0.0r);
	else if(value == 0.75r) return vector<real, 2>(0.0r, -1.0r);
	vector<real, 2> l(0.0r, 1.0r), r(1.0r, 0.0r), n;
	real v = value - 0.25r * floor(value / 0.25r), a, al = 0.25r, ar = 0.0r;
	while(true)
	{
		n = vector_normal(l + r);
		a = 0.5r * (al + ar);
		if(ar + 0.000000001r == al) break;
		if(a < v)
		{
			r = n;
			ar = a;
		}
		else
		{
			l = n;
			al = a;
		}
	}
	if(value >= 0.75r) n = vector<real, 2>(n.y, -n.x);
	else if(value >= 0.5r) n = -n;
	else if(value >= 0.25r) n = vector<real, 2>(-n.y, n.x);
	return n;
}

real vector_angle(vector<real, 2> value)
{
	if(value.x == 0.0r)
	{
		if(value.y > 0.0r) return 0.25r;
		else return 0.75r;
	}
	else if(value.y == 0.0r)
	{
		if(value.x > 0.0r) return 0.0r;
		else return 0.5r;
	}
	value = vector_normal(value);
	vector<real, 2> v, l(0.0r, 1.0r), r(1.0r, 0.0r), n;
	real a, al = 0.25r, ar = 0.0r;
	if(value.x < 0.0r && value.y < 0.0r)
		v = -value;
	else if(value.x < 0.0r)
		v = vector<real, 2>(value.y, -value.x);
	else if(value.y < 0.0r)
		v = vector<real, 2>(-value.y, value.x);
	else v = value;
	while(true)
	{
		n = vector_normal(l + r);
		a = 0.5r * (al + ar);
		if(ar + 0.000000001r == al) break;
		if(n.x < v.x)
		{
			l = n;
			al = a;
		}
		else
		{
			r = n;
			ar = a;
		}
	}
	if(value.x < 0.0r && value.y < 0.0r)
		a += 0.5r;
	else if(value.x < 0.0r)
		a += 0.25r;
	else if(value.y < 0.0r)
		a += 0.75r;
	return a;
}

vector<real, 2> closest_line_point(
	vector<real, 2> point,
	vector<real, 2> line_point1,
	vector<real, 2> line_point2)
{
	vector<real, 2> dir = line_point2 - line_point1;
	real len = vector_length(dir),
		dist = vector_dot(point - line_point1, dir /= len);
	if(dist < real(0)) return line_point1;
	else if(dist > len) return line_point2;
	return line_point1 + dir * dist;
}

vector<real, 2> closest_infinite_line_point(
	vector<real, 2> point,
	vector<real, 2> linePoint1,
	vector<real, 2> linePoint2)
{
	vector<real, 2> dir = vector_normal(linePoint2 - linePoint1);
	return linePoint1 + dir * vector_dot(point - linePoint1, dir);
}

bool intersect_lines(
	vector<real, 2> line1_point1,
	vector<real, 2> line1_point2,
	vector<real, 2> line2_point1,
	vector<real, 2> line2_point2,
	vector<real, 2> *intersect_point)
{
	vector<real, 2> v1 = line1_point2 - line1_point1,
		v2 = line2_point2 - line2_point1;
	if(abs(v1.x * v2.y - v1.y * v2.x) < 0.0001r) return false;
	real c = v2.x * (line1_point1.y - line2_point1.y) - v2.y * (line1_point1.x - line2_point1.x);
	intersect_point->x = line1_point1.x + c * v1.x / (v1.x * v2.y - v1.y * v2.x);
	intersect_point->y = line1_point1.y + c * v1.y / (v1.x * v2.y - v1.y * v2.x);
	return true;
}

bool ccw_test(vector<real, 2> a, vector<real, 2> b, vector<real, 2> c)
{
	return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y) >= 0.0r;
}

matrix<real, 3, 3> scale_matrix(real x, real y, vector<real, 2> origin)
{
	matrix<real, 3, 3> mat;
	mat.m[0][0] = x;
	mat.m[0][1] = 0.0r;
	mat.m[0][2] = 0.0r;
	mat.m[1][0] = 0.0r;
	mat.m[1][1] = y;
	mat.m[1][2] = 0.0r;
	mat.m[2][0] = (1.0r - x)*origin.x;
	mat.m[2][1] = (1.0r - y)*origin.y;
	mat.m[2][2] = 1.0r;
	return mat;
}

matrix<real, 3, 3> rotate_matrix(real angle, vector<real, 2> origin)
{
	matrix<real, 3, 3> mat;
	vector<real, 2> n = angle_normal(angle);
	mat.m[0][0] = n.x;
	mat.m[0][1] = n.y;
	mat.m[0][2] = 0.0r;
	mat.m[1][0] = -n.y;
	mat.m[1][1] = n.x;
	mat.m[1][2] = 0.0r;
	mat.m[2][0] = (1.0r - mat.m[0][0])*origin.x - mat.m[1][0] * origin.y;
	mat.m[2][1] = (1.0r - mat.m[1][1])*origin.y - mat.m[0][1] * origin.x;
	mat.m[2][2] = 1.0r;
	return mat;
}

matrix<real, 3, 3> translate_matrix(real x, real y)
{
	matrix<real, 3, 3> mat;
	mat.m[0][0] = 1.0r;
	mat.m[0][1] = 0.0r;
	mat.m[0][2] = 0.0r;
	mat.m[1][0] = 0.0r;
	mat.m[1][1] = 1.0r;
	mat.m[1][2] = 0.0r;
	mat.m[2][0] = x;
	mat.m[2][1] = y;
	mat.m[2][2] = 1.0r;
	return mat;
}
