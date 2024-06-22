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
	adaptive_size<real> width_desc;
	uint32 min_width;
	uint32 max_width;
	adaptive_size<real> height_desc;
	uint32 min_height;
	uint32 max_height;
	adaptive_size<real> margin_left;
	adaptive_size<real> margin_bottom;
	adaptive_size<real> margin_right;
	adaptive_size<real> margin_top;
	adaptive_size<real> padding_left;
	adaptive_size<real> padding_bottom;
	adaptive_size<real> padding_right;
	adaptive_size<real> padding_top;
	bool visible;
	bool enabled;
	bool focusable;
	bool (*hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*subframes)(indefinite<frame> fm, array<handleable<frame>> *frames);
	vector<uint32, 2> (*content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void (*render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*mouse_click)(indefinite<frame> fm);
	bool hook_mouse_click;
	bool return_mouse_click;
	void (*mouse_release)(indefinite<frame> fm);
	bool hook_mouse_release;
	bool return_mouse_release;
	void (*mouse_move)(indefinite<frame> fm);
	bool hook_mouse_move;
	bool return_mouse_move;
	void (*start_hover)(indefinite<frame> fm);
	void (*end_hover)(indefinite<frame> fm);
	void (*focus_receive)(indefinite<frame> fm);
	void (*focus_loss)(indefinite<frame> fm);
	void (*mouse_wheel_rotate)(indefinite<frame> fm);
	bool hook_mouse_wheel_rotate;
	bool return_mouse_wheel_rotate;
	void (*key_press)(indefinite<frame> fm);
	void (*key_release)(indefinite<frame> fm);
	void (*char_input)(indefinite<frame> fm);

	frame();
};
