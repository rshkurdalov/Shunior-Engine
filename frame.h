#pragma once
#include "global_types.h"
#include "array.h"
#include "ui_types.h"
#include "graphics.h"

struct frame
{
	int32 x;
	int32 y;
	uint32 width;
	uint32 height;
	ui_size width_desc;
	uint32 min_width;
	uint32 max_width;
	ui_size height_desc;
	uint32 min_height;
	uint32 max_height;
	ui_size margin_left;
	ui_size margin_bottom;
	ui_size margin_right;
	ui_size margin_top;
	ui_size padding_left;
	ui_size padding_bottom;
	ui_size padding_right;
	ui_size padding_top;
	bool visible;
	bool enabled;
	bool focusable;
	bool (*hit_test)(frame *fm, vector<int32, 2> point);
	void (*subframes)(frame *fm, array<frame *> *frames);
	vector<uint32, 2> (*content_size)(frame *fm, uint32 viewport_width, uint32 viewport_height);
	void (*render)(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*mouse_click)(frame *fm);
	bool hook_mouse_click;
	bool return_mouse_click;
	void (*mouse_release)(frame *fm);
	bool hook_mouse_release;
	bool return_mouse_release;
	void (*mouse_move)(frame *fm);
	bool hook_mouse_move;
	bool return_mouse_move;
	void (*start_hover)(frame *fm);
	void (*end_hover)(frame *fm);
	void (*focus_receive)(frame *fm);
	void (*focus_loss)(frame *fm);
	void (*mouse_wheel_rotate)(frame *fm);
	bool hook_mouse_wheel_rotate;
	bool return_mouse_wheel_rotate;
	void (*key_press)(frame *fm);
	void (*key_release)(frame *fm);
	void (*char_input)(frame *fm);
	void *data;

	frame();
	~frame();
};
