#include "frame_templates.h"

void scroll_bar_data::attach(frame *fm)
{
	vertical = true;
	content_size = 0;
	viewport_size = 0;
	viewport_offset = 0;
}

void scroll_bar_data::shift(uint32 value, bool forward)
{
	if(forward)
	{
		if(viewport_offset < value)
			viewport_offset = 0;
		else viewport_offset -= value;
	}
	else viewport_offset += value;
}

void scroll_bar_model::attach(frame *fm)
{
	fm->width_desc = 12uiabs;
	fm->height_desc = 12uiabs;
	fm->margin_left = 0uiabs;
	fm->margin_bottom = 0uiabs;
	fm->margin_right = 0uiabs;
	fm->margin_top = 0uiabs;
	fm->padding_left = 2uiabs;
	fm->padding_bottom = 2uiabs;
	fm->padding_right = 2uiabs;
	fm->padding_top = 2uiabs;
	fm->return_mouse_wheel_rotate = true;
}

void scroll_bar_model::render(frame *fm, scroll_bar_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	if(!fm->visible || data->viewport_size >= data->content_size) return;
	rectangle<int32> content_viewport = frame_content_viewport(fm);
	data->viewport_offset = min(data->viewport_offset, data->content_size - data->viewport_size);
	int32 slider_size = (data->vertical ? content_viewport.extent.y : content_viewport.extent.x)
		* data->viewport_size / data->content_size;
	slider_size = min(max(slider_size, 6), (data->vertical ? content_viewport.extent.y : content_viewport.extent.x));
	int32 slider_offset = ((data->vertical ? content_viewport.extent.y : content_viewport.extent.x) - slider_size)
		* data->viewport_offset / (data->content_size - data->viewport_size);
	rectangle<int32> slider_rect;
	if(data->vertical)
	{
		slider_rect.position = vector<int32, 2>(
			content_viewport.position.x - point.x,
			content_viewport.position.y + content_viewport.extent.y - slider_offset - slider_size - point.y);
		slider_rect.extent = vector<int32, 2>(content_viewport.extent.x, slider_size);
	}
	else
	{
		slider_rect.position = vector<int32, 2>(
			content_viewport.position.x + slider_offset - point.x,
			content_viewport.position.y - point.y);
		slider_rect.extent = vector<int32, 2>(slider_size, content_viewport.extent.y);
	}
	bp->br.switch_solid_color(alpha_color(127, 127, 127, 255));
	bp->fill_area(slider_rect, bmp);
}

void scroll_bar_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	scroll_bar *sb = (scroll_bar *)(fm->data);
	sb->model.render(fm, &sb->data, point, bp, bmp);
}

scroll_bar::scroll_bar()
{
	fm.data = this;
	fm.render = scroll_bar_render;
	data.attach(&fm);
	model.attach(&fm);
}
