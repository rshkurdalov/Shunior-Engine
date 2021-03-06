#pragma once
#include "global_operators.h"

struct real
{
	uint32 integer;
	uint32 negative : 1;
	uint32 fraction : 31;

	real() {}

	template<typename value_type>
	real(value_type right)
	{
		if(right < (value_type)0)
		{
			integer = (uint32)(-right);
			negative = true;
		}
		else
		{
			integer = (uint32)right;
			negative = false;
		}
		fraction = 0;
	}

	real(uint32 integer, uint32 fraction)
		: integer(integer), fraction(fraction), negative(false) {}

	real(uint32 integer, uint32 fraction, bool negative)
		: integer(integer), fraction(fraction), negative(negative) {}

	int32 get_int32()
	{
		if(negative) return -(int32)integer;
		else return (int32)integer;
	}
};

const uint32 max_real_fraction = 999999999;
const real min_real = real(4294967295, max_real_fraction, true);
const real max_real = real(4294967295, max_real_fraction);
const real undefined_real = real(0, 0, true);

bool operator==(real a, real b);
bool operator!=(real a, real b);
bool operator<(real a, real b);
bool operator<=(real a, real b);
bool operator>(real a, real b);
bool operator>=(real a, real b);
real operator-(real value);
real operator+(real a, real b);
real &operator+=(real &a, real b);
real operator-(real a, real b);
real &operator-=(real &a, real b);
real operator*(real a, real b);
real &operator*=(real &a, real b);
real operator/(real a, real b);
real &operator/=(real &a, real b);
real operator""r(const char8 *str);
real abs(real value);
real floor(real value);
real ceil(real value);
real round(real value);
real pow(real x, uint32 y);
real root(real x, uint32 y);
