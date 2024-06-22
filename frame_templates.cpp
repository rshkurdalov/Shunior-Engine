#include "frame_templates.h"

handleable<frame> hovered_frame_ref;
handleable<frame> focused_frame_ref;
handleable<frame> pulled_frame_ref;

void hover_frame(handleable<frame> fm)
{
	if(hovered_frame_ref.object.addr == fm.object.addr) return;
	if(hovered_frame_ref.object.addr != nullptr)
		hovered_frame_ref.core->end_hover(hovered_frame_ref.object);
	hovered_frame_ref = fm;
	if(hovered_frame_ref.object.addr != nullptr)
		hovered_frame_ref.core->start_hover(hovered_frame_ref.object);
}

handleable<frame> hovered_frame()
{
	return hovered_frame_ref;
}

void focus_frame(handleable<frame> fm)
{
	if(focused_frame_ref.object.addr == fm.object.addr) return;
	if(focused_frame_ref.object.addr != nullptr)
		focused_frame_ref.core->focus_loss(focused_frame_ref.object);
	focused_frame_ref = fm;
	if(focused_frame_ref.object.addr != nullptr)
		focused_frame_ref.core->focus_receive(focused_frame_ref.object);
}

handleable<frame> focused_frame()
{
	return focused_frame_ref;
}

void pull_frame(handleable<frame> fm)
{
	pulled_frame_ref = fm;
}

handleable<frame> pulled_frame()
{
	return pulled_frame_ref;
}

void layout_model::attach(handleable<frame> fm)
{
	background_color = alpha_color(0, 0, 0, 0);
	fm.core->padding_left = 0uiabs;
	fm.core->padding_bottom = 0uiabs;
	fm.core->padding_right = 0uiabs;
	fm.core->padding_top = 0uiabs;
}

void layout_model::render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle viewport = utility<frame>().frame_viewport(fm.core),
		content_viewport = utility<frame>().frame_content_viewport(fm.core);
	viewport.position -= point;
	if(!fm.core->visible
		|| background_color.a == 0
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	bp->br.switch_solid_color(background_color);
	bp->fill_area(viewport, bmp);
}

bool default_frame_hit_test(indefinite<frame> fm, vector<int32, 2> point)
{
	return false;
}

vector<uint32, 2> default_frame_content_size(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height)
{
	return vector<uint32, 2>(0, 0);
}

default_frame_callback_set::default_frame_callback_set()
{
	frame_hit_test = default_frame_hit_test;
	frame_content_size = default_frame_content_size;
}

default_frame_callback_set dfcs;

default_frame_callback_set *default_frame_callbacks()
{
	return &dfcs;
}
