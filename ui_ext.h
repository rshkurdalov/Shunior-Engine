#pragma once
#include "ui_types.h"
#include "real.h"
#include "frame.h"
#include "geometry.h"

adaptive_size<real> operator""uiabs(uint64 value);
adaptive_size<real> operator""uirel(const char8 *str);
adaptive_size<real> operator""uiauto(uint64 value);

template<> struct utility<frame>
{
	uint32 resolve_size(adaptive_size<real> size, uint32 related_size);
	rectangle<int32> frame_viewport(frame *fm);
	rectangle<int32> frame_content_viewport(frame *fm);
	vector<uint32, 2> content_size_to_frame_size(frame *fm, uint32 content_width, uint32 content_height);
	vector<uint32, 2> frame_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void cascade_frames(handleable<frame> fm, array<handleable<frame>> *frames);
	bool rectangular_hit_test(frame *fm, vector<int32, 2> point);
};
