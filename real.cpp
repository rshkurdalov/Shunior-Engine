#include "real.h"

bool operator==(real a, real b)
{
	return a.integer == b.integer && a.fraction == b.fraction && a.negative == b.negative;
}

bool operator!=(real a, real b)
{
	return !(a == b);
}

bool operator<(real a, real b)
{
	if(a.negative)
	{
		if(b.negative) return a.integer > b.integer || a.integer == b.integer && a.fraction > b.fraction;
		else return true;
	}
	else
	{
		if(b.negative) return false;
		else return a.integer < b.integer || a.integer == b.integer && a.fraction < b.fraction;
	}
}

bool operator<=(real a, real b)
{
	if(a.negative)
	{
		if(b.negative) return a.integer > b.integer || a.integer == b.integer && a.fraction >= b.fraction;
		else return true;
	}
	else
	{
		if(b.negative) return false;
		else return a.integer < b.integer || a.integer == b.integer && a.fraction <= b.fraction;
	}
}

bool operator>(real a, real b)
{
	return b < a;
}

bool operator>=(real a, real b)
{
	return b <= a;
}

real operator-(real value)
{
	if(value == undefined_real) return undefined_real;
	return real(
		value.integer,
		value.fraction,
		!value.negative && (value.integer != 0 || value.fraction != 0));
}

real operator+(real a, real b)
{
	if(a == undefined_real || b == undefined_real) return undefined_real;
	if(a.negative == b.negative)
	{
		a.integer += b.integer;
		a.fraction += b.fraction;
		if(a.fraction > max_real_fraction)
		{
			a.fraction -= (max_real_fraction + 1);
			a.integer++;
		}
	}
	else
	{
		if(a.integer < b.integer || a.integer == b.integer && a.fraction < b.fraction)
			swap(&a, &b);
		a.integer -= b.integer;
		if(a.fraction >= b.fraction)
		{
			a.fraction -= b.fraction;
			if(a.integer == 0 && a.fraction == 0)
				a.negative = false;
		}
		else
		{
			a.fraction = (max_real_fraction + 1 + a.fraction - b.fraction);
			a.integer--;
		}
	}
	return a;
}

real &operator+=(real &a, real b)
{
	a = a + b;
	return a;
}

real operator-(real a, real b)
{
	return a + -b;
}

real &operator-=(real &a, real b)
{
	a = a - b;
	return a;
}

real operator*(real a, real b)
{
	if(a == undefined_real || b == undefined_real) return undefined_real;
	real c;
	c.integer = a.integer * b.integer;
	uint64 d = uint64(a.integer) * uint64(b.fraction);
	c.integer += uint32(d / (max_real_fraction + 1));
	c.fraction = uint32(d % (max_real_fraction + 1));
	d = uint64(a.fraction) * uint64(b.integer);
	c.integer += uint32(d / (max_real_fraction + 1));
	c.fraction += uint32(d % (max_real_fraction + 1));
	if(c.fraction > max_real_fraction)
	{
		c.fraction -= (max_real_fraction + 1);
		c.integer++;
	}
	c.fraction += uint32(uint64(a.fraction) * uint64(b.fraction) / uint64(max_real_fraction + 1));
	if(c.fraction > max_real_fraction)
	{
		c.fraction -= (max_real_fraction + 1);
		c.integer++;
	}
	c.negative = (a.negative != b.negative) && (c.integer != 0 || c.fraction != 0);
	return c;
}

real &operator*=(real &a, real b)
{
	a = a * b;
	return a;
}

real operator/(real a, real b)
{
	if(a == undefined_real || b == undefined_real || b == real(0)) return undefined_real;
	uint64 c = a.integer, d = b.integer, e;
	real r = 0;
	for(uint32 i = 0, j = (max_real_fraction + 1) / 10; i < 9; i++, j /= 10)
	{
		c = c * 10 + a.fraction / j;
		a.fraction -= (a.fraction / j) * j;
		d = d * 10 + b.fraction / j;
		b.fraction -= (b.fraction / j) * j;
	}
	r.integer = uint32(c / d);
	e = (c % d) * 10;
	for(uint32 i = 0; i < 9; i++)
	{
		r.fraction = r.fraction * 10 + uint32(e / d);
		e = (e % d) * 10;
	}
	r.negative = a.negative != b.negative && (r.integer != 0 || r.fraction != 0);
	return r;
}

real &operator/=(real &a, real b)
{
	a = a / b;
	return a;
}

real operator""r(const char8 *str)
{
	real value(0);
	uint64 i = 0, p1 = 0, p2;
	uint32 multiplier = 1;
	while(str[i] != '\0' && str[i] != '.') i++;
	for(p2 = i, i--; i != p1 - 1; i--)
	{
		value.integer += (str[i] - 48) * multiplier;
		multiplier *= 10;
	}
	if(str[p2] == '\0') return value;
	multiplier = (max_real_fraction + 1) / 10;
	for(i = p2 + 1; str[i] != '\0'; i++)
	{
		value.fraction += (str[i] - 48) * multiplier;
		multiplier /= 10;
	}
	return value;
}

real abs(real value)
{
	return real(value.integer, value.fraction, false);
}

real floor(real value)
{
	if(value.negative)
	{
		if(value.fraction != 0)
			return real(value.integer + 1, 0, true);
		else return value;
	}
	else return real(value.integer, 0, false);
}

real ceil(real value)
{
	if(value.negative)
		return real(value.integer, 0, value.integer != 0);
	else
	{
		if(value.fraction != 0)
			return real(value.integer + 1, 0, false);
		else return value;
	}
}

real round(real value)
{
	return value.fraction <= ((max_real_fraction + 1) / 2) ?
		real(value.integer, 0, value.negative && value.integer != 0)
		: real(value.integer + 1, 0, value.negative);
}

real pow(real x, uint32 y)
{
	if(y == 1) return x;
	real a = pow(x, y >> 1), b = a * a;
	if(a != 0.0r && abs(b / a - a) > 0.000000002r)
		b = undefined_real;
	else if(y & 1)
	{
		a = b;
		b *= x;
		if(x != 0.0r && abs(b / x - a) > 0.000000001r)
			b = undefined_real;
	}
	return b;
}

real root(real x, uint32 y)
{
	if(y < 2)
	{
		if(y == 0)
		{
			if(x == 0.0r) return 0.0r;
			else return 1;
		}
		else return x;
	}
	real l, r, m, p;
	if(x < 0.0r)
	{
		l = min(x, -1.0r);
		r = 0.0r;
	}
	else
	{
		l = 0.0r;
		r = max(x, 1.0r);
	}
	while(true)
	{
		m = 0.5r * (l + r);
		if(l == m || r == m) break;
		p = pow(m, y);
		if(p < x && p != undefined_real) l = m;
		else r = m;
	}
	if(pow(r, y) <= x) return r;
	return m;
}
