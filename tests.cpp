#include "tests.h"
#include "string.h"
#include "parser.h"
#include "vector.h"
#include "matrix.h"
#include "array.h"
#include "frame.h"
#include "frame_templates.h"
#include "set.h"

void test_array()
{
	array<uint32> arr;
	arr.push(1);
	arr.push(2);
	arr.push(3);
	arr.push(4);
	arr.push(5);
	arr.push(6);
	arr.push(7);
	uint32 extArr[3] = {8, 9, 10};
	arr.insert_range(1, extArr, extArr + 2);
	arr.pop();
	arr.remove(1);
	arr.remove_range(1, 5);
	arr.clear();
	arr.push(7);
	arr.reset();
	arr.insert(0, uint32(8));
	arr.insert_default(0, 4);
	arr.clear();

	uint64 idx;
	arr.binary_insert(5);
	idx = arr.lower_bound(key<uint32>(1));
	idx = arr.lower_bound(key<uint32>(5));
	idx = arr.lower_bound(key<uint32>(6));
	arr.binary_insert(3);
	idx = arr.lower_bound(key<uint32>(1));
	idx = arr.lower_bound(key<uint32>(3));
	idx = arr.lower_bound(key<uint32>(5));
	idx = arr.lower_bound(key<uint32>(6));
	arr.binary_insert(3);
	arr.binary_insert(3);
	arr.binary_remove(3);
	arr.binary_remove(4);
	idx = arr.binary_search(5);
	idx = arr.binary_search(4);
}

void test_string()
{
	string str, str1;
	str << "text";
	str << u"text";
	str << U"text";
	str.clear();
	str << "abc" << -2 << U'+' << int8(42) << U'=' << uint16(40) << "  r = " << 40.0r;
	int32 v1;
	string_line<char32>(U"-50", 3) >> v1;
	uint32 v2;
	string_line<char32>(U"500", 3) >> v2;
	real v3;
	string_line<char32>(U"-50.689", 7) >> v3;
	string_line<char32>(U"50.08", 5) >> v3;

	char16 str2[] = u"15 89.6 -8 - ab -9. 874yt-14.0";
	parser p(str2, array_size(str2) - 1, parse_mode::text, string_format::u16);
	p >> v1;
	p >> v3;
	p >> v1;
	p >> v3;
	p >> v3;
	p >> v3;
	p.position = 0;
	str.clear();
	p >> str;
	str << U' ';
	p >> str;
}

void test_vector()
{
	vector<int32, 2> v, t, r;
	v.x = 0;
	v.y = 1;
	t.x = 5;
	t.y = 11;
	r = v + t; // x = 5, y = 12
	r += t; // x = 10, y = 23
	r -= vector<int32, 2>(4, 3); // x = 6, y = 20
	r = -r; // x = -6, y = -20
	r = r - vector<int32, 2>(-6, -20); // x = 0, y = 0
	r += t; // x = 5, y = 11
	r -= t; // x = 0, y = 0
	r += 2 * t; // x = 10, y = 22
	r += t * 3; // x = 25, y = 55
	r *= 2; // x = 50, y = 110
	r /= 2; // x = 25, y = 55
	r -= t / 2; // x = 23, y = 50
}

void test_matrix()
{
	matrix<real, 2, 3> mat = 1.0r, mat1;
	mat = 0.0r;
	mat1 = 1;
	mat += mat1;
	mat += mat1 + mat1;
	mat.m[1][1] -= 0.5r;
	mat.m[1][2] -= 1.0r;
	mat -= mat1;
	mat = 3.0r * mat1 - mat;
	mat *= 2.0r;
	mat /= 2.0r;
	mat = mat1 / 2.0r + mat;
	real mtValues[3][2] =
	{
		{2.0r, 6.0r},
		{3.0r, 1.0r},
		{4.0r, 1.5r}
	};
	matrix<real, 3, 2> mt(mtValues);
	vector<real, 3> vt(2.5r, 5.0r, 50.0r);
	matrix<real, 1, 2> a = vt * mt;
	matrix<real, 2, 1> b;
	b.m[0][0] = 1.0r;
	b.m[1][0] = 5.0r;
	matrix<real, 2, 3> c = b * vt;
	matrix<real, 3, 3> d = mt * c;
	real eValues[5][5] =
	{
		{8, 9, 8, 5, 1},
		{74, 8, 5, 35, 12},
		{54, 12, 32, 9, 10},
		{12, 54, 87, 45, 24},
		{70, 4, 7, 8, 1}
	};
	matrix<real, 5, 5> e(eValues);
	real det = determinant(e); // det = 2996358
	matrix<real, 2, 3> mtTransposed = transpose_matrix(mt);
	invert_matrix(&e);

	real tr_values[3][3] =
	{
		{5.5r, 10.0r, 0.0r},
		{8.0r, 9.0r, 0.0r},
		{50.0r, 4.0r, 1.0r}
	};
	matrix<real, 3, 3> tr(tr_values);
	matrix<real, 3, 3> itr = tr;
	invert_matrix(&itr);
	matrix<real, 1, 3> mul = vector<real, 3>(1.0r, 1.0r, 1.0r) * tr;
	vector<real, 2> p = vector<real, 2>(mul.m[0][0], mul.m[0][1]);
	mul = vector<real, 3>(p.x, p.y, 1.0r) * itr;
	p = vector<real, 2>(mul.m[0][0], mul.m[0][1]);
}

void test_real()
{
	real a = 12.24r, b = 0.5r, c = a / b; // c = 24.48
	real d = 2.0r, e = 3.0r;
	real f = d / e; // f = 0.666666666
	real g = 1.0r, h = -1.0r;
	real l = g / h; // l = -1
	l = h / h; // l = 1
	l = e / h; // l = -3
	real m = 0.0r;
	l = m / e; // l = 0
	l = e / m;
	l = m / m;
	l = f / m;
	l = m / h;
	l = b - g; // l = -0.5
	l = e - a; // l = -9.24
	l = root(4.0r, 2); // l = 2
	l = root(80.0r, 2); // l = 8.944271909
	l = root(100000000.0r, 8); // l = 10
}

void test_linear_algebra()
{
	real r = angle_normal(0.111408460r).y; // r = 0.644217687
	r = vector_angle(angle_normal(0.111408460r));
	r = angle_normal(0.5r).y; // r = 0
	r = vector_angle(angle_normal(0.5r));
	r = angle_normal(1.0r).y; // r = 0
	r = angle_normal(-1.0r).y; // r = 0
	r = vector_angle(angle_normal(-1.0r));
	r = angle_normal(1.5r).y; // r = 0
	r = vector_angle(angle_normal(1.5r));
	r = angle_normal(0.0r).y; // r = 0
	r = angle_normal(0.25r).y; // r = 1
	r = angle_normal(-0.25r).y; // r = -1

	r = angle_normal(0.0r).x; // r = 1
	r = angle_normal(0.111408460r).x; // r = 0.764842187
	r = angle_normal(0.5r).x; // r = -1

	r = root(9.0r, 2);
	r = root(8.0r, 3);
	r = root(800000.0r, 2);

	vector<real, 2> v = vector<real, 2>(96.107510240r - 95.080509997r, 2.660436428r - 2.380373700r);
	r = vector_dot(v, v);
	r = root(r, 2);
}

void test_set()
{
	set<int32> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(3);
	s.remove(key<int32>(3));

	s.insert(3);
	s.remove(2);

	s.insert(2);
	s.clear();

	s.insert(2);
	s.insert(0);
	s.insert(1);

	s.clear();
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(0);
	s.remove(2);
}
