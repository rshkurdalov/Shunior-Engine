#include "frame_templates.h"

frame *hovered_frame_addr;
frame *focused_frame_addr;
frame *pulled_frame_addr;

void hover_frame(frame *fm)
{
	if(hovered_frame_addr == fm) return;
	if(hovered_frame_addr != nullptr)
		hovered_frame_addr->end_hover(hovered_frame_addr);
	hovered_frame_addr = fm;
	if(hovered_frame_addr != nullptr)
		hovered_frame_addr->start_hover(hovered_frame_addr);
}

frame *hovered_frame()
{
	return hovered_frame_addr;
}

void focus_frame(frame *fm)
{
	if(focused_frame_addr == fm) return;
	if(focused_frame_addr != nullptr)
		focused_frame_addr->focus_loss(focused_frame_addr);
	focused_frame_addr = fm;
	if(focused_frame_addr != nullptr)
		focused_frame_addr->focus_receive(focused_frame_addr);
}

frame *focused_frame()
{
	return focused_frame_addr;
}

void pull_frame(frame *fm)
{
	pulled_frame_addr = fm;
}

frame *pulled_frame()
{
	return pulled_frame_addr;
}

void layout_model::attach(frame *fm)
{
	fm->padding_left = 0uiabs;
	fm->padding_bottom = 0uiabs;
	fm->padding_right = 0uiabs;
	fm->padding_top = 0uiabs;
}

void layout_model::render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{

}
