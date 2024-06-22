#pragma once
#include "frame.h"
#include "ui_ext.h"
#include "hardware.h"
#include "time.h"
#include "text_layout.h"

void hover_frame(handleable<frame> fm);
handleable<frame> hovered_frame();
void focus_frame(handleable<frame> fm);
handleable<frame> focused_frame();
void pull_frame(handleable<frame> fm);
handleable<frame> pulled_frame();

struct window
{
	frame fm;
	bitmap bmp;
	handleable<frame> layout;
	void *handler;

	window();
	~window();
	void update();
	void open();
	void close();
	void hide();
};

struct scroll_bar_data
{
	bool vertical;
	uint32 content_size;
	uint32 viewport_size;
	uint32 viewport_offset;

	void attach(handleable<frame> fm);
	void shift(uint32 value, bool forward);
};

struct scroll_bar_model
{
	void attach(handleable<frame> fm);
	void render(handleable<frame> fm, scroll_bar_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

struct scroll_bar
{
	frame fm;
	scroll_bar_data data;
	scroll_bar_model model;

	scroll_bar();
};

struct layout_model
{
	alpha_color background_color;

	void attach(handleable<frame> fm);
	void render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

struct flow_layout_frame
{
	handleable<frame> fm;
	horizontal_align halign;
	vertical_align valign;
	bool line_break;

	flow_layout_frame() {}

	flow_layout_frame(
		handleable<frame> fm,
		horizontal_align halign,
		vertical_align valign,
		bool line_break)
		: fm(fm),
		halign(halign),
		valign(valign),
		line_break(line_break) {}
};

struct flow_layout_data
{
	array<flow_layout_frame> frames;
	flow_axis direction;
	flow_offset offset;
	bool multiline;
	scroll_bar xscroll;
	scroll_bar yscroll;

	void attach(handleable<frame> fm);
	void subframes(handleable<frame> fm, array<handleable<frame>> *frames_addr);
	vector<uint32, 2> content_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void update_layout(handleable<frame> fm);
	void render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void mouse_wheel_rotate(handleable<frame> fm);
};

struct flow_layout
{
	frame fm;
	flow_layout_data data;
	layout_model model;

	flow_layout();
};

struct grid_layout_frame
{
	handleable<frame> fm;
	horizontal_align halign;
	vertical_align valign;

	grid_layout_frame() {}

	grid_layout_frame(
		handleable<frame> fm,
		horizontal_align halign,
		vertical_align valign)
		: fm(fm),
		halign(halign),
		valign(valign) {}
};

struct grid_layout_data
{
	array<adaptive_size<real>> rows_size;
	array<adaptive_size<real>> columns_size;
	dynamic_matrix<grid_layout_frame> frames;
	uint64 growth_row;
	uint64 growth_column;
	scroll_bar xscroll;
	scroll_bar yscroll;

	void attach(handleable<frame> fm);
	void insert_row(uint32 insert_idx, adaptive_size<real> size);
	void remove_row(uint32 remove_idx);
	void insert_column(uint32 insert_idx, adaptive_size<real> size);
	void remove_column(uint32 remove_idx);
	void subframes(handleable<frame> fm, array<handleable<frame>> *frames_addr);
	vector<uint32, 2> content_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void update_layout(handleable<frame> fm);
	void render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void mouse_wheel_rotate(handleable<frame> fm);
};

struct grid_layout
{
	frame fm;
	grid_layout_data data;
	layout_model model;

	grid_layout();
};

struct text_field_data
{
	string font;
	uint32 font_size;
	text_layout tl;
	uint64 caret;
	uint64 select_caret;
	bool selecting;
	bool selection_visible;
	bool editable;
	scroll_bar scroll;

	void attach(handleable<frame> fm);
	void select(uint64 idx_begin, uint64 idx_end);
	void deselect(uint64 return_idx);
	void insert(string &text);
	void remove();
	void scroll_to_caret(handleable<frame> fm);
	void subframes(handleable<frame> fm, array<handleable<frame> > *frames);
	vector<uint32, 2> content_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void mouse_click(handleable<frame> fm);
	void mouse_move(handleable<frame> fm);
	void focus_receive(handleable<frame> fm);
	void focus_loss(handleable<frame> fm);
	void mouse_wheel_rotate(handleable<frame> fm);
	void key_press(handleable<frame> fm);
	void char_input(handleable<frame> fm);
};

struct text_field_model
{
	bitmap surface;
	vector<uint32, 2> rendering_size;

	void attach(handleable<frame> fm);
	void render(handleable<frame> fm, text_field_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

struct text_field
{
	frame fm;
	text_field_data data;
	text_field_model model;

	text_field();
};

struct push_button_data
{
	void (*button_click)(void *data);
	void *data;

	void attach(text_field_data *tf_data);
	void mouse_click(handleable<frame> fm);
	void mouse_release(handleable<frame> fm);
};

struct push_button_model
{
	bitmap inner_surface;
	bitmap border_surface;
	vector<uint32, 2> rendering_size;
	
	void attach(handleable<frame> fm);
	void render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

struct push_button
{
	frame fm;
	text_field_data tf_data;
	push_button_data pb_data;
	push_button_model model;

	push_button();
};

struct default_frame_callback_set
{
	bool (*frame_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	vector<uint32, 2> (*frame_content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);

	void (*window_mouse_click)(indefinite<frame> fm);
	void (*window_mouse_release)(indefinite<frame> fm);
	void (*window_mouse_move)(indefinite<frame> fm);
	void (*window_mouse_wheel_rotate)(indefinite<frame> fm);
	void (*window_key_press)(indefinite<frame> fm);
	void (*window_key_release)(indefinite<frame> fm);
	void (*window_char_input)(indefinite<frame> fm);

	bool (*scroll_bar_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*scroll_bar_render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);

	bool (*flow_layout_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*flow_layout_subframes)(indefinite<frame> fm, array<handleable<frame>> *frames);
	vector<uint32, 2> (*flow_layout_content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void (*flow_layout_render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*flow_layout_mouse_wheel_rotate)(indefinite<frame> fm);

	bool (*grid_layout_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*grid_layout_subframes)(indefinite<frame> fm, array<handleable<frame>> *frames);
	vector<uint32, 2> (*grid_layout_content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void (*grid_layout_render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*grid_layout_mouse_wheel_rotate)(indefinite<frame> fm);

	bool (*text_field_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*text_field_subframes)(indefinite<frame> fm, array<handleable<frame>> *frames);
	vector<uint32, 2> (*text_field_content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void (*text_field_render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*text_field_mouse_click)(indefinite<frame> fm);
	void (*text_field_mouse_move)(indefinite<frame> fm);
	void (*text_field_focus_receive)(indefinite<frame> fm);
	void (*text_field_focus_loss)(indefinite<frame> fm);
	void (*text_field_mouse_wheel_rotate)(indefinite<frame> fm);
	void (*text_field_key_press)(indefinite<frame> fm);
	void (*text_field_char_input)(indefinite<frame> fm);

	bool (*push_button_hit_test)(indefinite<frame> fm, vector<int32, 2> point);
	void (*push_button_subframes)(indefinite<frame> fm, array<handleable<frame>> *frames);
	vector<uint32, 2> (*push_button_content_size)(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height);
	void (*push_button_render)(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void (*push_button_mouse_click)(indefinite<frame> fm);
	void (*push_button_mouse_release)(indefinite<frame> fm);
	void (*push_button_mouse_move)(indefinite<frame> fm);
	void (*push_button_focus_receive)(indefinite<frame> fm);
	void (*push_button_focus_loss)(indefinite<frame> fm);
	void (*push_button_mouse_wheel_rotate)(indefinite<frame> fm);
	void (*push_button_key_press)(indefinite<frame> fm);
	void (*push_button_char_input)(indefinite<frame> fm);

	default_frame_callback_set();
};

default_frame_callback_set *default_frame_callbacks();
