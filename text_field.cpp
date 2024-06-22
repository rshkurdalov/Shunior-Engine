#include "frame_templates.h"
#include "timer.h"
#include "os_api.h"

bool caret_visible;
timer_trigger_postaction caret_timer_callback(void *data)
{
	caret_visible = !caret_visible;
	os_update_windows();
	return timer_trigger_postaction::reactivate;
}
timer *caret_timer = nullptr;

void text_field_data::attach(handleable<frame> fm)
{
	font << U"cambria";
	font_size = 20;
	caret = 0;
	select_caret = 0;
	selecting = false;
	selection_visible = true;
	tl.multiline = true;
	editable = false;
}

void text_field_data::select(uint64 idx_begin, uint64 idx_end)
{
	caret = idx_begin;
	select_caret = idx_end;
}

void text_field_data::deselect(uint64 return_idx)
{
	caret = return_idx;
	select_caret = return_idx;
}

void text_field_data::insert(string &text)
{
	remove();
	tl.insert_text(caret, text, font, font_size);
	caret += text.size;
	select_caret = caret;
}

void text_field_data::remove()
{
	if(select_caret < caret)
		swap(&caret, &select_caret);
	tl.glyphs.remove_range(caret, select_caret);
	select_caret = caret;
}

void text_field_data::scroll_to_caret(handleable<frame> fm)
{
	if(tl.glyphs.size == 0) return;
	rectangle<int32> content_viewport = utility<frame>().frame_content_viewport(fm.core);
	vector<int32, 2> point;
	int32 line_height;
	tl.hit_test_position(caret, &point, &line_height);
	if(tl.multiline)
	{
		point.y += content_viewport.extent.y + int32(scroll.data.viewport_offset) - int32(tl.height);
		if(point.y < 0)
			scroll.data.shift(uint32(-point.y), false);
		else if(point.y + line_height >= content_viewport.extent.y)
			scroll.data.shift(point.y + line_height - content_viewport.extent.y, true);
	}
	else
	{
		point.x -= int32(scroll.data.viewport_offset);
		if(point.x < 0)
			scroll.data.shift(uint32(-point.x), true);
		else if(point.x >= content_viewport.extent.x)
			scroll.data.shift(point.x - content_viewport.extent.x, false);
	}
}

void text_field_data::subframes(handleable<frame> fm, array<handleable<frame>> *frames)
{
	frames->push(handleable<frame>(&scroll, &scroll.fm));
}

vector<uint32, 2> text_field_data::content_size(handleable<frame> fm, uint32 viewport_width, uint32 viewport_height)
{
	uint32 tl_width = tl.width;
	tl.width = 1000000000;
	vector<int32, 2> size = tl.content_size();
	tl.width = tl_width;
	return vector<uint32, 2>(uint32(size.x), uint32(size.y));
}

struct text_field_render_glyph_args
{
	frame *fm;
	text_field_data *tf;
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
	text_field_render_glyph_args *args = (text_field_render_glyph_args *)(data);
	rectangle<int32> content_viewport = utility<frame>().frame_content_viewport(args->fm);
	content_viewport.position -= args->base_point;
	if(point.y - baseline + line_height < content_viewport.position.y
		|| point.y - baseline > content_viewport.position.y + content_viewport.extent.y)
		return;
	rectangle<int32> rect;
	uint64 begin = args->tf->caret, end = args->tf->select_caret;
	if(end < begin) swap(&begin, &end);
	if(args->tf->selection_visible && begin <= idx && idx < end)
	{
		rect.position = vector<int32, 2>(point.x, point.y - baseline);
		rect.extent = vector<int32, 2>(gl.data->advance.x, line_height);
		args->bp->br.switch_solid_color(alpha_color(0, 0, 200, 255));
		args->bp->fill_area(rect, bmp);
	}
	if(args->tf->selection_visible && begin <= idx && idx < end)
		args->bp->br.switch_solid_color(alpha_color(255, 255, 255, 255));
	else args->bp->br.switch_solid_color(alpha_color(0, 0, 0, 255));
	args->bp->fill_opacity_bitmap(
		gl.data->bmp,
		vector<int32, 2>(
			point.x + int32(gl.data->bmp_offset.x),
			point.y + int32(gl.data->bmp_offset.y)),
		bmp);
	if(gl.underlined)
	{
		rect.position = vector<int32, 2>(point.x, point.y + gl.data->underline_offset);
		rect.extent = vector<int32, 2>(gl.data->advance.x, gl.data->underline_size);
		args->bp->fill_area(rect, bmp);
	}
	if(gl.strikedthrough)
	{
		rect.position = vector<int32, 2>(point.x, point.y + gl.data->strikethrough_offset);
		rect.extent = vector<int32, 2>(gl.data->advance.x, gl.data->strikethrough_size);
		args->bp->fill_area(rect, bmp);
	}
	if(caret_visible
		&& focused_frame().core == args->fm
		&& args->tf->editable
		&& (idx == args->tf->caret
		|| idx == args->tf->caret - 1
			&& args->tf->caret == args->tf->tl.glyphs.size))
	{
		rectangle<real> caret_rect;
		geometry_path caret_path;
		caret_rect.position = vector<real, 2>(real(point.x), real(point.y - baseline) + 0.1r * real(line_height));
		if(args->tf->caret == args->tf->tl.glyphs.size)
			caret_rect.position.x += real(gl.data->advance.x);
		caret_rect.extent = vector<real, 2>(2.0r, 0.8r * real(line_height));
		caret_path.push_rectangle(caret_rect);
		set_identity_matrix(&args->bp->transform);
		args->bp->br.switch_solid_color(alpha_color(0, 0, 0, 255));
		args->bp->render(caret_path, bmp);
	}
}

void text_field_data::render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle viewport = utility<frame>().frame_viewport(fm.core),
		content_viewport = utility<frame>().frame_content_viewport(fm.core);
	viewport.position -= point;
	content_viewport.position -= point;
	if(!fm.core->visible
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	scroll.fm.width = scroll.fm.width_desc.value.integer;
	scroll.fm.height = uint32(viewport.extent.y);
	scroll.fm.x = point.x + viewport.position.x + viewport.extent.x - scroll.fm.width;
	scroll.fm.y = point.y + viewport.position.y;
	tl.width = uint32(content_viewport.extent.x);
	if(tl.multiline)
	{
		scroll.fm.visible = true;
		scroll.data.content_size = uint32(tl.content_size().y);
		scroll.data.viewport_size = uint32(content_viewport.extent.y);
		if(scroll.data.content_size > scroll.data.viewport_size)
		{
			content_viewport.extent.x -= scroll.fm.width;
			scroll.data.viewport_offset
				= min(scroll.data.viewport_offset, scroll.data.content_size - scroll.data.viewport_size);
		}
		else
		{
			scroll.fm.visible = false;
			scroll.data.viewport_offset = 0;
		}
		if(content_viewport.extent.x <= 0) return;
	}
	else
	{
		scroll.fm.visible = false;
		scroll.data.content_size = uint32(tl.content_size().x);
		scroll.data.viewport_size = uint32(content_viewport.extent.x);
		if(scroll.data.content_size > scroll.data.viewport_size)
			scroll.data.viewport_offset
				= min(scroll.data.viewport_offset, scroll.data.content_size - scroll.data.viewport_size);
		else scroll.data.viewport_offset = 0;
	}
	tl.width = uint32(content_viewport.extent.x);
	tl.height = uint32(content_viewport.extent.y);
	bp->push_scissor(viewport);
	text_field_render_glyph_args args;
	args.fm = fm.core;
	args.tf = this;
	args.bp = bp;
	args.base_point = point;
	vector<int32, 2> text_point;
	if(tl.multiline)
		text_point = vector<int32, 2>(
			content_viewport.position.x,
			content_viewport.position.y
				+ content_viewport.extent.y
				- int32(tl.height)
				+ int32(scroll.data.viewport_offset));
	else text_point = vector<int32, 2>(
		content_viewport.position.x - int32(scroll.data.viewport_offset),
		content_viewport.position.y
			+ content_viewport.extent.y
			- int32(tl.height));
	tl.render(
		text_point,
		text_field_render_glyph,
		&args,
		bmp);
	bp->pop_scissor();
	if(caret_visible
		&& focused_frame().object.addr == fm.object.addr
		&& editable
		&& tl.glyphs.size == 0)
	{
		rectangle<real> rect;
		rect.position = vector<real, 2>(
			content_viewport.position.x,
			real(content_viewport.position.y + content_viewport.extent.y) - 0.9r * real(font_size));
		rect.extent = vector<real, 2>(2.0r, 0.8r * real(font_size));
		geometry_path rect_path;
		rect_path.push_rectangle(rect);
		bp->rasterization = rasterization_mode::fill;
		set_identity_matrix(&bp->transform);
		bp->br.switch_solid_color(alpha_color(0, 0, 0, 255));
		bp->render(rect_path, bmp);
	}
	scroll.fm.render(&scroll.fm, point, bp, bmp);
}

void text_field_data::mouse_click(handleable<frame> fm)
{
	rectangle<int32> content_viewport = utility<frame>().frame_content_viewport(fm.core);
	if(tl.multiline)
		tl.hit_test_point(
			vector<int32, 2>(
				mouse()->position.x - content_viewport.position.x,
				mouse()->position.y - (content_viewport.position.y + int32(scroll.data.viewport_offset))),
			&caret);
	else tl.hit_test_point(
			vector<int32, 2>(
				mouse()->position.x - (content_viewport.position.x - int32(scroll.data.viewport_offset)),
				mouse()->position.y - content_viewport.position.y),
			&caret);
	select_caret = caret;
	selecting = true;
}

void text_field_data::mouse_move(handleable<frame> fm)
{
	rectangle<int32> content_viewport = utility<frame>().frame_content_viewport(fm.core);
	if(selecting)
	{
		if(!mouse()->left_pressed)
			selecting = false;
		else
		{
			if(tl.multiline)
				tl.hit_test_point(
					vector<int32, 2>(
						mouse()->position.x - content_viewport.position.x,
						mouse()->position.y - (content_viewport.position.y + int32(scroll.data.viewport_offset))),
					&caret);
			else tl.hit_test_point(
				vector<int32, 2>(
					mouse()->position.x - (content_viewport.position.x - int32(scroll.data.viewport_offset)),
					mouse()->position.y - content_viewport.position.y),
				&caret);
		}
	}
}

void text_field_data::focus_receive(handleable<frame> fm)
{
	if(!editable) return;
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

void text_field_data::focus_loss(handleable<frame> fm)
{
	if(caret_timer != nullptr)
		caret_timer->reset();
	caret = 0;
	select_caret = 0;
}

void text_field_data::mouse_wheel_rotate(handleable<frame> fm)
{
	scroll.data.shift(50, mouse()->wheel_forward);
}

void text_field_data::key_press(handleable<frame> fm)
{
	if(keyboard()->pressed_count == 1)
	{
		if(keyboard()->key_pressed[uint8(key_code::left)] && caret != 0)
		{
			caret--;
			select_caret = caret;
			scroll_to_caret(fm);
		}
		else if(keyboard()->key_pressed[uint8(key_code::right)] && caret != tl.glyphs.size)
		{
			caret++;
			select_caret = caret;
			scroll_to_caret(fm);
		}
		else if(keyboard()->key_pressed[uint8(key_code::down)] && tl.multiline && tl.glyphs.size != 0)
		{
			vector<int32, 2> point;
			int32 line_height;
			tl.hit_test_position(caret, &point, &line_height);
			point.y--;
			tl.hit_test_point(point, &caret);
			select_caret = caret;
			scroll_to_caret(fm);
		}
		else if(keyboard()->key_pressed[uint8(key_code::up)] && tl.multiline && tl.glyphs.size != 0)
		{
			vector<int32, 2> point;
			int32 line_height;
			tl.hit_test_position(caret, &point, &line_height);
			point.y += line_height + 1;
			tl.hit_test_point(point, &caret);
			select_caret = caret;
			scroll_to_caret(fm);
		}
	}
	else if(keyboard()->pressed_count == 2 && keyboard()->key_pressed[uint8(key_code::ctrl)])
	{
		if(keyboard()->key_pressed[uint8(key_code::c)] && caret != select_caret)
		{
			uint64 begin = caret, end = select_caret;
			if(end < begin) swap(&begin, &end);
			string str;
			while(begin < end)
			{
				str.push(tl.glyphs.addr[begin].code);
				begin++;
			}
			os_copy_text_to_clipboard(str);
		}
		else if(keyboard()->key_pressed[uint8(key_code::v)] && editable)
		{
			string str;
			os_copy_text_from_clipboard(&str);
			insert(str);
			scroll_to_caret(fm);
		}
		else if(keyboard()->key_pressed[uint8(key_code::x)] && editable)
		{
			uint64 begin = caret, end = select_caret;
			if(end < begin) swap(&begin, &end);
			string str;
			while(begin < end)
			{
				str.push(tl.glyphs.addr[begin].code);
				begin++;
			}
			os_copy_text_to_clipboard(str);
			remove();
			scroll_to_caret(fm);
		}
		else if(keyboard()->key_pressed[uint8(key_code::a)])
		{
			select(tl.glyphs.size, 0);
			scroll_to_caret(fm);
		}
	}
}

void text_field_data::char_input(handleable<frame> fm)
{
	if(!editable
		|| keyboard()->key_pressed[uint8(key_code::ctrl)]
		|| keyboard()->key_pressed[uint8(key_code::alt)])
		return;
	if(keyboard()->char_code == char32(key_code::backspace))
	{
		if(caret == select_caret
			&& caret != 0
			&& tl.glyphs.size != 0)
			select(caret - 1, caret);
		remove();
		scroll_to_caret(fm);
	}
	else if(keyboard()->char_code == char32(key_code::enter)
		&& !tl.multiline
		&& focused_frame().object.addr == fm.object.addr)
		focus_frame(handleable<frame>(nullptr, nullptr));
	else if(keyboard()->char_code == char32(key_code::enter)
		|| keyboard()->char_code == char32(key_code::tab)
		|| keyboard()->char_code >= U' ')
	{
		string str;
		if(keyboard()->char_code == char32(key_code::enter))
			str.push(U'\n');
		else str.push(keyboard()->char_code);
		insert(str);
		scroll_to_caret(fm);
	}
}

void text_field_model::attach(handleable<frame> fm)
{
	rendering_size = vector<uint32, 2>(0, 0);
	fm.core->padding_left = 4uiabs;
	fm.core->padding_bottom = 4uiabs;
	fm.core->padding_right = 4uiabs;
	fm.core->padding_top = 4uiabs;
}

void text_field_model::render(handleable<frame> fm, text_field_data *data, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle viewport = utility<frame>().frame_viewport(fm.core),
		content_viewport = utility<frame>().frame_content_viewport(fm.core);
	viewport.position -= vector<int32, 2>(fm.core->x, fm.core->y);
	if(!fm.core->visible
		|| !data->editable
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	if(fm.core->width != rendering_size.x || fm.core->height != rendering_size.y)
	{
		rendering_size = vector<uint32, 2>(fm.core->width, fm.core->height);
		surface.resize(fm.core->width, fm.core->height);
		for(uint32 i = 0; i < surface.width * surface.height; i++)
			surface.data[i] = alpha_color(0, 0, 0, 0);
		geometry_path path;
		path.push_rectangle(rectangle<real>(viewport));
		bitmap_processor bp_surface;
		bp_surface.br.switch_solid_color(alpha_color(255, 255, 255, 255));
		bp_surface.render(path, &surface);
		bp_surface.rasterization = rasterization_mode::outline;
		bp_surface.line_width = 1.0r;
		bp_surface.br.switch_solid_color(alpha_color(0, 0, 0, 255));
		bp_surface.render(path, &surface);
	}
	bp->fill_bitmap(surface, vector<int32, 2>(fm.core->x, fm.core->y) - point, bmp);
}

bool text_field_hit_test(indefinite<frame> fm, vector<int32, 2> point)
{
	text_field *tf = (text_field *)(fm.addr);
	return utility<frame>().rectangular_hit_test(&tf->fm, point);
}

void text_field_subframes(indefinite<frame> fm, array<handleable<frame>> *frames)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.subframes(handleable<frame>(tf, &tf->fm), frames);
}

vector<uint32, 2> text_field_content_size(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height)
{
	text_field *tf = (text_field *)(fm.addr);
	return tf->data.content_size(handleable<frame>(tf, &tf->fm), viewport_width, viewport_height);
}

void text_field_render(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->model.render(handleable<frame>(tf, &tf->fm), &tf->data, point, bp, bmp);
	tf->data.render(handleable<frame>(tf, &tf->fm), point, bp, bmp);
}

void text_field_mouse_click(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.mouse_click(handleable<frame>(tf, &tf->fm));
}

void text_field_mouse_move(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.mouse_move(handleable<frame>(tf, &tf->fm));
}

void text_field_focus_receive(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.focus_receive(handleable<frame>(tf, &tf->fm));
}

void text_field_focus_loss(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.focus_loss(handleable<frame>(tf, &tf->fm));
}

void text_field_mouse_wheel_rotate(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.mouse_wheel_rotate(handleable<frame>(tf, &tf->fm));
}

void text_field_key_press(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.key_press(handleable<frame>(tf, &tf->fm));
}

void text_field_char_input(indefinite<frame> fm)
{
	text_field *tf = (text_field *)(fm.addr);
	tf->data.char_input(handleable<frame>(tf, &tf->fm));
}

text_field::text_field()
{
	fm.hit_test = text_field_hit_test;
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
	data.attach(handleable<frame>(this, &fm));
	model.attach(handleable<frame>(this, &fm));
}

struct text_field_module_initializer
{
	text_field_module_initializer()
	{
		default_frame_callbacks()->text_field_hit_test = text_field_hit_test;
		default_frame_callbacks()->text_field_content_size = text_field_content_size;
		default_frame_callbacks()->text_field_subframes = text_field_subframes;
		default_frame_callbacks()->text_field_render = text_field_render;
		default_frame_callbacks()->text_field_mouse_click = text_field_mouse_click;
		default_frame_callbacks()->text_field_mouse_move = text_field_mouse_move;
		default_frame_callbacks()->text_field_mouse_wheel_rotate = text_field_mouse_wheel_rotate;
		default_frame_callbacks()->text_field_focus_receive = text_field_focus_receive;
		default_frame_callbacks()->text_field_focus_loss = text_field_focus_loss;
		default_frame_callbacks()->text_field_key_press = text_field_key_press;
		default_frame_callbacks()->text_field_char_input = text_field_char_input;
	}
} initializer;
