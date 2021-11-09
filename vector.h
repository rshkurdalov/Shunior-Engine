#pragma once
#include "global_operators.h"

template<typename value_type, uint32 extent> struct vector
{
	value_type coord[extent];

	vector() {}

	template<typename right_type> vector(vector<right_type, extent> &v)
	{
		coherent_mapping<extent>(
			[] (value_type value) -> right_type { return right_type(value); }, coord, v.coord);
	}
};

template<typename value_type> struct vector<value_type, 1>
{
	union
	{
		value_type x;
		value_type coord[1];
	};

	vector() {}

	vector(value_type x) : x(x) {}

	template<typename right_type> vector(vector<right_type, 1> v)
	{
		x = right_type(v.x);
	}
};

template<typename value_type> struct vector<value_type, 2>
{
	union
	{
		struct { value_type x, y; };
		value_type coord[2];
	};

	vector() {}

	vector(value_type x, value_type y) : x(x), y(y) {}

	template<typename right_type> vector(vector<right_type, 2> v)
	{
		x = right_type(v.x);
		y = right_type(v.y);
	}
};

template<typename value_type> struct vector<value_type, 3>
{
	union
	{
		struct { value_type x, y, z; };
		value_type coord[3];
	};

	vector() {}

	vector(value_type x, value_type y, value_type z) : x(x), y(y), z(z) {}

	template<typename right_type> vector(vector<right_type, 3> v)
	{
		x = right_type(v.x);
		y = right_type(v.y);
		z = right_type(v.z);
	}
};

template<typename value_type> struct vector<value_type, 4>
{
	union
	{
		struct { value_type x, y, z, w; };
		value_type coord[4];
	};

	vector() {}

	vector(value_type x, value_type y, value_type z, value_type w) : x(x), y(y), z(z), w(w) {}

	template<typename right_type> vector(vector<right_type, 4> v)
	{
		x = right_type(v.x);
		y = right_type(v.y);
		z = right_type(v.z);
		w = right_type(v.w);
	}
};

template<typename value_type, uint32 extent>
vector<value_type, extent> operator-(vector<value_type, extent> value)
{
	coherent_run<extent>(negate<value_type>, value.coord);
	return value;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> &operator+=(
	vector<value_type, extent> &left,
	vector<value_type, extent> right)
{
	coherent_run<extent>(add<value_type>, left.coord, right.coord);
	return left;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> operator+(
	vector<value_type, extent> left,
	vector<value_type, extent> right)
{
	return left += right;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> &operator-=(
	vector<value_type, extent> &left,
	vector<value_type, extent> right)
{
	coherent_run<extent>(sub<value_type>, left.coord, right.coord);
	return left;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> operator-(
	vector<value_type, extent> left,
	vector<value_type, extent> right)
{
	return left -= right;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> &operator*=(
	vector<value_type, extent> &left,
	value_type right)
{
	coherent_run<extent>([right](value_type &value) -> void { value *= right; }, left.coord);
	return left;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> operator*(
	vector<value_type, extent> left,
	value_type right)
{
	return left *= right;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> operator*(
	value_type left,
	vector<value_type, extent> right)
{
	return right *= left;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> &operator/=(
	vector<value_type, extent> &left,
	value_type right)
{
	coherent_run<extent>([right](value_type &value) -> void { value /= right; }, left.coord);
	return left;
}

template<typename value_type, uint32 extent>
vector<value_type, extent> operator/(
	vector<value_type, extent> left,
	value_type right)
{
	return left /= right;
}

template<typename value_type, uint32 extent> bool operator==(
	vector<value_type, extent> left,
	vector<value_type, extent> right)
{
	return conjunction<extent>(equal<value_type>, left.coord, right.coord);
}

template<typename value_type, uint32 extent> bool operator!=(
	vector<value_type, extent> left,
	vector<value_type, extent> right)
{
	return !(left == right);
}
