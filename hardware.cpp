#include "hardware.h"

mouse_device mouse_instance;
keyboard_device keyboard_instance;

mouse_device::mouse_device()
{
	position = vector<int32, 2>(0, 0);
	prev_position = vector<int32, 2>(0 ,0);
	last_clicked = mouse_button::left;
	last_left_clicked_time = 0;
	double_click = false;
	last_released = mouse_button::right;
	wheel_forward = false;
	left_pressed = false;
	right_pressed = false;
	wheel_pressed = false;
	x1_pressed = false;
	x2_pressed = false;
	double_click_time << milliseconds(500);
}

mouse_device *mouse()
{
	return &mouse_instance;
}

keyboard_device::keyboard_device()
{
	last_pressed = key_code::none;
	last_released = key_code::none;
	char_code = U'\0';
	for(uint64 i = 0; i < 256; i++)
		key_pressed[i] = false;
	pressed_count = 0;
}

keyboard_device *keyboard()
{
	return &keyboard_instance;
}
