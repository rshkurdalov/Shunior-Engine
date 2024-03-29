#include "frame_templates.h"

void flow_layout_data::attach(frame *fm)
{
	direction = flow_axis::x;
	offset = flow_offset::right;
	multiline = true;
	xscroll.data.vertical = false;
}

void flow_layout_data::subframes(frame *fm, array<frame *> *frames_addr)
{
	frames_addr->push(&xscroll.fm);
	frames_addr->push(&yscroll.fm);
	for(uint64 i = 0; i < frames.size; i++)
		frames_addr->push(frames.addr[i].fm);
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
	frame *fm;
	flow_layout_data *fl;
	int32 viewport_width;
	int32 viewport_height;
	int32 content_width;
	int32 content_height;
	array<flow_layout_line_metrics> line_metrics;

	flow_layout_metrics(frame *fm, flow_layout_data *fl, int32 viewport_width, int32 viewport_height)
		: fm(fm), fl(fl), viewport_width(viewport_width), viewport_height(viewport_height)
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
			if(metrics->fm->height_desc.type == ui_size_type::autosize
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
	else
	{
		for(uint64 i = 0; i < metrics->fl->frames.size; i++)
		{
			flf = &metrics->fl->frames.addr[i];
			if(metrics->fm->width_desc.type == ui_size_type::autosize
				&& flf->fm->width_desc.type == ui_size_type::relative)
			{
				if(flf->fm->height_desc.type == ui_size_type::autosize)
					size.y = flf->fm->max_height;
				else size.y = resolve_ui_size(flf->fm->height_desc, metrics->viewport_height);
				size.y = min(max(flf->fm->min_height, size.y), flf->fm->max_height);
				size.y -= resolve_ui_size(flf->fm->margin_bottom, size.y)
					+ resolve_ui_size(flf->fm->padding_bottom, size.y)
					+ resolve_ui_size(flf->fm->margin_top, size.y)
					+ resolve_ui_size(flf->fm->padding_top, size.y);
				size.x = flf->fm->max_width;
				size.x -= resolve_ui_size(flf->fm->margin_left, size.x)
					+ resolve_ui_size(flf->fm->padding_left, size.x)
					+ resolve_ui_size(flf->fm->margin_right, size.x)
					+ resolve_ui_size(flf->fm->padding_right, size.x);
				size = flf->fm->content_size(flf->fm, size.x, size.y);
				size = content_size_to_frame_size(flf->fm, size.x, size.y);
				if(flf->fm->height_desc.type != ui_size_type::autosize)
					size.y = resolve_ui_size(flf->fm->height_desc, metrics->viewport_height);
				size.x = min(max(flf->fm->min_width, size.x), flf->fm->max_width);
				size.y = min(max(flf->fm->min_height, size.y), flf->fm->max_height);
			}
			else size = evaluate_frame_size(flf->fm, metrics->viewport_width, metrics->viewport_height);
			if(metrics->fl->multiline
				&& i != end_idx
				&& (flf->line_break
					|| uint32(flf->valign) < uint32(last_valign)
					|| line.size1 + line.size2 + line.size3 + (int32)size.y > metrics->viewport_height))
			{
				line.frame_count = i - end_idx;
				metrics->content_height = max(metrics->content_height, line.size1 + line.size2 + line.size3);
				metrics->content_width += line.linespace;
				metrics->line_metrics.push(line);
				end_idx = i;
				line = flow_layout_line_metrics();
			}
			last_valign = flf->valign;
			if(flf->valign == vertical_align::bottom) line.size1 += int32(size.y);
			else if(flf->valign == vertical_align::center) line.size2 += int32(size.y);
			else line.size3 += int32(size.y);
			line.linespace = max(line.linespace, int32(size.x));
		}
		if(end_idx != metrics->fl->frames.size)
		{
			line.frame_count = metrics->fl->frames.size - end_idx;
			metrics->content_height = max(metrics->content_height, line.size1 + line.size2 + line.size3);
			metrics->content_width += line.linespace;
			metrics->line_metrics.push(line);
		}
	}
}

vector<uint32, 2> flow_layout_data::content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	flow_layout_metrics metrics(fm, this, viewport_width, viewport_height);
	flow_layout_evaluate_metrics(&metrics);
	if(metrics.content_width > int32(viewport_width))
		metrics.content_height += int32(xscroll.fm.height_desc.value);
	if(metrics.content_height > int32(viewport_height))
		metrics.content_width += int32(yscroll.fm.width_desc.value);
	return vector<uint32, 2>(metrics.content_width, metrics.content_height);
}

void flow_layout_data::update_layout(frame *fm)
{
	rectangle<int32> viewport = frame_viewport(fm);
	rectangle<int32> content_viewport = frame_content_viewport(fm);
	flow_layout_metrics metrics(fm, this, content_viewport.extent.x, content_viewport.extent.y);
	flow_layout_evaluate_metrics(&metrics);
	bool reevaluate = false;
	xscroll.fm.height = xscroll.fm.height_desc.value.integer;
	yscroll.fm.width = yscroll.fm.width_desc.value.integer;
	if(metrics.content_width > metrics.viewport_width)
	{
		reevaluate = true;
		metrics.viewport_height -= xscroll.fm.height;
		xscroll.fm.visible = true;
	}
	else xscroll.fm.visible = false;
	if(metrics.content_height > metrics.viewport_height)
	{
		reevaluate = true;
		metrics.viewport_width -= yscroll.fm.width;
		yscroll.fm.visible = true;
	}
	else yscroll.fm.visible = false;
	if(reevaluate)
	{
		metrics.content_width = 0;
		metrics.content_height = 0;
		metrics.line_metrics.clear();
		flow_layout_evaluate_metrics(&metrics);
		xscroll.data.content_size = uint32(metrics.content_width);
		xscroll.data.viewport_size = uint32(metrics.viewport_width);
		yscroll.data.content_size = uint32(metrics.content_height);
		yscroll.data.viewport_size = uint32(metrics.viewport_height);
		xscroll.fm.width = uint32(content_viewport.extent.x);
		yscroll.fm.height = uint32(content_viewport.extent.y);
		if(xscroll.fm.visible && yscroll.fm.visible)
		{
			xscroll.fm.width -= yscroll.fm.width;
			yscroll.fm.height -= xscroll.fm.height;
		}
		xscroll.fm.x = content_viewport.position.x;
		xscroll.fm.y = content_viewport.position.y;
		yscroll.fm.x = content_viewport.position.x
			+ content_viewport.extent.x - int32(yscroll.fm.width);
		yscroll.fm.y = content_viewport.position.y
			+ content_viewport.extent.y - int32(yscroll.fm.height);
	}
	int32 offset1, offset2, offset3, fi = 0;
	vector<uint32, 2> size;
	vector<int32, 2> line_position;
	if(direction == flow_axis::x)
	{
		if(offset == flow_offset::right)
			line_position = vector<int32, 2>(
				content_viewport.position.x,
				content_viewport.position.y + content_viewport.extent.y);
		else line_position = vector<int32, 2>(
			content_viewport.position.x,
			content_viewport.position.y + content_viewport.extent.y - metrics.content_height);
		for(uint64 i = 0; i < metrics.line_metrics.size; i++)
		{
			if(offset == flow_offset::right)
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
			if(metrics.content_width <= metrics.viewport_width
				&& offset3 + metrics.line_metrics.addr[i].size3 > metrics.viewport_width)
			{
				offset3 = metrics.viewport_width - metrics.line_metrics.addr[i].size3;
				offset2 = offset3 - metrics.line_metrics.addr[i].size2;
			}
			for(uint64 j = metrics.line_metrics.addr[i].frame_count; j != 0; j--, fi++)
			{
				if(fm->height_desc.type == ui_size_type::autosize
					&& frames.addr[fi].fm->height_desc.type == ui_size_type::relative)
					size = evaluate_frame_size(
						frames.addr[fi].fm,
						metrics.viewport_width,
						metrics.line_metrics.addr[i].linespace);
				else size = evaluate_frame_size(
					frames.addr[fi].fm,
					metrics.viewport_width,
					metrics.viewport_height);
				frames.addr[fi].fm->width = size.x;
				frames.addr[fi].fm->height = size.y;
				if(frames.addr[fi].halign == horizontal_align::left)
				{
					frames.addr[fi].fm->x = line_position.x + offset1;
					offset1 += int32(frames.addr[fi].fm->width);
				}
				else if(frames.addr[fi].halign == horizontal_align::center)
				{
					frames.addr[fi].fm->x = line_position.x + offset2;
					offset2 += int32(frames.addr[fi].fm->width);
				}
				else
				{
					frames.addr[fi].fm->x = line_position.x + offset3;
					offset3 += int32(frames.addr[fi].fm->width);
				}
				if(frames.addr[fi].valign == vertical_align::top)
					frames.addr[fi].fm->y = line_position.y
						+ metrics.line_metrics.addr[i].linespace
						- int32(frames.addr[fi].fm->height);
				else if(frames.addr[fi].valign == vertical_align::center)
					frames.addr[fi].fm->y = line_position.y
						+ (metrics.line_metrics.addr[i].linespace - int32(frames.addr[fi].fm->height)) / 2;
				else frames.addr[fi].fm->y = line_position.y;
			}
			if(offset == flow_offset::left)
				line_position.y += metrics.line_metrics.addr[i].linespace;
		}
	}
	else
	{
		if(offset == flow_offset::right)
			line_position = vector<int32, 2>(
				content_viewport.position.x,
				content_viewport.position.y);
		else line_position = vector<int32, 2>(
			content_viewport.position.x + content_viewport.extent.x,
			content_viewport.position.y);
		for(uint64 i = 0; i < metrics.line_metrics.size; i++)
		{
			if(offset == flow_offset::left)
				line_position.x -= metrics.line_metrics.addr[i].linespace;
			offset1 = 0;
			offset2 = max(
				(metrics.viewport_height - metrics.line_metrics.addr[i].size2) / 2,
				metrics.line_metrics.addr[i].size1);
			if(metrics.line_metrics.addr[i].size2 == 0)
				offset2 = metrics.line_metrics.addr[i].size1;
			offset3 = max(
				metrics.viewport_height - metrics.line_metrics.addr[i].size3,
				offset2 + metrics.line_metrics.addr[i].size2);
			if(metrics.content_height <= metrics.viewport_height
				&& offset3 + metrics.line_metrics.addr[i].size3 > metrics.viewport_height)
			{
				offset3 = metrics.viewport_height - metrics.line_metrics.addr[i].size3;
				offset2 = offset3 - metrics.line_metrics.addr[i].size2;
			}
			for(uint64 j = metrics.line_metrics.addr[i].frame_count; j != 0; j--, fi++)
			{
				if(fm->width_desc.type == ui_size_type::autosize
					&& frames.addr[fi].fm->width_desc.type == ui_size_type::relative)
					size = evaluate_frame_size(
						frames.addr[fi].fm,
						metrics.line_metrics.addr[i].linespace,
						metrics.viewport_height);
				else size = evaluate_frame_size(
					frames.addr[fi].fm,
					metrics.viewport_width,
					metrics.viewport_height);
				frames.addr[fi].fm->width = size.x;
				frames.addr[fi].fm->height = size.y;
				if(frames.addr[fi].valign == vertical_align::bottom)
				{
					frames.addr[fi].fm->y = line_position.y + offset1;
					offset1 += int32(frames.addr[fi].fm->height);
				}
				else if(frames.addr[fi].valign == vertical_align::center)
				{
					frames.addr[fi].fm->y = line_position.y + offset2;
					offset2 += int32(frames.addr[fi].fm->height);
				}
				else
				{
					frames.addr[fi].fm->y = line_position.y + offset3;
					offset3 += int32(frames.addr[fi].fm->height);
				}
				if(frames.addr[fi].halign == horizontal_align::right)
					frames.addr[fi].fm->x = line_position.x
						+ metrics.line_metrics.addr[i].linespace
						- int32(frames.addr[fi].fm->width);
				else if(frames.addr[fi].halign == horizontal_align::center)
					frames.addr[fi].fm->x = line_position.x
						+ (metrics.line_metrics.addr[i].linespace - int32(frames.addr[fi].fm->width)) / 2;
				else frames.addr[fi].fm->x = line_position.x;
			}
			if(offset == flow_offset::right)
				line_position.x += metrics.line_metrics.addr[i].linespace;
		}
	}
}

void flow_layout_data::render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle<int32> content_viewport = frame_content_viewport(fm);
	content_viewport.position -= point;
	if(!fm->visible
		|| content_viewport.extent.x <= 0
		|| content_viewport.extent.y <= 0)
		return;
	update_layout(fm);
	bp->push_scissor(content_viewport);
	if(xscroll.data.content_size > xscroll.data.viewport_size)
		xscroll.data.viewport_offset
			= min(xscroll.data.viewport_offset, xscroll.data.content_size - xscroll.data.viewport_size);
	else xscroll.data.viewport_offset = 0;
	if(yscroll.data.content_size > yscroll.data.viewport_size)
		yscroll.data.viewport_offset
			= min(yscroll.data.viewport_offset, yscroll.data.content_size - yscroll.data.viewport_size);
	else yscroll.data.viewport_offset = 0;
	for(uint64 i = 0; i < frames.size; i++)
	{
		frames.addr[i].fm->x -= int32(xscroll.data.viewport_offset);
		frames.addr[i].fm->y += int32(yscroll.data.viewport_offset);
		if(frames.addr[i].fm->x < point.x + content_viewport.position.x + content_viewport.extent.x
			&& frames.addr[i].fm->x + int32(frames.addr[i].fm->width)
				>= point.x + content_viewport.position.x
			&& frames.addr[i].fm->y < point.y + content_viewport.position.y + content_viewport.extent.y
			&& frames.addr[i].fm->y + int32(frames.addr[i].fm->height)
				>= point.y + content_viewport.position.y)
			frames.addr[i].fm->render(frames.addr[i].fm, point, bp, bmp);
	}
	bp->pop_scissor();
	xscroll.fm.render(&xscroll.fm, point, bp, bmp);
	yscroll.fm.render(&yscroll.fm, point, bp, bmp);
}

void flow_layout_data::mouse_wheel_rotate(frame *fm)
{
	if(yscroll.fm.visible)
		yscroll.data.shift(50, mouse()->wheel_forward);
	else xscroll.data.shift(50, mouse()->wheel_forward);
}

void flow_layout_subframes(frame *fm, array<frame *> *frames)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	fl->data.subframes(fm, frames);
}

vector<uint32, 2> flow_layout_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	return fl->data.content_size(fm, viewport_width, viewport_height);
}

void flow_layout_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	fl->model.render(fm, point, bp, bmp);
	fl->data.render(fm, point, bp, bmp);
}

void flow_layout_mouse_wheel_rotate(frame *fm)
{
	flow_layout *fl = (flow_layout *)(fm->data);
	fl->data.mouse_wheel_rotate(fm);
}

flow_layout::flow_layout()
{
	fm.data = this;
	fm.subframes = flow_layout_subframes;
	fm.content_size = flow_layout_content_size;
	fm.render = flow_layout_render;
	fm.mouse_wheel_rotate = flow_layout_mouse_wheel_rotate;
	data.attach(&fm);
	model.attach(&fm);
}
