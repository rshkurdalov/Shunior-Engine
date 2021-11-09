#include "frame_templates.h"

void push_button_data::attach(text_field_data *tf_data)
{
	tf_data->tl.halign = horizontal_align::center;
	tf_data->selection_visible = false;
	tf_data->editable = false;
	button_click = nullptr;
}

void push_button_data::mouse_click(frame *fm)
{
	if(mouse()->last_clicked == mouse_button::left)
		pull_frame(fm);
}

void push_button_data::mouse_release(frame *fm)
{
	if(pulled_frame() == fm
		&& fm->hit_test(fm, mouse()->position)
		&& button_click != nullptr)
		button_click(data);
}

void push_button_model::attach(frame *fm)
{
	rendered_size = vector<uint32, 2>(0, 0);
}

void push_button_model::render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	rectangle viewport = frame_viewport(fm),
		content_viewport = frame_content_viewport(fm);
	viewport.position -= vector<int32, 2>(fm->x, fm->y);
	if(!fm->visible
		|| viewport.extent.x <= 0 || viewport.extent.y <= 0
		|| content_viewport.extent.x <= 0 || content_viewport.extent.y <= 0)
		return;
	if(fm->width != rendered_size.x || fm->height != rendered_size.y)
	{
		rendered_size = vector<uint32, 2>(fm->width, fm->height);
		inner_surface.resize(fm->width, fm->height);
		border_surface.resize(fm->width, fm->height);
		for(uint32 i = 0; i < fm->width * fm->height; i++)
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
		bp_surface.set_solid_color_brush(alpha_color(0, 0, 0, 255));
		bp_surface.render(path, &inner_surface);
		bp_surface.rasterization = rasterization_mode::outline;
		bp_surface.line_width = 2.0r;
		bp_surface.set_solid_color_brush(alpha_color(100, 100, 100, 255));
		bp_surface.render(path, &border_surface);
	}
	if(pulled_frame() == fm)
		bp->set_solid_color_brush(alpha_color(170, 170, 170, 255));
	else if(hovered_frame() == fm)
		bp->set_solid_color_brush(alpha_color(185, 185, 185, 255));
	else bp->set_solid_color_brush(alpha_color(200, 200, 200, 255));
	bp->fill_opacity_bitmap(inner_surface, vector<int32, 2>(fm->x, fm->y) - point, bmp);
	if(pulled_frame() == fm || hovered_frame() == fm)
	{
		gradient_stop stops[2];
		stops[0].offset = 0.0r;
		stops[0].color = alpha_color(140, 140, 140, 255);
		stops[1].offset = 1.0r;
		stops[1].color = alpha_color(140, 140, 140, 0);
		vector<real, 2> position = vector<real, 2>(mouse()->position - point);
		viewport.position += vector<int32, 2>(fm->x, fm->y) - point;
		position.x = max(position.x, real(viewport.position.x));
		position.x = min(position.x, real(viewport.position.x + viewport.extent.x));
		position.y = max(position.y, real(viewport.position.y));
		position.y = min(position.y, real(viewport.position.y + viewport.extent.y));
		real r = real(max(viewport.extent.x, viewport.extent.y) / 2);
		bp->set_radial_gradient_brush(stops, array_size(stops), position, vector<real, 2>(0.0r, 0.0r), r, r);
		bp->color_interpolation = color_interpolation_mode::linear;
		bp->fill_opacity_bitmap(inner_surface, vector<int32, 2>(fm->x, fm->y) - point, bmp);
	}
	bp->fill_bitmap(border_surface, vector<int32, 2>(fm->x, fm->y) - point, bmp);
}

void push_button_subframes(frame *fm, array<frame *> *frames)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.subframes(fm, frames);
}

vector<uint32, 2> push_button_content_size(frame *fm, uint32 viewport_width, uint32 viewport_height)
{
	push_button *pb = (push_button *)(fm->data);
	return pb->tf_data.content_size(fm, viewport_width, viewport_height);
}

void push_button_render(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	push_button *pb = (push_button *)(fm->data);
	pb->model.render(fm, point, bp, bmp);
	pb->tf_data.render(fm, point, bp, bmp);
}

void push_button_mouse_click(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.mouse_click(fm);
	pb->pb_data.mouse_click(fm);
}

void push_button_mouse_release(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->pb_data.mouse_release(fm);
}

void push_button_mouse_move(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.mouse_move(fm);
}

void push_button_focus_receive(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.focus_receive(fm);
}

void push_button_focus_loss(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.focus_loss(fm);
}

void push_button_mouse_wheel_rotate(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.mouse_wheel_rotate(fm);
}

void push_button_key_press(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.key_press(fm);
}

void push_button_char_input(frame *fm)
{
	push_button *pb = (push_button *)(fm->data);
	pb->tf_data.char_input(fm);
}

push_button::push_button()
{
	fm.data = this;
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
	tf_data.attach(&fm);
	pb_data.attach(&tf_data);
}
