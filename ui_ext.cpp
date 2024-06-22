#include "ui_ext.h"

adaptive_size<real> operator""uiabs(uint64 value)
{
	return adaptive_size<real>(adaptive_size_type::absolute, real(uint32(value)));
}

adaptive_size<real> operator""uirel(const char8 *str)
{
	return adaptive_size<real>(adaptive_size_type::relative, operator""r(str));
}

adaptive_size<real> operator""uiauto(uint64 value)
{
	return adaptive_size<real>(adaptive_size_type::autosize, real(uint32(value)));
}

uint32 utility<frame>::resolve_size(adaptive_size<real> size, uint32 dependent_size)
{
	if(size.type == adaptive_size_type::relative)
		size.value *= real(dependent_size);
	return uint32(size.value);
}

rectangle<int32> utility<frame>::frame_viewport(frame *fm)
{
	rectangle<int32> rect;
	rect.position.x = fm->x + int32(utility<frame>().resolve_size(fm->margin_left, fm->width));
	rect.position.y = fm->y + int32(utility<frame>().resolve_size(fm->margin_bottom, fm->height));
	rect.extent.x = int32(fm->width) - (rect.position.x - fm->x)
		- int32(utility<frame>().resolve_size(fm->margin_right, fm->width));
	rect.extent.y = int32(fm->height) - (rect.position.y - fm->y)
		- int32(utility<frame>().resolve_size(fm->margin_top, fm->height));
	return rect;
}

rectangle<int32> utility<frame>::frame_content_viewport(frame *fm)
{
	rectangle<int32> rect;
	rect.position.x = fm->x
		+ int32(utility<frame>().resolve_size(fm->margin_left, fm->width))
		+ int32(utility<frame>().resolve_size(fm->padding_left, fm->width));
	rect.position.y = fm->y
		+ int32(utility<frame>().resolve_size(fm->margin_bottom, fm->height))
		+ int32(utility<frame>().resolve_size(fm->padding_bottom, fm->height));
	rect.extent.x = fm->width - (rect.position.x - fm->x)
		- int32(utility<frame>().resolve_size(fm->margin_right, fm->width))
		- int32(utility<frame>().resolve_size(fm->padding_right, fm->width));
	rect.extent.y = fm->height - (rect.position.y - fm->y)
		- int32(utility<frame>().resolve_size(fm->margin_top, fm->height))
		- int32(utility<frame>().resolve_size(fm->padding_top, fm->height));
	return rect;
}

vector<uint32, 2> utility<frame>::content_size_to_frame_size(frame *fm, uint32 content_width, uint32 content_height)
{
	vector<uint32, 2> size(content_width, content_height);
	real denominator = 1.0r;
	if(fm->margin_left.type == adaptive_size_type::relative)
		denominator -= fm->margin_left.value;
	else size.x += fm->margin_left.value.integer;
	if(fm->padding_left.type == adaptive_size_type::relative)
		denominator -= fm->padding_left.value;
	else size.x += fm->padding_left.value.integer;
	if(fm->margin_right.type == adaptive_size_type::relative)
		denominator -= fm->margin_right.value;
	else size.x += fm->margin_right.value.integer;
	if(fm->padding_right.type == adaptive_size_type::relative)
		denominator -= fm->padding_right.value;
	else size.x += fm->padding_right.value.integer;
	size.x = (real(size.x) / denominator).integer;
	denominator = 1.0r;
	if(fm->margin_bottom.type == adaptive_size_type::relative)
		denominator -= fm->margin_bottom.value;
	else size.y += fm->margin_bottom.value.integer;
	if(fm->padding_bottom.type == adaptive_size_type::relative)
		denominator -= fm->padding_bottom.value;
	else size.y += fm->padding_bottom.value.integer;
	if(fm->margin_top.type == adaptive_size_type::relative)
		denominator -= fm->margin_top.value;
	else size.y += fm->margin_top.value.integer;
	if(fm->padding_top.type == adaptive_size_type::relative)
		denominator -= fm->padding_top.value;
	else size.y += fm->padding_top.value.integer;
	size.y = (real(size.y) / denominator).integer;
	return size;
}

vector<uint32, 2> utility<frame>::frame_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height)
{
	vector<uint32, 2> size, content_size;
	size.x = utility<frame>().resolve_size(fm.core->width_desc, viewport_width);
	size.x = min(max(fm.core->min_width, size.x), fm.core->max_width);
	size.y = utility<frame>().resolve_size(fm.core->height_desc, viewport_height);
	size.y = min(max(fm.core->min_height, size.y), fm.core->max_height);
	if(fm.core->width_desc.type == adaptive_size_type::autosize
		|| fm.core->height_desc.type == adaptive_size_type::autosize)
	{
		if(fm.core->width_desc.type == adaptive_size_type::autosize)
			viewport_width = fm.core->max_width;
		else viewport_width = size.x;
		viewport_width -= utility<frame>().resolve_size(fm.core->margin_left, viewport_width)
			+ utility<frame>().resolve_size(fm.core->padding_left, viewport_width)
			+ utility<frame>().resolve_size(fm.core->margin_right, viewport_width)
			+ utility<frame>().resolve_size(fm.core->padding_right, viewport_width);
		if(fm.core->height_desc.type == adaptive_size_type::autosize)
			viewport_height = fm.core->max_height;
		else viewport_height = size.y;
		viewport_height -= utility<frame>().resolve_size(fm.core->margin_bottom, viewport_height)
			+ utility<frame>().resolve_size(fm.core->padding_bottom, viewport_height)
			+ utility<frame>().resolve_size(fm.core->margin_top, viewport_height)
			+ utility<frame>().resolve_size(fm.core->padding_top, viewport_height);
		content_size = fm.core->content_size(fm.object, viewport_width, viewport_height);
		content_size = utility<frame>().content_size_to_frame_size(fm.core, content_size.x, content_size.y);
		if(fm.core->width_desc.type == adaptive_size_type::autosize)
		{
			size.x = content_size.x;
			size.x = min(max(fm.core->min_width, size.x), fm.core->max_width);
		}
		if(fm.core->height_desc.type == adaptive_size_type::autosize)
		{
			size.y = content_size.y;
			size.y = min(max(fm.core->min_height, size.y), fm.core->max_height);
		}
	}
	return size;
}

void utility<frame>::cascade_frames(handleable<frame> root_fm, array<handleable<frame>> *frames)
{
	uint64 idx_begin = frames->size, idx_end;
	root_fm.core->subframes(root_fm.object, frames);
	idx_end = frames->size;
	while(idx_begin != idx_end)
		cascade_frames(frames->addr[idx_begin++], frames);
}

bool utility<frame>::rectangular_hit_test(frame *fm, vector<int32, 2> point)
{
	rectangle<int32> viewport = utility<frame>().frame_viewport(fm);
	return point.x >= viewport.position.x
		&& point.x < viewport.position.x + viewport.extent.x
		&& point.y >= viewport.position.y
		&& point.y < viewport.position.y + viewport.extent.y;
}
