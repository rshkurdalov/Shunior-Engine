#include "frame_templates.h"

void push_button_data::attach(text_field_data *tf_data)
{
	tf_data->tl.halign = horizontal_align::center;
	tf_data->selection_visible = false;
	tf_data->editable = false;
	button_click = nullptr;
}

void push_button_data::mouse_click(handleable<frame> fm)
{
	if(mouse()->last_clicked == mouse_button::left)
		pull_frame(fm);
}

void push_button_data::mouse_release(handleable<frame> fm)
{
	if(pulled_frame().object.addr == fm.object.addr
		&& fm.core->hit_test(fm.object, mouse()->position)
		&& button_click != nullptr)
		button_click(data);
}

void push_button_model::attach(handleable<frame> fm)
{
	rendering_size = vector<uint32, 2>(0, 0);
}

void push_button_model::render(handleable<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle viewport = utility<frame>().frame_viewport(fm.core),
		content_viewport = utility<frame>().frame_content_viewport(fm.core);
	viewport.position -= vector<int32, 2>(fm.core->x, fm.core->y);
	if(!fm.core->visible
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	if(fm.core->width != rendering_size.x || fm.core->height != rendering_size.y)
	{
		rendering_size = vector<uint32, 2>(fm.core->width, fm.core->height);
		inner_surface.resize(fm.core->width, fm.core->height);
		border_surface.resize(fm.core->width, fm.core->height);
		for(uint32 i = 0; i < fm.core->width * fm.core->height; i++)
		{
			inner_surface.data[i] = alpha_color(0, 0, 0, 0);
			border_surface.data[i] = alpha_color(0, 0, 0, 0);
		}
		geometry_path path;
		rounded_rectangle<real> rrect(
			rectangle<real>(viewport),
			0.07r * real(viewport.extent.x),
			0.07r * real(viewport.extent.y));
		path.push_rounded_rectangle(rrect);
		bitmap_processor bp_surface;
		bp_surface.br.switch_solid_color(alpha_color(0, 0, 0, 255));
		bp_surface.render(path, &inner_surface);
		bp_surface.rasterization = rasterization_mode::outline;
		bp_surface.line_width = 2.0r;
		bp_surface.br.switch_solid_color(alpha_color(100, 100, 100, 255));
		bp_surface.render(path, &border_surface);
	}
	if(pulled_frame().object.addr == fm.object.addr)
		bp->br.switch_solid_color(alpha_color(170, 170, 170, 255));
	else if(hovered_frame().object.addr == fm.object.addr)
		bp->br.switch_solid_color(alpha_color(185, 185, 185, 255));
	else bp->br.switch_solid_color(alpha_color(200, 200, 200, 255));
	bp->fill_opacity_bitmap(inner_surface, vector<int32, 2>(fm.core->x, fm.core->y) - point, bmp);
	if(pulled_frame().object.addr == fm.object.addr || hovered_frame().object.addr == fm.object.addr)
	{
		gradient_stop stops[2];
		stops[0].offset = 0.0r;
		stops[0].color = alpha_color(140, 140, 140, 255);
		stops[1].offset = 1.0r;
		stops[1].color = alpha_color(140, 140, 140, 0);
		vector<real, 2> position = vector<real, 2>(mouse()->position - point);
		viewport.position += vector<int32, 2>(fm.core->x, fm.core->y) - point;
		position.x = max(position.x, real(viewport.position.x));
		position.x = min(position.x, real(viewport.position.x + viewport.extent.x));
		position.y = max(position.y, real(viewport.position.y));
		position.y = min(position.y, real(viewport.position.y + viewport.extent.y));
		real r = real(max(viewport.extent.x, viewport.extent.y) / 2);
		bp->br.switch_radial_gradient(stops, array_size(stops), position, vector<real, 2>(0.0r, 0.0r), r, r);
		bp->color_interpolation = color_interpolation_mode::linear;
		bp->fill_opacity_bitmap(inner_surface, vector<int32, 2>(fm.core->x, fm.core->y) - point, bmp);
	}
	bp->fill_bitmap(border_surface, vector<int32, 2>(fm.core->x, fm.core->y) - point, bmp);
}

bool push_button_hit_test(indefinite<frame> fm, vector<int32, 2> point)
{
	push_button *pb = (push_button *)(fm.addr);
	return utility<frame>().rectangular_hit_test(&pb->fm, point);
}

void push_button_subframes(indefinite<frame> fm, array<handleable<frame>> *frames)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.subframes(handleable<frame>(pb, &pb->fm), frames);
}

vector<uint32, 2> push_button_content_size(indefinite<frame> fm, uint32 viewport_width, uint32 viewport_height)
{
	push_button *pb = (push_button *)(fm.addr);
	return pb->tf_data.content_size(handleable<frame>(pb, &pb->fm), viewport_width, viewport_height);
}

void push_button_render(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->model.render(handleable<frame>(pb, &pb->fm), point, bp, bmp);
	pb->tf_data.render(handleable<frame>(pb, &pb->fm), point, bp, bmp);
}

void push_button_mouse_click(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.mouse_click(handleable<frame>(pb, &pb->fm));
	pb->pb_data.mouse_click(handleable<frame>(pb, &pb->fm));
}

void push_button_mouse_release(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->pb_data.mouse_release(handleable<frame>(pb, &pb->fm));
}

void push_button_mouse_move(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.mouse_move(handleable<frame>(pb, &pb->fm));
}

void push_button_focus_receive(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.focus_receive(handleable<frame>(pb, &pb->fm));
}

void push_button_focus_loss(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.focus_loss(handleable<frame>(pb, &pb->fm));
}

void push_button_mouse_wheel_rotate(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.mouse_wheel_rotate(handleable<frame>(pb, &pb->fm));
}

void push_button_key_press(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.key_press(handleable<frame>(pb, &pb->fm));
}

void push_button_char_input(indefinite<frame> fm)
{
	push_button *pb = (push_button *)(fm.addr);
	pb->tf_data.char_input(handleable<frame>(pb, &pb->fm));
}

push_button::push_button()
{
	fm.hit_test = push_button_hit_test;
	fm.subframes = push_button_subframes;
	fm.content_size = push_button_content_size;
	fm.render = push_button_render;
	fm.mouse_click = push_button_mouse_click;
	fm.mouse_release = push_button_mouse_release;
	fm.mouse_move = push_button_mouse_move;
	fm.focus_receive = push_button_focus_receive;
	fm.focus_loss = push_button_focus_loss;
	fm.mouse_wheel_rotate = push_button_mouse_wheel_rotate;
	fm.key_press = push_button_key_press;
	fm.char_input = push_button_char_input;
	tf_data.attach(handleable<frame>(this, &fm));
	pb_data.attach(&tf_data);
}

struct push_button_module_initializer
{
	push_button_module_initializer()
	{
		default_frame_callbacks()->push_button_hit_test = push_button_hit_test;
		default_frame_callbacks()->push_button_content_size = push_button_content_size;
		default_frame_callbacks()->push_button_subframes = push_button_subframes;
		default_frame_callbacks()->push_button_render = push_button_render;
		default_frame_callbacks()->push_button_mouse_click = push_button_mouse_click;
		default_frame_callbacks()->push_button_mouse_release = push_button_mouse_release;
		default_frame_callbacks()->push_button_mouse_move = push_button_mouse_move;
		default_frame_callbacks()->push_button_mouse_wheel_rotate = push_button_mouse_wheel_rotate;
		default_frame_callbacks()->push_button_focus_receive = push_button_focus_receive;
		default_frame_callbacks()->push_button_focus_loss = push_button_focus_loss;
		default_frame_callbacks()->push_button_key_press = push_button_key_press;
		default_frame_callbacks()->push_button_char_input = push_button_char_input;
	}
} initializer;
