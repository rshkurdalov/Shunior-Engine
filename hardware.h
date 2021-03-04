#pragma once
#include "vector.h"
#include "time.h"

enum struct mouse_button
{
	left,
	right,
	wheel,
	x1,
	x2
};

struct mouse_device
{
	vector<int32, 2> position;
	vector<int32, 2> prev_position;
	mouse_button last_clicked;
	nanoseconds last_left_clicked_time;
	bool double_click;
	mouse_button last_released;
	bool wheel_forward;
	bool left_pressed;
	bool right_pressed;
	bool wheel_pressed;
	bool x1_pressed;
	bool x2_pressed;
	nanoseconds double_click_time;

	mouse_device();
};

mouse_device *mouse();

enum struct key_code
{
	none = 0,
	backspace = 8,
	tab = 9,
	enter = 13,
	shift = 16,
	ctrl = 17,
	alt = 18,
	left = 37,
	up = 38,
	right = 39,
	down = 40,
	a = 65,
	b = 66,
	c = 67,
	d = 68,
	e = 69,
	f = 70,
	g = 71,
	h = 72,
	i = 73,
	j = 74,
	k = 75,
	l = 76,
	m = 77,
	n = 78,
	o = 79,
	p = 80,
	q = 81,
	r = 82,
	s = 83,
	t = 84,
	u = 85,
	v = 86,
	w = 87,
	x = 88,
	y = 89,
	z = 90
};

struct keyboard_device
{
	key_code last_pressed;
	key_code last_released;
	char32 char_code;
	bool key_pressed[256];
	uint32 pressed_count;

	keyboard_device();
};

keyboard_device *keyboard();
