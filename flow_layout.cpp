#include "frame_templates.h"

flow_layout::flow_layout()
{
	fm.data = this;
	fm.padding_left = 0uiabs;
	fm.padding_bottom = 0uiabs;
	fm.padding_right = 0uiabs;
	fm.padding_top = 0uiabs;
	fm.subframes = flow_layout_subframes;
	fm.content_size = flow_layout_content_size;
	fm.render = flow_layout_render;
	fm.mouse_wheel_rotate = flow_layout_mouse_wheel_rotate;
	direction = flow_axis::x;
	offset = flow_offset::right;
	multiline = true;
	xscroll.vertical = false;
}

void flow_layout_subframes(frame *fm, array<frame *> *frames)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	frames->push(&fl->xscroll.fm);
	frames->push(&fl->yscroll.fm);
	for(uint64 i = 0; i < fl->frames.size; i++)
		frames->push(fl->frames.addr[i].fm);
}

struct flow_layout_line_metrics
{
	uint64 frame_count;
	int32 linespace;
	int32 size1;
	int32 size2;
	int32 size3;

	flow_layout_line_metrics()
		: frame_count(0), linespace(0),
		size1(0), size2(0), size3(0) {}
};

struct flow_layout_metrics
{
	flow_layout *fl;
	int32 viewport_width;
	int32 viewport_height;
	int32 content_width;
	int32 content_height;
	array<flow_layout_line_metrics> line_metrics;

	flow_layout_metrics(flow_layout *fl, int32 viewport_width, int32 viewport_height)
		: fl(fl), viewport_width(viewport_width), viewport_height(viewport_height)
	{
		content_width = 0;
		content_height = 0;
	}
};

void flow_layout_evaluate_metrics(flow_layout_metrics *metrics)
{
	flow_layout_line_metrics line;
	horizontal_align last_halign = horizontal_align::left;
	vertical_align last_valign = vertical_align::top;
	flow_layout_frame *flf;
	uint64 end_idx = 0;
	vector<uint32, 2> size;
	if(metrics->fl->direction == flow_axis::x)
	{
		for(uint64 i = 0; i < metrics->fl->frames.size; i++)
		{
			flf = &metrics->fl->frames.addr[i];
			if(metrics->fl->fm.height_desc.type == ui_size_type::autosize
				&& flf->fm->height_desc.type == ui_size_type::relative)
			{
				if(flf->fm->width_desc.type == ui_size_type::autosize)
					size.x = flf->fm->max_width;
				else size.x = resolve_ui_size(flf->fm->width_desc, metrics->viewport_width);
				size.x = min(max(flf->fm->min_width, size.x), flf->fm->max_width);
				size.x -= resolve_ui_size(flf->fm->margin_left, size.x)
					+ resolve_ui_size(flf->fm->padding_left, size.x)
					+ resolve_ui_size(flf->fm->margin_right, size.x)
					+ resolve_ui_size(flf->fm->padding_right, size.x);
				size.y = flf->fm->max_height;
				size.y -= resolve_ui_size(flf->fm->margin_bottom, size.y)
					+ resolve_ui_size(flf->fm->padding_bottom, size.y)
					+ resolve_ui_size(flf->fm->margin_top, size.y)
					+ resolve_ui_size(flf->fm->padding_top, size.y);
				size = flf->fm->content_size(flf->fm, size.x, size.y);
				size = content_size_to_frame_size(flf->fm, size.x, size.y);
				if(flf->fm->width_desc.type != ui_size_type::autosize)
					size.x = resolve_ui_size(flf->fm->width_desc, metrics->viewport_width);
				size.x = min(max(flf->fm->min_width, size.x), flf->fm->max_width);
				size.y = min(max(flf->fm->min_height, size.y), flf->fm->max_height);
			}
			else size = evaluate_frame_size(flf->fm, metrics->viewport_width, metrics->viewport_height);
			if(metrics->fl->multiline
				&& i != end_idx
				&& (flf->line_break
					|| uint32(flf->halign) < uint32(last_halign)
					|| line.size1 + line.size2 + line.size3 + (int32)size.x > metrics->viewport_width))
			{
				line.frame_count = i - end_idx;
				metrics->content_width = max(metrics->content_width, line.size1 + line.size2 + line.size3);
				metrics->content_height += line.linespace;
				metrics->line_metrics.push(line);
				end_idx = i;
				line = flow_layout_line_metrics();
			}
			last_halign = flf->halign;
			if(flf->halign == horizontal_align::left) line.size1 += int32(size.x);
			else if(flf->halign == horizontal_align::center) line.size2 += int32(size.x);
			else line.size3 += int32(size.x);
			line.linespace = max(line.linespace, int32(size.y));
		}
		if(end_idx != metrics->fl->frames.size)
		{
			line.frame_count = metrics->fl->frames.size - end_idx;
			metrics->content_width = max(metrics->content_width, line.size1 + line.size2 + line.size3);
			metrics->content_height += line.linespace;
			metrics->line_metrics.push(line);
		}
	}
}

vector<uint32, 2> flow_layout_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	flow_layout_metrics metrics(fl, viewport_width, viewport_height);
	flow_layout_evaluate_metrics(&metrics);
	if(metrics.content_width > int32(viewport_width))
		metrics.content_height += int32(fl->xscroll.fm.height_desc.value);
	if(metrics.content_height > int32(viewport_height))
		metrics.content_width += int32(fl->yscroll.fm.width_desc.value);
	return vector<uint32, 2>(metrics.content_width, metrics.content_height);
}

void flow_layout_update_layout(frame *fm)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	rectangle<int32> viewport = frame_viewport(&fl->fm);
	rectangle<int32> content_viewport = frame_content_viewport(&fl->fm);
	flow_layout_metrics metrics(fl, content_viewport.extent.x, content_viewport.extent.y);
	flow_layout_evaluate_metrics(&metrics);
	bool reevaluate = false;
	fl->xscroll.fm.height = fl->xscroll.fm.height_desc.value.integer;
	fl->yscroll.fm.width = fl->yscroll.fm.width_desc.value.integer;
	if(metrics.content_width > metrics.viewport_width)
	{
		reevaluate = true;
		metrics.viewport_height -= fl->xscroll.fm.height;
		fl->xscroll.fm.visible = true;
	}
	else fl->xscroll.fm.visible = false;
	if(metrics.content_height > metrics.viewport_height)
	{
		reevaluate = true;
		metrics.viewport_width -= fl->yscroll.fm.width;
		fl->yscroll.fm.visible = true;
	}
	else fl->yscroll.fm.visible = false;
	if(reevaluate)
	{
		metrics.content_width = 0;
		metrics.content_height = 0;
		metrics.line_metrics.clear();
		flow_layout_evaluate_metrics(&metrics);
		fl->xscroll.content_size = uint32(metrics.content_width);
		fl->xscroll.viewport_size = uint32(metrics.viewport_width);
		fl->yscroll.content_size = uint32(metrics.content_height);
		fl->yscroll.viewport_size = uint32(metrics.viewport_height);
		fl->xscroll.fm.width = uint32(content_viewport.extent.x);
		fl->yscroll.fm.height = uint32(content_viewport.extent.y);
		if(fl->xscroll.fm.visible && fl->yscroll.fm.visible)
		{
			fl->xscroll.fm.width -= fl->yscroll.fm.width;
			fl->yscroll.fm.height -= fl->xscroll.fm.height;
		}
		fl->xscroll.fm.x = content_viewport.position.x;
		fl->xscroll.fm.y = content_viewport.position.y;
		fl->yscroll.fm.x = content_viewport.position.x
			+ content_viewport.extent.x - int32(fl->yscroll.fm.width);
		fl->yscroll.fm.y = content_viewport.position.y
			+ content_viewport.extent.y - int32(fl->yscroll.fm.height);
	}
	int32 offset1, offset2, offset3, fi = 0;
	vector<uint32, 2> size;
	vector<int32, 2> line_position;
	if(fl->direction == flow_axis::x)
	{
		if(fl->offset == flow_offset::right)
			line_position = vector<int32, 2>(
				content_viewport.position.x,
				content_viewport.position.y + content_viewport.extent.y);
		else line_position = vector<int32, 2>(
			content_viewport.position.x,
			content_viewport.position.y + content_viewport.extent.y - metrics.content_height);
		for(uint64 i = 0; i < metrics.line_metrics.size; i++)
		{
			if(fl->offset == flow_offset::right)
				line_position.y -= metrics.line_metrics.addr[i].linespace;
			offset1 = 0;
			offset2 = max(
				(metrics.viewport_width - metrics.line_metrics.addr[i].size2) / 2,
				metrics.line_metrics.addr[i].size1);
			if(metrics.line_metrics.addr[i].size2 == 0)
				offset2 = metrics.line_metrics.addr[i].size1;
			offset3 = max(
				metrics.viewport_width - metrics.line_metrics.addr[i].size3,
				offset2 + metrics.line_metrics.addr[i].size2);
			for(uint64 j = metrics.line_metrics.addr[i].frame_count; j != 0; j--, fi++)
			{
				if(fl->fm.height_desc.type == ui_size_type::autosize
					&& fl->frames.addr[fi].fm->height_desc.type == ui_size_type::relative)
					size = evaluate_frame_size(
						fl->frames.addr[fi].fm,
						metrics.viewport_width,
						metrics.line_metrics.addr[i].linespace);
				else size = evaluate_frame_size(
					fl->frames.addr[fi].fm,
					metrics.viewport_width,
					metrics.viewport_height);
				fl->frames.addr[fi].fm->width = size.x;
				fl->frames.addr[fi].fm->height = size.y;
				if(fl->frames.addr[fi].halign == horizontal_align::left)
				{
					fl->frames.addr[fi].fm->x = line_position.x + offset1;
					offset1 += int32(fl->frames.addr[fi].fm->width);
				}
				else if(fl->frames.addr[fi].halign == horizontal_align::center)
				{
					fl->frames.addr[fi].fm->x = line_position.x + offset2;
					offset2 += int32(fl->frames.addr[fi].fm->width);
				}
				else
				{
					fl->frames.addr[fi].fm->x = line_position.x + offset3;
					offset3 += int32(fl->frames.addr[fi].fm->width);
				}
				if(fl->frames.addr[fi].valign == vertical_align::top)
					fl->frames.addr[fi].fm->y = line_position.y
						+ metrics.line_metrics.addr[i].linespace
						- int32(fl->frames.addr[fi].fm->height);
				else if(fl->frames.addr[fi].valign == vertical_align::center)
					fl->frames.addr[fi].fm->y = line_position.y
						+ (metrics.line_metrics.addr[i].linespace - int32(fl->frames.addr[fi].fm->height)) / 2;
				else fl->frames.addr[fi].fm->y = line_position.y;
			}
			if(fl->offset == flow_offset::left)
				line_position.y += metrics.line_metrics.addr[i].linespace;
		}
	}
}

void flow_layout_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	rectangle<int32> content_viewport = frame_content_viewport(&fl->fm);
	content_viewport.position -= point;
	if(!fl->fm.visible
		|| content_viewport.extent.x <= 0
		|| content_viewport.extent.y <= 0)
		return;
	flow_layout_update_layout(&fl->fm);
	bp->push_scissor(content_viewport);
	if(fl->xscroll.content_size > fl->xscroll.viewport_size)
		fl->xscroll.viewport_offset = min(fl->xscroll.viewport_offset, fl->xscroll.content_size - fl->xscroll.viewport_size);
	else fl->xscroll.viewport_offset = 0;
	if(fl->yscroll.content_size > fl->yscroll.viewport_size)
		fl->yscroll.viewport_offset = min(fl->yscroll.viewport_offset, fl->yscroll.content_size - fl->yscroll.viewport_size);
	else fl->yscroll.viewport_offset = 0;
	for(uint64 i = 0; i < fl->frames.size; i++)
	{
		fl->frames.addr[i].fm->x -= int32(fl->xscroll.viewport_offset);
		fl->frames.addr[i].fm->y += int32(fl->yscroll.viewport_offset);
		if(fl->frames.addr[i].fm->x < point.x + content_viewport.position.x + content_viewport.extent.x
			&& fl->frames.addr[i].fm->x + int32(fl->frames.addr[i].fm->width)
			>= point.x + content_viewport.position.x
			&& fl->frames.addr[i].fm->y < point.y + content_viewport.position.y + content_viewport.extent.y
			&& fl->frames.addr[i].fm->y + int32(fl->frames.addr[i].fm->height)
			>= point.y + content_viewport.position.y)
			fl->frames.addr[i].fm->render(fl->frames.addr[i].fm, point, bp, bmp);
	}
	bp->pop_scissor();
	fl->xscroll.fm.render(&fl->xscroll.fm, point, bp, bmp);
	fl->yscroll.fm.render(&fl->yscroll.fm, point, bp, bmp);
}

void flow_layout_mouse_wheel_rotate(frame *fm)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	if(fl->yscroll.fm.visible)
		fl->yscroll.shift(50, mouse()->wheel_forward);
	else fl->xscroll.shift(50, mouse()->wheel_forward);
}
