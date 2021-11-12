#pragma once
#include "frame.h"
#include "ui_ext.h"
#include "hardware.h"
#include "time.h"
#include "text_layout.h"

void hover_frame(frame *fm);
frame *hovered_frame();
void focus_frame(frame *fm);
frame *focused_frame();
void pull_frame(frame *fm);
frame *pulled_frame();

struct window
{
	frame fm;
	bitmap bmp;
	frame *layout;
	void *handler;

	window();
	~window();
	void update();
	void open();
	void close();
	void hide();
};

void window_mouse_click(frame *fm);
void window_mouse_release(frame *fm);
void window_mouse_move(frame *fm);
void window_mouse_wheel_rotate(frame *fm);
void window_key_press(frame *fm);
void window_key_release(frame *fm);
void window_char_input(frame *fm);

struct scroll_bar_data
{
	bool vertical;
	uint32 content_size;
	uint32 viewport_size;
	uint32 viewport_offset;

	void attach(frame *fm);
	void shift(uint32 value, bool forward);
};

struct scroll_bar_model
{
	void attach(frame *fm);
	void render(frame *fm, scroll_bar_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

void scroll_bar_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);

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

	void attach(frame *fm);
	void render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

struct flow_layout_frame
{
	frame *fm;
	horizontal_align halign;
	vertical_align valign;
	bool line_break;

	flow_layout_frame(
		frame *fm,
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

	void attach(frame *fm);
	void subframes(frame *fm, array<frame *> *frames_addr);
	vector<uint32, 2> content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
	void update_layout(frame *fm);
	void render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void mouse_wheel_rotate(frame *fm);
};

void flow_layout_subframes(frame *fm, array<frame *> *frames);
vector<uint32, 2> flow_layout_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
void flow_layout_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
void flow_layout_mouse_wheel_rotate(frame *fm);

struct flow_layout
{
	frame fm;
	flow_layout_data data;
	layout_model model;

	flow_layout();
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

	void attach(frame *fm);
	void select(uint64 idx_begin, uint64 idx_end);
	void deselect(uint64 return_idx);
	void insert(string &text);
	void remove();
	void scroll_to_caret(frame *fm);
	void subframes(frame *fm, array<frame *> *frames);
	vector<uint32, 2> content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
	void render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
	void mouse_click(frame *fm);
	void mouse_move(frame *fm);
	void focus_receive(frame *fm);
	void focus_loss(frame *fm);
	void mouse_wheel_rotate(frame *fm);
	void key_press(frame *fm);
	void char_input(frame *fm);
};


struct text_field_model
{
	bitmap surface;
	vector<uint32, 2> rendered_size;

	void attach(frame *fm);
	void render(frame *fm, text_field_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};


void text_field_subframes(frame *fm, array<frame *> *frames);
vector<uint32, 2> text_field_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
void text_field_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
void text_field_mouse_click(frame *fm);
void text_field_mouse_move(frame *fm);
void text_field_focus_receive(frame *fm);
void text_field_focus_loss(frame *fm);
void text_field_mouse_wheel_rotate(frame *fm);
void text_field_key_press(frame *fm);
void text_field_char_input(frame *fm);

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
	void mouse_click(frame *fm);
	void mouse_release(frame *fm);
};

struct push_button_model
{
	bitmap inner_surface;
	bitmap border_surface;
	vector<uint32, 2> rendered_size;
	
	void attach(frame *fm);
	void render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
};

void push_button_subframes(frame *fm, array<frame *> *frames);
vector<uint32, 2> push_button_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
void push_button_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
void push_button_mouse_click(frame *fm);
void push_button_mouse_release(frame *fm);
void push_button_mouse_move(frame *fm);
void push_button_focus_receive(frame *fm);
void push_button_focus_loss(frame *fm);
void push_button_mouse_wheel_rotate(frame *fm);
void push_button_key_press(frame *fm);
void push_button_char_input(frame *fm);

struct push_button
{
	frame fm;
	text_field_data tf_data;
	push_button_data pb_data;
	push_button_model model;

	push_button();
};
