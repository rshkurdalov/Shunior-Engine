#include "frame_templates.h"
#include "os_api.h"

handleable<frame> window_mouse_event_receiver(window *wnd, void (*mouse_callback)(indefinite<frame> fm))
{
	handleable<frame> fm = wnd->layout;
	array<handleable<frame>> frames;
	wnd->layout.core->subframes(wnd->layout.object, &frames);
	uint64 i = 0;
	while(i < frames.size)
	{
		if(frames.addr[i].core->visible
			&& frames.addr[i].core->hit_test(frames.addr[i].object, mouse()->position))
		{
			if(mouse_callback == wnd->fm.mouse_click)
			{
				if(frames.addr[i].core->hook_mouse_click) return frames.addr[i];
				else if(frames.addr[i].core->return_mouse_click) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_release)
			{
				if(frames.addr[i].core->hook_mouse_release) return frames.addr[i];
				else if(frames.addr[i].core->return_mouse_release) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_move)
			{
				if(frames.addr[i].core->hook_mouse_move) return frames.addr[i];
				else if(frames.addr[i].core->return_mouse_move) return fm;
			}
			else if(mouse_callback == wnd->fm.mouse_wheel_rotate)
			{
				if(frames.addr[i].core->hook_mouse_wheel_rotate) return frames.addr[i];
				else if(frames.addr[i].core->return_mouse_wheel_rotate) return fm;
			}
			fm = frames.addr[i];
			frames.clear();
			frames.addr[i].core->subframes(frames.addr[i].object, &frames);
			i = 0;
		}
		else i++;
	}
	return fm;
}

void window_mouse_click(indefinite<frame> fm)
{
	window *wnd = (window *)(fm.addr);
	handleable<frame> receiver = window_mouse_event_receiver(wnd, window_mouse_click);
	if(receiver.core->focusable) focus_frame(receiver);
	else focus_frame(handleable<frame>(nullptr, nullptr));
	receiver.core->mouse_click(receiver.object);
}

void window_mouse_release(indefinite<frame> fm)
{
	window *wnd = (window *)(fm.addr);
	handleable<frame> receiver = window_mouse_event_receiver(wnd, window_mouse_release);
	receiver.core->mouse_release(receiver.object);
	if(pulled_frame().object.addr != nullptr)
	{
		pulled_frame().core->mouse_release(pulled_frame().object);
		pull_frame(handleable<frame>(nullptr, nullptr));
	}
}

void window_mouse_move(indefinite<frame> fm)
{
	window *wnd = (window *)(fm.addr);
	handleable<frame> receiver = window_mouse_event_receiver(wnd, window_mouse_move);
	hover_frame(receiver);
	receiver.core->mouse_move(receiver.object.addr);
	if(pulled_frame().object.addr != nullptr)
		pulled_frame().core->mouse_move(pulled_frame().object);
}

void window_mouse_wheel_rotate(indefinite<frame> fm)
{
	window *wnd = (window *)(fm.addr);
	handleable<frame> receiver = window_mouse_event_receiver(wnd, window_mouse_wheel_rotate);
	receiver.core->mouse_wheel_rotate(receiver.object);
}

void window_key_press(indefinite<frame> fm)
{
	if(focused_frame().object.addr != nullptr)
		focused_frame().core->key_press(focused_frame().object);
}

void window_key_release(indefinite<frame> fm)
{
	if(focused_frame().object.addr != nullptr)
		focused_frame().core->key_release(focused_frame().object);
}

void window_char_input(indefinite<frame> fm)
{
	if(focused_frame().object.addr != nullptr)
		focused_frame().core->char_input(focused_frame().object);
}

window::window()
{
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
	layout.core->x = fm.x;
	layout.core->y = fm.y;
	layout.core->width = fm.width;
	layout.core->height = fm.height;
	for(uint32 i = 0; i < bmp.width * bmp.height; i++)
		bmp.data[i] = alpha_color(0, 0, 0, 0);
	bitmap_processor bp;
	layout.core->render(layout.object, vector<int32, 2>(fm.x, fm.y), &bp, &bmp);
	//os_render_window(this);
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

struct window_module_initializer
{
	window_module_initializer()
	{
		default_frame_callbacks()->window_mouse_click = window_mouse_click;
		default_frame_callbacks()->window_key_press = window_key_press;
		default_frame_callbacks()->window_mouse_release = window_key_release;
		default_frame_callbacks()->window_mouse_move = window_mouse_move;
		default_frame_callbacks()->window_mouse_wheel_rotate = window_mouse_wheel_rotate;
		default_frame_callbacks()->window_key_press = window_key_press;
		default_frame_callbacks()->window_key_release = window_key_release;
		default_frame_callbacks()->window_char_input = window_char_input;
	}
} initializer;
