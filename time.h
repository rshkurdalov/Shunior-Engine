#pragma once
#include "global_types.h"

struct nanoseconds
{
	int64 value;

	nanoseconds() {}
	nanoseconds(int64 value) : value(value) {}
};

struct microseconds
{
	int64 value;

	microseconds() {}
	microseconds(int64 value) : value(value) {}
};

struct milliseconds
{
	int64 value;

	milliseconds() {}
	milliseconds(int64 value) : value(value) {}
};

struct seconds
{
	int64 value;

	seconds() {}
	seconds(int64 value) : value(value) {}
};

struct minutes
{
	int64 value;

	minutes() {}
	minutes(int64 value) : value(value) {}
};

struct hours
{
	int64 value;

	hours() {}
	hours(int64 value) : value(value) {}
};

struct days
{
	int64 value;

	days() {}
	days(int64 value) : value(value) {}
};

nanoseconds &operator<<(nanoseconds &ns, microseconds mcs);
nanoseconds &operator<<(nanoseconds &ns, milliseconds ms);
nanoseconds &operator<<(nanoseconds &ns, seconds s);
nanoseconds &operator<<(nanoseconds &ns, minutes min);
nanoseconds &operator<<(nanoseconds &ns, hours h);
nanoseconds &operator<<(nanoseconds &ns, days d);
microseconds &operator<<(microseconds &mcs, nanoseconds ns);
microseconds &operator<<(microseconds &mcs, milliseconds ms);
microseconds &operator<<(microseconds &mcs, seconds s);
microseconds &operator<<(microseconds &mcs, minutes min);
microseconds &operator<<(microseconds &mcs, hours h);
microseconds &operator<<(microseconds &mcs, days d);
milliseconds &operator<<(milliseconds &ms, nanoseconds ns);
milliseconds &operator<<(milliseconds &ms, microseconds mcs);
milliseconds &operator<<(milliseconds &ms, seconds s);
milliseconds &operator<<(milliseconds &ms, minutes min);
milliseconds &operator<<(milliseconds &ms, hours h);
milliseconds &operator<<(milliseconds &ms, days d);
seconds &operator<<(seconds &s, nanoseconds ns);
seconds &operator<<(seconds &s, microseconds mcs);
seconds &operator<<(seconds &s, milliseconds ms);
seconds &operator<<(seconds &s, minutes min);
seconds &operator<<(seconds &s, hours h);
seconds &operator<<(seconds &s, days d);
minutes &operator<<(minutes &min, nanoseconds ns);
minutes &operator<<(minutes &min, microseconds mcs);
minutes &operator<<(minutes &min, milliseconds ms);
minutes &operator<<(minutes &min, seconds s);
minutes &operator<<(minutes &min, hours h);
minutes &operator<<(minutes &min, days d);
hours &operator<<(hours &h, nanoseconds ns);
hours &operator<<(hours &h, microseconds mcs);
hours &operator<<(hours &h, milliseconds ms);
hours &operator<<(hours &h, seconds s);
hours &operator<<(hours &h, minutes min);
hours &operator<<(hours &h, days d);
days &operator<<(days &d, nanoseconds ns);
days &operator<<(days &d, microseconds mcs);
days &operator<<(days &d, milliseconds ms);
days &operator<<(days &d, seconds s);
days &operator<<(days &d, minutes min);
days &operator<<(days &d, hours h);
nanoseconds now();
