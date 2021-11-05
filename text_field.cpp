#include "frame_templates.h"
#include "timer.h"
#include "os_api.h"

bool caret_visible;
timer_trigger_action caret_timer_callback(void *data)
{
	caret_visible = !caret_visible;
	os_update_windows();
	return timer_trigger_action::reactivate;
}
timer *caret_timer = nullptr;

text_field::text_field()
{
	fm.data = this;
	fm.padding_left = 4uiabs;
	fm.padding_bottom = 4uiabs;
	fm.padding_right = 4uiabs;
	fm.padding_top = 4uiabs;
	fm.subframes = text_field_subframes;
	fm.content_size = text_field_content_size;
	fm.render = text_field_render;
	fm.mouse_click = text_field_mouse_click;
	fm.mouse_move = text_field_mouse_move;
	fm.focus_receive = text_field_focus_receive;
	fm.focus_loss = text_field_focus_loss;
	fm.mouse_wheel_rotate = text_field_mouse_wheel_rotate;
	fm.key_press = text_field_key_press;
	fm.char_input = text_field_char_input;
	font << U"cambria";
	font_size = 20;
	caret = 0;
	select_caret = 0;
	selecting = false;
	tl.multiline = true;
	editable = false;
}

void text_field::select(uint64 idx_begin, uint64 idx_end)
{
	caret = idx_begin;
	select_caret = idx_end;
}

void text_field::deselect(uint64 return_idx)
{
	caret = return_idx;
	select_caret = return_idx;
}

void text_field::insert(string &text)
{
	remove();
	tl.insert_text(caret, text, font, font_size);
	caret += text.size;
	select_caret = caret;
}

void text_field::remove()
{
	if(select_caret < caret)
		swap(&caret, &select_caret);
	tl.glyphs.remove_range(caret, select_caret);
	select_caret = caret;
}

void text_field::scroll_to_caret()
{
	if(tl.glyphs.size == 0) return;
	rectangle<int32> content_viewport = frame_content_viewport(&fm);
	vector<int32, 2> point;
	int32 line_height;
	tl.hit_test_position(caret, &point, &line_height);
	if(tl.multiline)
	{
		point.y += content_viewport.extent.y + (int32)scroll.viewport_offset - int32(tl.height);
		if(point.y < 0)
			scroll.shift(uint32(-point.y), false);
		else if(point.y + line_height >= content_viewport.extent.y)
			scroll.shift(point.y + line_height - content_viewport.extent.y, true);
	}
	else
	{
		point.x -= int32(scroll.viewport_offset);
		if(point.x < 0)
			scroll.shift(uint32(-point.x), true);
		else if(point.x >= content_viewport.extent.x)
			scroll.shift(point.x - content_viewport.extent.x, false);
	}
}

void text_field_subframes(frame *fm, array<frame *> *frames)
{
	text_field *tf = (text_field *)fm->data;
	frames->push(&tf->scroll.fm);
}

vector<uint32, 2> text_field_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	text_field *tf = (text_field *)fm->data;
	uint32 tl_width = tf->tl.width;
	tf->tl.width = 1000000000;
	vector<int32, 2> size = tf->tl.content_size();
	tf->tl.width = tl_width;
	return vector<uint32, 2>(uint32(size.x), uint32(size.y));
}

struct text_field_render_glyph_args
{
	text_field *tf;
	bitmap_processor *bp;
	vector<int32, 2> base_point;
};

void text_field_render_glyph(
	glyph &gl,
	vector<int32, 2> point,
	int32 baseline,
	int32 line_height,
	uint64 idx,
	void *data,
	bitmap *bmp)
{
	text_field_render_glyph_args *args = (text_field_render_glyph_args *)data;
	rectangle<int32> content_viewport = frame_content_viewport(&args->tf->fm);
	content_viewport.position -= args->base_point;
	if(point.y - baseline + line_height < content_viewport.position.y
		|| point.y - baseline > content_viewport.position.y + content_viewport.extent.y)
		return;
	rectangle<real> rect;
	geometry_path rect_path;
	args->bp->rasterization = rasterization_mode::fill;
	uint64 begin = args->tf->caret, end = args->tf->select_caret;
	if(end < begin) swap(&begin, &end);
	if(begin <= idx && idx < end)
	{
		rect.position = vector<real, 2>(0.0r, 0.0r);
		rect.extent = vector<real, 2>(
			real(gl.data->advance.x),
			real(line_height));
		rect_path.push_rectangle(rect);
		args->bp->set_solid_color_brush(alpha_color(0, 0, 200, 255));
		args->bp->transform = translate_matrix(point.x, round(point.y - baseline));
		args->bp->render(rect_path, bmp);
		rect_path.data.clear();
	}
	if(begin <= idx && idx < end)
		args->bp->set_solid_color_brush(alpha_color(255, 255, 255, 255));
	else args->bp->set_solid_color_brush(alpha_color(0, 0, 0, 255));
	args->bp->fill_opacity_bitmap(
		gl.data->bmp,
		vector<int32, 2>(
			point.x + gl.data->bmp_offset.x.get_int32(),
			point.y + gl.data->bmp_offset.y.get_int32()),
		bmp);
	if(gl.underlined)
	{
		rect.position = vector<real, 2>(
			0.0r,
			real(gl.data->underline_offset));
		rect.extent = vector<real, 2>(
			real(gl.data->advance.x),
			real(gl.data->underline_size));
		rect_path.push_rectangle(rect);
		args->bp->transform = translate_matrix(point.x, point.y);
		args->bp->render(rect_path, bmp);
		rect_path.data.clear();
	}
	if(gl.strikedthrough)
	{
		rect.position = vector<real, 2>(
			0.0r,
			real(gl.data->strikethrough_offset));
		rect.extent = vector<real, 2>(
			real(gl.data->advance.x),
			real(gl.data->strikethrough_size));
		rect_path.push_rectangle(rect);
		args->bp->transform = translate_matrix(point.x, point.y);
		args->bp->render(rect_path, bmp);
		rect_path.data.clear();
	}
	if(caret_visible
		&& focused_frame() == &args->tf->fm
		&& args->tf->editable
		&& (idx == args->tf->caret
		|| idx == args->tf->caret - 1
			&& args->tf->caret == args->tf->tl.glyphs.size))
	{
		rect.position = vector<real, 2>(point.x, point.y - baseline + 0.1r * line_height);
		if(args->tf->caret == args->tf->tl.glyphs.size)
			rect.position.x += real(gl.data->advance.x);
		rect.position.x = rect.position.x;
		rect.extent = vector<real, 2>(2.0r, 0.8r * line_height);
		rect_path.push_rectangle(rect);
		set_identity_matrix(&args->bp->transform);
		args->bp->set_solid_color_brush(alpha_color(0, 0, 0, 255));
		args->bp->render(rect_path, bmp);
	}
}

void text_field_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	text_field *tf = (text_field *)fm->data;
	rectangle viewport = frame_viewport(&tf->fm),
		content_viewport = frame_content_viewport(&tf->fm);
	viewport.position -= point;
	content_viewport.position -= point;
	if(!tf->fm.visible
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	geometry_path rect_path;
	if(tf->editable)
	{
		rect_path.push_rectangle(rectangle<real>(viewport));
		set_identity_matrix(&bp->transform);
		bp->rasterization = rasterization_mode::fill;
		bp->brush = brush_type::solid;
		bp->color = alpha_color(255, 255, 255, 255);
		bp->render(rect_path, bmp);
		bp->rasterization = rasterization_mode::outline;
		bp->line_width = 1.0r;
		bp->color = alpha_color(0, 0, 0, 255);
		bp->render(rect_path, bmp);
		rect_path.data.clear();
	}
	tf->scroll.fm.width = tf->scroll.fm.width_desc.value.integer;
	tf->scroll.fm.height = (uint32)viewport.extent.y;
	tf->scroll.fm.x = point.x + viewport.position.x + viewport.extent.x - tf->scroll.fm.width;
	tf->scroll.fm.y = point.y + viewport.position.y;
	tf->tl.width = uint32(content_viewport.extent.x);
	if(tf->tl.multiline)
	{
		tf->scroll.fm.visible = true;
		tf->scroll.content_size = ceil(tf->tl.content_size().y).integer;
		tf->scroll.viewport_size = (uint32)content_viewport.extent.y;
		if(tf->scroll.content_size > tf->scroll.viewport_size)
		{
			content_viewport.extent.x -= tf->scroll.fm.width;
			tf->scroll.viewport_offset = min(tf->scroll.viewport_offset, tf->scroll.content_size - tf->scroll.viewport_size);
		}
		else
		{
			tf->scroll.fm.visible = false;
			tf->scroll.viewport_offset = 0;
		}
		if(content_viewport.extent.x <= 0) return;
	}
	else
	{
		tf->scroll.fm.visible = false;
		tf->scroll.content_size = ceil(tf->tl.content_size().x).integer;
		tf->scroll.viewport_size = (uint32)content_viewport.extent.x;
		if(tf->scroll.content_size > tf->scroll.viewport_size)
			tf->scroll.viewport_offset = min(tf->scroll.viewport_offset, tf->scroll.content_size - tf->scroll.viewport_size);
		else tf->scroll.viewport_offset = 0;
	}
	tf->tl.width = uint32(content_viewport.extent.x);
	tf->tl.height = uint32(content_viewport.extent.y);
	bp->push_scissor(viewport);
	text_field_render_glyph_args args;
	args.tf = tf;
	args.bp = bp;
	args.base_point = point;
	vector<int32, 2> text_point;
	if(tf->tl.multiline)
		text_point = vector<int32, 2>(
			content_viewport.position.x,
			content_viewport.position.y
				+ content_viewport.extent.y
				- (int32)tf->tl.height
				+ (int32)tf->scroll.viewport_offset);
	else text_point = vector<int32, 2>(
		content_viewport.position.x - (int32)tf->scroll.viewport_offset,
		content_viewport.position.y
			+ content_viewport.extent.y
			- (int32)tf->tl.height);
	tf->tl.render(
		text_point,
		text_field_render_glyph,
		&args,
		bmp);
	bp->pop_scissor();
	if(caret_visible
		&& focused_frame() == &tf->fm
		&& tf->editable
		&& tf->tl.glyphs.size == 0)
	{
		rectangle<real> rect;
		rect.position = vector<real, 2>(
			content_viewport.position.x,
			content_viewport.position.y + content_viewport.extent.y - 0.9r * tf->font_size);
		rect.extent = vector<real, 2>(2.0r, 0.8r * tf->font_size);
		rect_path.push_rectangle(rect);
		bp->rasterization = rasterization_mode::fill;
		set_identity_matrix(&bp->transform);
		bp->set_solid_color_brush(alpha_color(0, 0, 0, 255));
		bp->render(rect_path, bmp);
	}
	tf->scroll.fm.render(&tf->scroll.fm, point, bp, bmp);
}

void text_field_mouse_click(frame *fm)
{
	text_field *tf = (text_field *)fm->data;
	rectangle<int32> content_viewport = frame_content_viewport(&tf->fm);
	if(tf->tl.multiline)
		tf->tl.hit_test_point(
			vector<int32, 2>(
				mouse()->position.x - content_viewport.position.x,
				mouse()->position.y - (content_viewport.position.y + (int32)tf->scroll.viewport_offset)),
			&tf->caret);
	else tf->tl.hit_test_point(
			vector<int32, 2>(
				mouse()->position.x - (content_viewport.position.x - (int32)tf->scroll.viewport_offset),
				mouse()->position.y - content_viewport.position.y),
			&tf->caret);
	tf->select_caret = tf->caret;
	tf->selecting = true;
}

void text_field_mouse_move(frame *fm)
{
	text_field *tf = (text_field *)fm->data;
	rectangle<int32> content_viewport = frame_content_viewport(&tf->fm);
	if(tf->selecting)
	{
		if(!mouse()->left_pressed)
			tf->selecting = false;
		else
		{
			if(tf->tl.multiline)
				tf->tl.hit_test_point(
					vector<int32, 2>(
						mouse()->position.x - content_viewport.position.x,
						mouse()->position.y - (content_viewport.position.y + (int32)tf->scroll.viewport_offset)),
					&tf->caret);
			else tf->tl.hit_test_point(
				vector<int32, 2>(
					mouse()->position.x - (content_viewport.position.x - (int32)tf->scroll.viewport_offset),
					mouse()->position.y - content_viewport.position.y),
				&tf->caret);
		}
	}
}

void text_field_focus_receive(frame *fm)
{
	caret_visible = true;
	if(caret_timer == nullptr)
	{
		caret_timer = new timer();
		caret_timer->period = 0;
		caret_timer->period << milliseconds(500);
		caret_timer->callback = caret_timer_callback;
	}
	caret_timer->run();
}

void text_field_focus_loss(frame *fm)
{
	if(caret_timer != nullptr)
		caret_timer->reset();
}

void text_field_mouse_wheel_rotate(frame *fm)
{
	text_field *tf = (text_field *)fm->data;
	tf->scroll.shift(50, mouse()->wheel_forward);
}

void text_field_key_press(frame *fm)
{
	text_field *tf = (text_field *)fm->data;
	if(keyboard()->pressed_count == 1)
	{
		if(keyboard()->key_pressed[(uint8)key_code::left] && tf->caret != 0)
		{
			tf->caret--;
			tf->select_caret = tf->caret;
			tf->scroll_to_caret();
		}
		else if(keyboard()->key_pressed[(uint8)key_code::right] && tf->caret != tf->tl.glyphs.size)
		{
			tf->caret++;
			tf->select_caret = tf->caret;
			tf->scroll_to_caret();
		}
		else if(keyboard()->key_pressed[(uint8)key_code::down] && tf->tl.multiline && tf->tl.glyphs.size != 0)
		{
			vector<int32, 2> point;
			int32 line_height;
			tf->tl.hit_test_position(tf->caret, &point, &line_height);
			point.y--;
			tf->tl.hit_test_point(point, &tf->caret);
			tf->select_caret = tf->caret;
			tf->scroll_to_caret();
		}
		else if(keyboard()->key_pressed[(uint8)key_code::up] && tf->tl.multiline && tf->tl.glyphs.size != 0)
		{
			vector<int32, 2> point;
			int32 line_height;
			tf->tl.hit_test_position(tf->caret, &point, &line_height);
			point.y += line_height + 1;
			tf->tl.hit_test_point(point, &tf->caret);
			tf->select_caret = tf->caret;
			tf->scroll_to_caret();
		}
	}
	else if(keyboard()->pressed_count == 2 && keyboard()->key_pressed[(uint8)key_code::ctrl])
	{
		if(keyboard()->key_pressed[(uint8)key_code::c] && tf->caret != tf->select_caret)
		{
			uint64 begin = tf->caret, end = tf->select_caret;
			if(end < begin) swap(&begin, &end);
			string str;
			while(begin < end)
			{
				str.push(tf->tl.glyphs.addr[begin].code);
				begin++;
			}
			os_copy_text_to_clipboard(str);
		}
		else if(keyboard()->key_pressed[(uint8)key_code::v] && tf->editable)
		{
			string str;
			os_copy_text_from_clipboard(&str);
			tf->insert(str);
			tf->scroll_to_caret();
		}
		else if(keyboard()->key_pressed[(uint8)key_code::x] && tf->editable)
		{
			uint64 begin = tf->caret, end = tf->select_caret;
			if(end < begin) swap(&begin, &end);
			string str;
			while(begin < end)
			{
				str.push(tf->tl.glyphs.addr[begin].code);
				begin++;
			}
			os_copy_text_to_clipboard(str);
			tf->remove();
			tf->scroll_to_caret();
		}
		else if(keyboard()->key_pressed[(uint8)key_code::a])
		{
			tf->select(tf->tl.glyphs.size, 0);
			tf->scroll_to_caret();
		}
	}
}

void text_field_char_input(frame *fm)
{
	text_field *tf = (text_field *)fm->data;
	if(!tf->editable
		|| keyboard()->key_pressed[(uint8)key_code::ctrl]
		|| keyboard()->key_pressed[(uint8)key_code::alt])
		return;
	if(keyboard()->char_code == (char32)key_code::backspace)
	{
		if(tf->caret == tf->select_caret
			&& tf->caret != 0
			&& tf->tl.glyphs.size != 0)
			tf->select(tf->caret - 1, tf->caret);
		tf->remove();
		tf->scroll_to_caret();
	}
	else if(keyboard()->char_code == (char32)key_code::enter
		&& !tf->tl.multiline
		&& focused_frame() == &tf->fm)
		focus_frame(nullptr);
	else if(keyboard()->char_code == (char32)key_code::enter
		|| keyboard()->char_code == (char32)key_code::tab
		|| keyboard()->char_code >= U' ')
	{
		string str;
		if(keyboard()->char_code == (char32)key_code::enter)
			str.push(U'\n');
		else str.push(keyboard()->char_code);
		tf->insert(str);
		tf->scroll_to_caret();
	}
}
