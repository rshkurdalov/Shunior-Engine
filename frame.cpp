#include "frame.h"
#include "ui_ext.h"
#include "frame_templates.h"

frame::frame()
{
	width_desc = 200uiauto;
	min_width = 0;
	max_width = 1000000000;
	height_desc = 200uiauto;
	min_height = 0;
	max_height = 1000000000;
	margin_left = 2uiabs;
	margin_bottom = 2uiabs;
	margin_right = 2uiabs;
	margin_top = 2uiabs;
	padding_left =  0.05uirel;
	padding_bottom = 0.05uirel;
	padding_right = 0.05uirel;
	padding_top = 0.05uirel;
	visible = true;
	enabled = true;
	focusable = false;
	hit_test = default_frame_callbacks()->frame_hit_test;
	subframes = empty_function<decltype(subframes)>();
	content_size = default_frame_callbacks()->frame_content_size;
	render = empty_function<decltype(render)>();
	mouse_click = empty_function<decltype(mouse_click)>();
	hook_mouse_click = false;
	return_mouse_click = false;
	mouse_release = empty_function<decltype(mouse_release)>();
	hook_mouse_release = false;
	return_mouse_release = false;
	mouse_move = empty_function<decltype(mouse_move)>();
	hook_mouse_move = false;
	return_mouse_move = false;
	start_hover = empty_function<decltype(start_hover)>();
	end_hover = empty_function<decltype(end_hover)>();
	focus_receive = empty_function<decltype(focus_receive)>();
	focus_loss = empty_function<decltype(focus_loss)>();
	mouse_wheel_rotate = empty_function<decltype(mouse_wheel_rotate)>();
	hook_mouse_wheel_rotate = false;
	return_mouse_wheel_rotate = false;
	key_press = empty_function<decltype(key_press)>();
	key_release = empty_function<decltype(key_release)>();
	char_input = empty_function<decltype(char_input)>();
}
