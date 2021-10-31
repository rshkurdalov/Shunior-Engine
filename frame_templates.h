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

struct scroll_bar
{
	frame fm;
	bool vertical;
	uint32 content_size;
	uint32 viewport_size;
	uint32 viewport_offset;

	scroll_bar();
	void shift(uint32 value, bool forward);
};

void scroll_bar_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);

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

struct flow_layout
{
	frame fm;
	array<flow_layout_frame> frames;
	flow_axis direction;
	flow_offset offset;
	bool multiline;
	scroll_bar xscroll;
	scroll_bar yscroll;

	flow_layout();
};

void flow_layout_subframes(frame *fm, array<frame *> *frames);
vector<uint32, 2> flow_layout_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
void flow_layout_update_layout(frame *fm);
void flow_layout_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
void flow_layout_mouse_wheel_rotate(frame *fm);

struct text_field
{
	frame fm;
	string font;
	real font_size;
	text_layout tl;
	uint64 caret;
	uint64 select_caret;
	bool selecting;
	bool editable;
	scroll_bar scroll;

	text_field();
	void select(uint64 idx_begin, uint64 idx_end);
	void deselect(uint64 return_idx);
	void insert(string &text);
	void remove();
	void scroll_to_caret();
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
