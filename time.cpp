#include "time.h"
#include "os_api.h"

nanoseconds &operator<<(nanoseconds &ns, microseconds mcs)
{
	ns.value += mcs.value * 1000;
	return ns;
}

nanoseconds &operator<<(nanoseconds &ns, milliseconds ms)
{
	ns.value += ms.value * 1000 * 1000;
	return ns;
}

nanoseconds &operator<<(nanoseconds &ns, seconds s)
{
	ns.value += s.value * 1000 * 1000 * 1000;
	return ns;
}

nanoseconds &operator<<(nanoseconds &ns, minutes min)
{
	ns.value += min.value * 60 * 1000 * 1000 * 1000;
	return ns;
}

nanoseconds &operator<<(nanoseconds &ns, hours h)
{
	ns.value += h.value * 60 * 60 * 1000 * 1000 * 1000;
	return ns;
}

nanoseconds &operator<<(nanoseconds &ns, days d)
{
	ns.value += int64(24) * 60 * 60 * 1000 * 1000 * 1000;
	return ns;
}

microseconds &operator<<(microseconds &mcs, nanoseconds ns)
{
	mcs.value += ns.value / 1000;
	return mcs;
}

microseconds &operator<<(microseconds &mcs, milliseconds ms)
{
	mcs.value += ms.value * 1000;
	return mcs;
}

microseconds &operator<<(microseconds &mcs, seconds s)
{
	mcs.value += s.value * 1000 * 1000;
	return mcs;
}

microseconds &operator<<(microseconds &mcs, minutes min)
{
	mcs.value += min.value * 60 * 1000 * 1000;
	return mcs;
}

microseconds &operator<<(microseconds &mcs, hours h)
{
	mcs.value += h.value * 60 * 60 * 1000 * 1000;
	return mcs;
}

microseconds &operator<<(microseconds &mcs, days d)
{
	mcs.value += d.value * 24 * 60 * 60 * 1000 * 1000;
	return mcs;
}

milliseconds &operator<<(milliseconds &ms, nanoseconds ns)
{
	ms.value += ns.value / (1000 * 1000);
	return ms;
}

milliseconds &operator<<(milliseconds &ms, microseconds mcs)
{
	ms.value += mcs.value / 1000;
	return ms;
}

milliseconds &operator<<(milliseconds &ms, seconds s)
{
	ms.value += s.value * 1000;
	return ms;
}

milliseconds &operator<<(milliseconds &ms, minutes min)
{
	ms.value += min.value * 60 * 1000;
	return ms;
}

milliseconds &operator<<(milliseconds &ms, hours h)
{
	ms.value += h.value * 60 * 60 * 1000;
	return ms;
}

milliseconds &operator<<(milliseconds &ms, days d)
{
	ms.value += d.value * 24 * 60 * 60 * 1000;
	return ms;
}

seconds &operator<<(seconds &s, nanoseconds ns)
{
	s.value += ns.value / (1000 * 1000 * 1000);
	return s;
}

seconds &operator<<(seconds &s, microseconds mcs)
{
	s.value += mcs.value / (1000 * 1000);
	return s;
}

seconds &operator<<(seconds &s, milliseconds ms)
{
	s.value += ms.value / 1000;
	return s;
}

seconds &operator<<(seconds &s, minutes min)
{
	s.value += min.value * 60;
	return s;
}

seconds &operator<<(seconds &s, hours h)
{
	s.value += h.value * 60 * 60;
	return s;
}

seconds &operator<<(seconds &s, days d)
{
	s.value += d.value * 24 * 60 * 60;
	return s;
}

minutes &operator<<(minutes &min, nanoseconds ns)
{
	min.value += ns.value / (int64(1000) * 1000 * 1000 * 60);
	return min;
}

minutes &operator<<(minutes &min, microseconds mcs)
{
	min.value += mcs.value / (1000 * 1000 * 60);
	return min;
}

minutes &operator<<(minutes &min, milliseconds ms)
{
	min.value += ms.value / (1000 * 60);
	return min;
}

minutes &operator<<(minutes &min, seconds s)
{
	min.value += s.value / 60;
	return min;
}

minutes &operator<<(minutes &min, hours h)
{
	min.value += h.value * 60;
	return min;
}

minutes &operator<<(minutes &min, days d)
{
	min.value += d.value * 24 * 60;
	return min;
}

hours &operator<<(hours &h, nanoseconds ns)
{
	h.value += ns.value / (int64(1000) * 1000 * 1000 * 60 * 60);
	return h;
}

hours &operator<<(hours &h, microseconds mcs)
{
	h.value += mcs.value / (int64(1000) * 1000 * 60 * 60);
	return h;
}

hours &operator<<(hours &h, milliseconds ms)
{
	h.value += ms.value / (1000 * 60 * 60);
	return h;
}

hours &operator<<(hours &h, seconds s)
{
	h.value += s.value / (60 * 60);
	return h;
}

hours &operator<<(hours &h, minutes min)
{
	h.value += min.value / 60;
	return h;
}

hours &operator<<(hours &h, days d)
{
	h.value += d.value * 24;
	return h;
}

days &operator<<(days &d, nanoseconds ns)
{
	d.value += ns.value / (int64(1000) * 1000 * 1000 * 60 * 60 * 24);
	return d;
}

days &operator<<(days &d, microseconds mcs)
{
	d.value += mcs.value / (int64(1000) * 1000 * 60 * 60 * 24);
	return d;
}

days &operator<<(days &d, milliseconds ms)
{
	d.value += ms.value / (1000 * 60 * 60 * 24);
	return d;
}

days &operator<<(days &d, seconds s)
{
	d.value += s.value / (60 * 60 * 24);
	return d;
}

days &operator<<(days &d, minutes min)
{
	d.value += min.value / (60 * 24);
	return d;
}

days &operator<<(days &d, hours h)
{
	d.value += h.value / 24;
	return d;
}

nanoseconds now()
{
	return nanoseconds(os_current_timestamp());
}
