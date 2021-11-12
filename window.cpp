#include "frame_templates.h"
#include "os_api.h"

window::window()
{
	fm.data = this;
	fm.width_desc = 600uiabs;
	fm.height_desc = 400uiabs;
	fm.width = 0;
	fm.height = 0;
	fm.mouse_click = window_mouse_click;
	fm.mouse_release = window_mouse_release;
	fm.mouse_move = window_mouse_move;
	fm.mouse_wheel_rotate = window_mouse_wheel_rotate;
	fm.key_press = window_key_press;
	fm.key_release = window_key_release;
	fm.char_input = window_char_input;
	os_create_window(this);
}

window::~window()
{
	os_destroy_window(this);
}

void window::update()
{
	vector<int32, 2> position = os_window_content_position(this);
	vector<uint32, 2> size = os_window_content_size(this);
	if(fm.width != size.x || fm.height != size.y)
	{
		bmp.resize(size.x, size.y);
		os_update_window_size(this);
	}
	fm.x = position.x;
	fm.y = position.y;
	fm.width = size.x;
	fm.height = size.y;
	layout->x = fm.x;
	layout->y = fm.y;
	layout->width = fm.width;
	layout->height = fm.height;
	bitmap_processor bp;
	layout->render(layout, vector<int32, 2>(fm.x, fm.y), &bp, &bmp);
	os_render_window(this);
}

void window::open()
{
	os_open_window(this);
}

void window::close()
{
	
}

void window::hide()
{

}

frame *window_mouse_event_receiver(window *wnd, void (*mouse_callback)(frame *fm))
{
	frame *fm = wnd->layout;
	array<frame *> frames;
	wnd->layout->subframes(wnd->layout, &frames);
	uint64 i = 0;
	while(i < frames.size)
	{
		if(frames.addr[i]->visible && frames.addr[i]->hit_test(frames.addr[i], mouse()->position))
		{
			if(mouse_callback == wnd->fm.mouse_click)
			{
				if(frames.addr[i]->hook_mouse_click) return frames.addr[i];
				else if(frames.addr[i]->return_mouse_click) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_release)
			{
				if(frames.addr[i]->hook_mouse_release) return frames.addr[i];
				else if(frames.addr[i]->return_mouse_release) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_move)
			{
				if(frames.addr[i]->hook_mouse_move) return frames.addr[i];
				else if(frames.addr[i]->return_mouse_move) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_wheel_rotate)
			{
				if(frames.addr[i]->hook_mouse_wheel_rotate) return frames.addr[i];
				else if(frames.addr[i]->return_mouse_wheel_rotate) return fm;
			}
			fm = frames.addr[i];
			frames.clear();
			frames.addr[i]->subframes(frames.addr[i], &frames);
			i = 0;
		}
		else i++;
	}
	return fm;
}

void window_mouse_click(frame *fm)
{
	window *wnd = (window *)(fm->data);
	frame *receiver = window_mouse_event_receiver(wnd, window_mouse_click);
	if(receiver->focusable) focus_frame(receiver);
	else focus_frame(nullptr);
	receiver->mouse_click(receiver);
}

void window_mouse_release(frame *fm)
{
	window *wnd = (window *)(fm->data);
	frame *receiver = window_mouse_event_receiver(wnd, window_mouse_release);
	receiver->mouse_release(receiver);
	if(pulled_frame() != nullptr)
	{
		pulled_frame()->mouse_release(pulled_frame());
		pull_frame(nullptr);
	}
}

void window_mouse_move(frame *fm)
{
	window *wnd = (window *)(fm->data);
	frame *receiver = window_mouse_event_receiver(wnd, window_mouse_move);
	hover_frame(receiver);
	receiver->mouse_move(receiver);
	if(pulled_frame() != nullptr)
		pulled_frame()->mouse_move(pulled_frame());
}

void window_mouse_wheel_rotate(frame *fm)
{
	window *wnd = (window *)(fm->data);
	frame *receiver = window_mouse_event_receiver(wnd, window_mouse_wheel_rotate);
	receiver->mouse_wheel_rotate(receiver);
}

void window_key_press(frame *fm)
{
	if(focused_frame() != nullptr)
		focused_frame()->key_press(focused_frame());
}

void window_key_release(frame *fm)
{
	if(focused_frame() != nullptr)
		focused_frame()->key_release(focused_frame());
}

void window_char_input(frame *fm)
{
	if(focused_frame() != nullptr)
		focused_frame()->char_input(focused_frame());
}
