#include "frame_templates.h"

void scroll_bar_data::attach(handleable<frame> fm)
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

void scroll_bar_model::attach(handleable<frame> fm)
{
	fm.core->width_desc = 12uiabs;
	fm.core->height_desc = 12uiabs;
	fm.core->margin_left = 0uiabs;
	fm.core->margin_bottom = 0uiabs;
	fm.core->margin_right = 0uiabs;
	fm.core->margin_top = 0uiabs;
	fm.core->padding_left = 2uiabs;
	fm.core->padding_bottom = 2uiabs;
	fm.core->padding_right = 2uiabs;
	fm.core->padding_top = 2uiabs;
	fm.core->return_mouse_wheel_rotate = true;
}

void scroll_bar_model::render(handleable<frame> fm, scroll_bar_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	if(!fm.core->visible || data->viewport_size >= data->content_size) return;
	rectangle<int32> content_viewport = utility<frame>().frame_content_viewport(fm.core);
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

bool scroll_bar_hit_test(indefinite<frame> fm, vector<int32, 2> point)
{
	scroll_bar *sb = (scroll_bar *)(fm.addr);
	return utility<frame>().rectangular_hit_test(&sb->fm, point);
}

void scroll_bar_render(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	scroll_bar *sb = (scroll_bar *)(fm.addr);
	sb->model.render(handleable<frame>(sb, &sb->fm), &sb->data, point, bp, bmp);
}

scroll_bar::scroll_bar()
{
	fm.hit_test = scroll_bar_hit_test;
	fm.render = scroll_bar_render;
	data.attach(handleable<frame>(this, &fm));
	model.attach(handleable<frame>(this, &fm));
}

struct scroll_bar_module_initializer
{
	scroll_bar_module_initializer()
	{
		default_frame_callbacks()->scroll_bar_hit_test = scroll_bar_hit_test;
		default_frame_callbacks()->scroll_bar_render = scroll_bar_render;
	}
} initializer;
