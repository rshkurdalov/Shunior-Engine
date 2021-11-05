#include "ui_ext.h"

ui_size operator""uiabs(uint64 value)
{
	return ui_size(ui_size_type::absolute, real((uint32)value));
}

ui_size operator""uirel(const char8 *str)
{
	return ui_size(ui_size_type::relative, operator""r(str));
}

ui_size operator""uiauto(uint64 value)
{
	return ui_size(ui_size_type::autosize, real((uint32)value));
}

uint32 resolve_ui_size(ui_size size, uint32 related_size)
{
	if(size.type == ui_size_type::relative)
		size.value *= real(related_size);
	return size.value.integer;
}

rectangle<int32> frame_viewport(frame *fm)
{
	rectangle<int32> rect;
	rect.position.x = fm->x + int32(resolve_ui_size(fm->margin_left, fm->width));
	rect.position.y = fm->y + int32(resolve_ui_size(fm->margin_bottom, fm->height));
	rect.extent.x = fm->width - (rect.position.x - fm->x)
		- int32(resolve_ui_size(fm->margin_right, fm->width));
	rect.extent.y = fm->height - (rect.position.y - fm->y)
		- int32(resolve_ui_size(fm->margin_top, fm->height));
	return rect;
}

rectangle<int32> frame_content_viewport(frame *fm)
{
	rectangle<int32> rect;
	rect.position.x = fm->x
		+ int32(resolve_ui_size(fm->margin_left, fm->width))
		+ int32(resolve_ui_size(fm->padding_left, fm->width));
	rect.position.y = fm->y
		+ int32(resolve_ui_size(fm->margin_bottom, fm->height))
		+ int32(resolve_ui_size(fm->padding_bottom, fm->height));
	rect.extent.x = fm->width - (rect.position.x - fm->x)
		- int32(resolve_ui_size(fm->margin_right, fm->width))
		- int32(resolve_ui_size(fm->padding_right, fm->width));
	rect.extent.y = fm->height - (rect.position.y - fm->y)
		- int32(resolve_ui_size(fm->margin_top, fm->height))
		- int32(resolve_ui_size(fm->padding_top, fm->height));
	return rect;
}

vector<uint32, 2> content_size_to_frame_size(frame *fm, uint32 content_width, uint32 content_height)
{
	vector<uint32, 2> size(content_width, content_height);
	real denominator = 1.0r;
	denominator = 1.0r;
	if(fm->margin_left.type == ui_size_type::relative)
		denominator -= fm->margin_left.value;
	else size.x += fm->margin_left.value.integer;
	if(fm->padding_left.type == ui_size_type::relative)
		denominator -= fm->padding_left.value;
	else size.x += fm->padding_left.value.integer;
	if(fm->margin_right.type == ui_size_type::relative)
		denominator -= fm->margin_right.value;
	else size.x += fm->margin_right.value.integer;
	if(fm->padding_right.type == ui_size_type::relative)
		denominator -= fm->padding_right.value;
	else size.x += fm->padding_right.value.integer;
	size.x = (real(size.x) / denominator).integer;
	denominator = 1.0r;
	if(fm->margin_bottom.type == ui_size_type::relative)
		denominator -= fm->margin_bottom.value;
	else size.y += fm->margin_bottom.value.integer;
	if(fm->padding_bottom.type == ui_size_type::relative)
		denominator -= fm->padding_bottom.value;
	else size.y += fm->padding_bottom.value.integer;
	if(fm->margin_top.type == ui_size_type::relative)
		denominator -= fm->margin_top.value;
	else size.y += fm->margin_top.value.integer;
	if(fm->padding_top.type == ui_size_type::relative)
		denominator -= fm->padding_top.value;
	else size.y += fm->padding_top.value.integer;
	size.y = (real(size.y) / denominator).integer;
	return size;
}

vector<uint32, 2> evaluate_frame_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	vector<uint32, 2> size, content_size;
	size.x = resolve_ui_size(fm->width_desc, viewport_width);
	size.x = min(max(fm->min_width, size.x), fm->max_width);
	size.y = resolve_ui_size(fm->height_desc, viewport_height);
	size.y = min(max(fm->min_height, size.y), fm->max_height);
	if(fm->width_desc.type == ui_size_type::autosize
		|| fm->height_desc.type == ui_size_type::autosize)
	{
		if(fm->width_desc.type == ui_size_type::autosize)
			viewport_width = fm->max_width;
		else viewport_width = size.x;
		viewport_width -= resolve_ui_size(fm->margin_left, viewport_width)
			+ resolve_ui_size(fm->padding_left, viewport_width)
			+ resolve_ui_size(fm->margin_right, viewport_width)
			+ resolve_ui_size(fm->padding_right, viewport_width);
		if(fm->height_desc.type == ui_size_type::autosize)
			viewport_height = fm->max_height;
		else viewport_height = size.y;
		viewport_height -= resolve_ui_size(fm->margin_bottom, viewport_height)
			+ resolve_ui_size(fm->padding_bottom, viewport_height)
			+ resolve_ui_size(fm->margin_top, viewport_height)
			+ resolve_ui_size(fm->padding_top, viewport_height);
		content_size = fm->content_size(fm, viewport_width, viewport_height);
		content_size = content_size_to_frame_size(fm, content_size.x, content_size.y);
		if(fm->width_desc.type == ui_size_type::autosize)
		{
			size.x = content_size.x;
			size.x = min(max(fm->min_width, size.x), fm->max_width);
		}
		if(fm->height_desc.type == ui_size_type::autosize)
		{
			size.y = content_size.y;
			size.y = min(max(fm->min_height, size.y), fm->max_height);
		}
	}
	return size;
}

void frame_global_subframes(frame *fm, array<frame *> *frames)
{
	uint64 idx_begin = frames->size, idx_end;
	fm->subframes(fm, frames);
	idx_end = frames->size;
	while(idx_begin != idx_end)
		frame_global_subframes(frames->addr[idx_begin++], frames);
}

bool frame_hit_test_def(frame *fm, vector<int32, 2> point)
{
	return point.x >= fm->x && point.x < fm->x + int32(fm->width)
		&& point.y >= fm->y && point.y < fm->y + int32(fm->height);
}

void frame_subframes_def(frame *fm, array<frame *> *frames)
{
	return;
}

vector<uint32, 2> frame_content_size_def(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	rectangle<int32> size = frame_content_viewport(fm);
	return vector<uint32, 2>(uint32(size.extent.x), uint32(size.extent.y));
}

void frame_render_def(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	return;
}

void frame_mouse_click_def(frame *fm)
{
	return;
}

void frame_mouse_release_def(frame *fm)
{
	return;
}

void frame_mouse_move_def(frame *fm)
{
	return;
}

void frame_start_hover_def(frame *fm)
{
	return;
}

void frame_end_hover_def(frame *fm)
{
	return;
}

void frame_focus_receive_def(frame *fm)
{
	return;
}

void frame_focus_loss_def(frame *fm)
{
	return;
}

void frame_mouse_wheel_rotate_def(frame *fm)
{
	return;
}

void frame_key_press_def(frame *fm)
{
	return;
}

void frame_key_release_def(frame *fm)
{
	return;
}

void frame_char_input_def(frame *fm)
{
	return;
}
