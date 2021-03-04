#include "frame.h"
#include "ui_ext.h"

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
	hit_test = frame_hit_test_def;
	subframes = frame_subframes_def;
	content_size = frame_content_size_def;
	render = frame_render_def;
	mouse_click = frame_mouse_click_def;
	hook_mouse_click = false;
	return_mouse_click = false;
	mouse_release = frame_mouse_release_def;
	hook_mouse_release = false;
	return_mouse_release = false;
	mouse_move = frame_mouse_move_def;
	hook_mouse_move = false;
	return_mouse_move = false;
	start_hover = frame_start_hover_def;
	end_hover = frame_end_hover_def;
	focus_receive = frame_focus_receive_def;
	focus_loss = frame_focus_loss_def;
	mouse_wheel_rotate = frame_mouse_wheel_rotate_def;
	hook_mouse_wheel_rotate = false;
	return_mouse_wheel_rotate = false;
	key_press = frame_key_press_def;
	key_release = frame_key_release_def;
	char_input = frame_char_input_def;
}

frame::~frame()
{

}
