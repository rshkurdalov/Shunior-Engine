#include "frame_templates.h"

void grid_layout_data::attach(frame *fm)
{
	growth_row = uint64(-1);
	growth_column = uint64(-1);
	xscroll.data.vertical = false;
}

void grid_layout_data::insert_row(uint32 insert_idx, ui_size size)
{
	rows_size.insert(insert_idx, size);
	frames.insert_rows(insert_idx, 1);
	for(uint64 j = 0; j < frames.columns; j++)
		frames.at(insert_idx, j).fm = nullptr;
}

void grid_layout_data::remove_row(uint32 remove_idx)
{
	rows_size.remove(remove_idx);
	frames.remove_rows(remove_idx, 1);
}

void grid_layout_data::insert_column(uint32 insert_idx, ui_size size)
{
	columns_size.insert(insert_idx, size);
	frames.insert_columns(insert_idx, 1);
	for(uint64 i = 0; i < frames.rows; i++)
		frames.at(i, insert_idx).fm = nullptr;
}

void grid_layout_data::remove_column(uint32 remove_idx)
{
	columns_size.remove(remove_idx);
	frames.remove_columns(remove_idx, 1);
}

void grid_layout_data::subframes(frame *fm, array<frame *> *frames_addr)
{
	frames_addr->push(&xscroll.fm);
	frames_addr->push(&yscroll.fm);
	for(uint64 i = 0; i < frames.rows * frames.columns; i++)
		if(frames.addr[i].fm != nullptr)
			frames_addr->push(frames.addr[i].fm);
}

struct grid_layout_metrics
{
	frame *fm;
	grid_layout_data *gl;
	int32 viewport_width;
	int32 viewport_height;
	int32 content_width;
	int32 content_height;
	array<uint32> rows_size;
	array<uint32> columns_size;

	grid_layout_metrics(frame *fm, grid_layout_data *gl, int32 viewport_width, int32 viewport_height)
		: fm(fm), gl(gl), viewport_width(viewport_width), viewport_height(viewport_height)
	{
		content_width = 0;
		content_height = 0;
		rows_size.insert_default(0, gl->rows_size.size);
		for(uint64 i = 0; i < rows_size.size; i++)
			rows_size.addr[i] = 0;
		columns_size.insert_default(0, gl->columns_size.size);
		for(uint64 i = 0; i < columns_size.size; i++)
			columns_size.addr[i] = 0;
	}
};

void grid_layout_evaluate_metrics(grid_layout_metrics *metrics)
{
	grid_layout_frame *glf;
	vector<uint32, 2> size;
	for(uint64 i = 0; i < metrics->rows_size.size; i++)
		if(metrics->gl->rows_size.addr[i].type != ui_size_type::autosize)
			metrics->rows_size.addr[i] = resolve_ui_size(metrics->gl->rows_size.addr[i], metrics->viewport_height);
	for(uint64 j = 0; j < metrics->columns_size.size; j++)
		if(metrics->gl->columns_size.addr[j].type != ui_size_type::autosize)
			metrics->columns_size.addr[j] = resolve_ui_size(metrics->gl->columns_size.addr[j], metrics->viewport_width);
	for(uint64 i = 0; i < metrics->gl->frames.rows; i++)
	{
		if(i == metrics->gl->growth_row) continue;
		for(uint64 j = 0; j < metrics->gl->frames.columns; j++)
		{
			if(j == metrics->gl->growth_column) continue;
			glf = &metrics->gl->frames.at(i, j);
			if(glf->fm == nullptr) continue;
			if(metrics->gl->rows_size.addr[i].type == ui_size_type::autosize
				&& metrics->gl->columns_size.addr[j].type == ui_size_type::autosize)
			{
				if(glf->fm->width_desc.type != ui_size_type::relative)
				{
					size.x = uint32(glf->fm->width_desc.value);
					size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				}
				else size.x = glf->fm->max_width;
				size.x -= resolve_ui_size(glf->fm->margin_left, size.x)
					+ resolve_ui_size(glf->fm->padding_left, size.x)
					+ resolve_ui_size(glf->fm->margin_right, size.x)
					+ resolve_ui_size(glf->fm->padding_right, size.x);
				if(glf->fm->height_desc.type != ui_size_type::relative)
				{
					size.y = uint32(glf->fm->height_desc.value);
					size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				}
				else size.y = glf->fm->max_height;
				size.y -= resolve_ui_size(glf->fm->margin_bottom, size.x)
					+ resolve_ui_size(glf->fm->padding_bottom, size.x)
					+ resolve_ui_size(glf->fm->margin_top, size.x)
					+ resolve_ui_size(glf->fm->padding_top, size.x);
				size = glf->fm->content_size(glf->fm, size.x, size.y);
				size = content_size_to_frame_size(glf->fm, size.x, size.y);
				if(glf->fm->width_desc.type == ui_size_type::absolute)
					size.x = uint32(glf->fm->width_desc.value);
				if(glf->fm->height_desc.type == ui_size_type::absolute)
					size.y = uint32(glf->fm->height_desc.value);
				size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				metrics->rows_size.addr[i] = max(metrics->rows_size.addr[i], size.y);
				metrics->columns_size.addr[j] = max(metrics->columns_size.addr[j], size.x);
			}
			else if(metrics->gl->rows_size.addr[i].type == ui_size_type::autosize)
			{
				if(glf->fm->width_desc.type == ui_size_type::autosize)
					size.x = glf->fm->max_width;
				else size.x = resolve_ui_size(glf->fm->width_desc, metrics->columns_size.addr[j]);
				size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				size.x -= resolve_ui_size(glf->fm->margin_left, size.x)
					+ resolve_ui_size(glf->fm->padding_left, size.x)
					+ resolve_ui_size(glf->fm->margin_right, size.x)
					+ resolve_ui_size(glf->fm->padding_right, size.x);
				if(glf->fm->height_desc.type == ui_size_type::relative)
					size.y = glf->fm->max_height;
				else
				{
					size.y = uint32(glf->fm->height_desc.value);
					size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				}
				size.y -= resolve_ui_size(glf->fm->margin_bottom, size.y)
					+ resolve_ui_size(glf->fm->padding_bottom, size.y)
					+ resolve_ui_size(glf->fm->margin_top, size.y)
					+ resolve_ui_size(glf->fm->padding_top, size.y);
				size = glf->fm->content_size(glf->fm, size.x, size.y);
				size = content_size_to_frame_size(glf->fm, size.x, size.y);
				if(glf->fm->width_desc.type != ui_size_type::autosize)
					size.x = resolve_ui_size(glf->fm->width_desc, metrics->columns_size.addr[j]);
				if(glf->fm->height_desc.type == ui_size_type::absolute)
					size.y = uint32(glf->fm->height_desc.value);
				size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				metrics->rows_size.addr[i] = max(metrics->rows_size.addr[i], size.y);
			}
			else if(metrics->gl->columns_size.addr[j].type == ui_size_type::autosize)
			{
				if(glf->fm->height_desc.type == ui_size_type::autosize)
					size.y = glf->fm->max_height;
				else size.y = resolve_ui_size(glf->fm->height_desc, metrics->rows_size.addr[i]);
				size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				size.y -= resolve_ui_size(glf->fm->margin_bottom, size.y)
					+ resolve_ui_size(glf->fm->padding_bottom, size.y)
					+ resolve_ui_size(glf->fm->margin_top, size.y)
					+ resolve_ui_size(glf->fm->padding_top, size.y);
				if(glf->fm->width_desc.type == ui_size_type::relative)
					size.x = glf->fm->max_width;
				else
				{
					size.x = uint32(glf->fm->width_desc.value);
					size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				}
				size.x -= resolve_ui_size(glf->fm->margin_left, size.x)
					+ resolve_ui_size(glf->fm->padding_left, size.x)
					+ resolve_ui_size(glf->fm->margin_right, size.x)
					+ resolve_ui_size(glf->fm->padding_right, size.x);
				size = glf->fm->content_size(glf->fm, size.x, size.y);
				size = content_size_to_frame_size(glf->fm, size.x, size.y);
				if(glf->fm->height_desc.type != ui_size_type::autosize)
					size.y = resolve_ui_size(glf->fm->height_desc, metrics->rows_size.addr[i]);
				if(glf->fm->width_desc.type == ui_size_type::absolute)
					size.x = uint32(glf->fm->width_desc.value);
				size.x = min(max(glf->fm->min_width, size.x), glf->fm->max_width);
				size.y = min(max(glf->fm->min_height, size.y), glf->fm->max_height);
				metrics->columns_size.addr[j] = max(metrics->columns_size.addr[j], size.x);
			}
		}
	}
	for(uint64 i = 0; i < metrics->rows_size.size; i++)
		if(i != metrics->gl->growth_row)
			metrics->content_height += metrics->rows_size.addr[i];
	for(uint64 j = 0; j < metrics->columns_size.size; j++)
		if(j != metrics->gl->growth_column)
			metrics->content_width += metrics->columns_size.addr[j];
	if(metrics->gl->growth_row != uint64(-1))
	{
		if(metrics->content_height >= metrics->viewport_height)
			metrics->rows_size.addr[metrics->gl->growth_row] = 0;
		else
		{
			metrics->rows_size.addr[metrics->gl->growth_row] = uint32(metrics->viewport_height - metrics->content_height);
			metrics->content_height = uint32(metrics->viewport_height);
		}
	}
	if(metrics->gl->growth_column != uint64(-1))
	{
		if(metrics->content_width >= metrics->viewport_width)
			metrics->columns_size.addr[metrics->gl->growth_column] = 0;
		else
		{
			metrics->columns_size.addr[metrics->gl->growth_column] = uint32(metrics->viewport_width - metrics->content_width);
			metrics->content_width = uint32(metrics->viewport_width);
		}
	}
}

vector<uint32, 2> grid_layout_data::content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	grid_layout_metrics metrics(fm, this, viewport_width, viewport_height);
	grid_layout_evaluate_metrics(&metrics);
	if(metrics.content_width > int32(viewport_width))
		metrics.content_height += int32(xscroll.fm.height_desc.value);
	if(metrics.content_height > int32(viewport_height))
		metrics.content_width += int32(yscroll.fm.width_desc.value);
	return vector<uint32, 2>(metrics.content_width, metrics.content_height);
}

void grid_layout_data::update_layout(frame *fm)
{
	rectangle<int32> viewport = frame_viewport(fm);
	rectangle<int32> content_viewport = frame_content_viewport(fm);
	grid_layout_metrics metrics(fm, this, content_viewport.extent.x, content_viewport.extent.y);
	grid_layout_evaluate_metrics(&metrics);
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
		for(uint64 i = 0; i < metrics.rows_size.size; i++)
			metrics.rows_size.addr[i] = 0;
		for(uint64 i = 0; i < metrics.columns_size.size; i++)
			metrics.columns_size.addr[i] = 0;
		grid_layout_evaluate_metrics(&metrics);
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
	grid_layout_frame *glf;
	vector<int32, 2> position = content_viewport.position;
	for(uint64 i = rows_size.size - 1; i != uint64(-1); i--)
	{
		for(uint64 j = 0; j < columns_size.size; j++)
		{
			glf = &frames.at(i, j);
			if(glf->fm != nullptr)
			{
				glf->fm->width = resolve_ui_size(glf->fm->width_desc, metrics.columns_size.addr[j]);
				glf->fm->height = resolve_ui_size(glf->fm->height_desc, metrics.rows_size.addr[i]);
				if(glf->halign == horizontal_align::left)
					glf->fm->x = position.x;
				else if(glf->halign == horizontal_align::center)
					glf->fm->x = position.x + (metrics.columns_size.addr[j] - glf->fm->width) / 2;
				else glf->fm->x = position.x + metrics.columns_size.addr[j] - glf->fm->width;
				if(glf->valign == vertical_align::bottom)
					glf->fm->y = position.y;
				else if(glf->valign == vertical_align::center)
					glf->fm->y = position.y + (metrics.rows_size.addr[i] - glf->fm->height) / 2;
				else glf->fm->y = position.y + metrics.rows_size.addr[i] - glf->fm->height;
			}
			position.x += int32(metrics.columns_size.addr[j]);
		}
		position.x = content_viewport.position.x;
		position.y += int32(metrics.rows_size.addr[i]);
	}
}

void grid_layout_data::render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
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
	for(uint64 i = 0; i < frames.rows * frames.columns; i++)
	{
		if(frames.addr[i].fm == nullptr) continue;
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

void grid_layout_data::mouse_wheel_rotate(frame *fm)
{
	if(yscroll.fm.visible)
		yscroll.data.shift(50, mouse()->wheel_forward);
	else xscroll.data.shift(50, mouse()->wheel_forward);
}

void grid_layout_subframes(frame *fm, array<frame *> *frames)
{
	grid_layout *gl = (grid_layout *)(fm->data);
	gl->data.subframes(fm, frames);
}

vector<uint32, 2> grid_layout_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	grid_layout *gl = (grid_layout *)(fm->data);
	return gl->data.content_size(fm, viewport_width, viewport_height);
}

void grid_layout_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	grid_layout *gl = (grid_layout *)(fm->data);
	gl->model.render(fm, point, bp, bmp);
	gl->data.render(fm, point, bp, bmp);
}

void grid_layout_mouse_wheel_rotate(frame *fm)
{
	grid_layout *gl = (grid_layout *)(fm->data);
	gl->data.mouse_wheel_rotate(fm);
}

grid_layout::grid_layout()
{
	fm.data = this;
	fm.subframes = grid_layout_subframes;
	fm.content_size = grid_layout_content_size;
	fm.render = grid_layout_render;
	fm.mouse_wheel_rotate = grid_layout_mouse_wheel_rotate;
	data.attach(&fm);
	model.attach(&fm);
}
