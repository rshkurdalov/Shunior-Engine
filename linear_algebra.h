#pragma once
#include "global_operators.h"
#include "real.h"
#include "vector.h"
#include "matrix.h"

template<uint32 extent>
real vector_dot(vector<real, extent> a, vector<real, extent> b)
{
	return accumulate<extent>(product<real, real>, add<real>, real(0), a.coord, b.coord);
}

template<uint32 extent>
real vector_length(vector<real, extent> v)
{
	return root(vector_dot(v, v), 2);
}

template<uint32 extent>
vector<real, 2> vector_normal(vector<real, extent> v)
{
	return v /= vector_length(v);
}

template<uint32 extent>
vector<real, 2> vector_cross(vector<real, extent> a, vector<real, extent> b)
{
	if constexpr(extent == 2)
		return vector<real, 2>(
			a.x * b.y - a.y * b.x,
			a.x * b.y - a.y * b.x);
	else if constexpr(extent == 3)
		return vector<real, 3>(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	else return vector<real, extent>();
}

template<typename value_type, uint32 rows, uint32 columns>
void set_identity_matrix(matrix<value_type, rows, columns> *mat)
{
	*mat = static_cast<value_type>(0);
	for(uint32 i = 0; i < min(rows, columns); i++)
		mat->m[i][i] = static_cast<value_type>(1);
}

template<typename value_type, uint32 n>
value_type determinant(matrix<value_type, n, n> &mat)
{
	if constexpr(n == 1)
		return mat.m[0][0];
	else if constexpr(n == 2)
		return mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0];
	else if constexpr(n == 3)
		return mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0]
			+ mat.m[0][2] * mat.m[1][0] * mat.m[2][1] - mat.m[0][2] * mat.m[1][1] * mat.m[2][0]
			- mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1];
	else if constexpr(n == 4)
		return mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0] - mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0]
			- mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0] + mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0]
			+ mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0] - mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0]
			- mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1] + mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1]
			+ mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1] - mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1]
			- mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1] + mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1]
			+ mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2] - mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2]
			- mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2] + mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2]
			+ mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2] - mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2]
			- mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3]
			+ mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3]
			- mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3] + mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3];
	else
	{
		value_type result = static_cast<value_type>(0);
		matrix<value_type, n - 1, n - 1> subm;
		for(uint32 i = 0; i < n; i++)
		{
			for(uint32 j = 1; j < n; j++)
			{
				for(uint32 k = 0; k < n; k++)
					if(k < i) subm.m[j - 1][k] = mat.m[j][k];
					else if(k > i) subm.m[j - 1][k - 1] = mat.m[j][k];
			}
			if(i & 1) result -= mat.m[0][i] * determinant(subm);
			else result += mat.m[0][i] * determinant(subm);
		}
		return result;
	}
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, columns, rows> transpose_matrix(matrix<value_type, rows, columns> &mat)
{
	matrix<value_type, columns, rows> result;
	for(uint32 i = 0; i < rows; i++)
	{
		for(uint32 j = 0; j < columns; j++)
			result.m[j][i] = mat.m[i][j];
	}
	return result;
}

template<typename value_type, uint32 n>
void invert_matrix(matrix<value_type, n, n> *mat)
{
	value_type d = determinant(*mat);
	if(d != static_cast<value_type>(0))
	{
		matrix<value_type, n, n> cfm;
		matrix<value_type, n - 1, n - 1> subm;
		for(uint32 i = 0; i < n; i++)
		{
			for(uint32 j = 0; j < n; j++)
			{
				for(uint32 k = 0; k < n; k++)
				{
					for(uint32 p = 0; p < n; p++)
						if(k < i && p < j) subm.m[k][p] = mat->m[k][p];
						else if(k < i && p > j) subm.m[k][p - 1] = mat->m[k][p];
						else if(k > i && p < j) subm.m[k - 1][p] = mat->m[k][p];
						else if(k > i && p > j) subm.m[k - 1][p - 1] = mat->m[k][p];
				}
				if((i + j) & 1) cfm.m[i][j] = -determinant(subm);
				else cfm.m[i][j] = determinant(subm);
			}
		}
		*mat = transpose_matrix(cfm);
		*mat *= (static_cast<value_type>(1) / d);
	}
}

vector<real, 2> angle_normal(real value);
real vector_angle(vector<real, 2> value);
vector<real, 2> closest_line_point(
	vector<real, 2> point,
	vector<real, 2> line_point1,
	vector<real, 2> line_point2);
vector<real, 2> closest_infinite_line_point(
	vector<real, 2> point,
	vector<real, 2> linePoint1,
	vector<real, 2> linePoint2);
bool intersect_lines(
	vector<real, 2> line1_point1,
	vector<real, 2> line1_point2,
	vector<real, 2> line2_point1,
	vector<real, 2> line2_point2,
	vector<real, 2> *intersect_point);
bool ccw_test(vector<real, 2> a, vector<real, 2> b, vector<real, 2> c);
matrix<real, 3, 3> scale_matrix(real x, real y, vector<real, 2> origin);
matrix<real, 3, 3> rotate_matrix(real angle, vector<real, 2> origin);
matrix<real, 3, 3> translate_matrix(real x, real y);
