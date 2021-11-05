#include "frame_templates.h"

scroll_bar::scroll_bar()
{
	fm.data = this;
	fm.width_desc = 12uiabs;
	fm.height_desc = 12uiabs;
	fm.margin_left = 0uiabs;
	fm.margin_bottom = 0uiabs;
	fm.margin_right = 0uiabs;
	fm.margin_top = 0uiabs;
	fm.padding_left = 2uiabs;
	fm.padding_bottom = 2uiabs;
	fm.padding_right = 2uiabs;
	fm.padding_top = 2uiabs;
	fm.render = scroll_bar_render;
	fm.return_mouse_wheel_rotate = true;
	vertical = true;
	content_size = 0;
	viewport_size = 0;
	viewport_offset = 0;
}

void scroll_bar::shift(uint32 value, bool forward)
{
	if(forward)
	{
		if(viewport_offset < value)
			viewport_offset = 0;
		else viewport_offset -= value;
	}
	else viewport_offset += value;
}

void scroll_bar_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	scroll_bar *sb = (scroll_bar *)fm->data;
	if(!sb->fm.visible || sb->viewport_size >= sb->content_size) return;
	rectangle<int32> content_viewport = frame_content_viewport(&sb->fm);
	sb->viewport_offset = min(sb->viewport_offset, sb->content_size - sb->viewport_size);
	int32 slider_size = (sb->vertical ? content_viewport.extent.y : content_viewport.extent.x)
		* sb->viewport_size / sb->content_size;
	slider_size = min(max(slider_size, 6), (sb->vertical ? content_viewport.extent.y : content_viewport.extent.x));
	int32 slider_offset = ((sb->vertical ? content_viewport.extent.y : content_viewport.extent.x) - slider_size)
		* sb->viewport_offset / (sb->content_size - sb->viewport_size);
	geometry_path path;
	rectangle<int32> slider_rect;
	if(sb->vertical)
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
	path.push_rectangle(rectangle<real>(slider_rect));
	bp->rasterization = rasterization_mode::fill;
	set_identity_matrix(&bp->transform);
	bp->brush = brush_type::solid;
	bp->color = alpha_color(127, 127, 127, 255);
	bp->render(path, bmp);
}
