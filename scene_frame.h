#pragma once
#include "graphics.h"
#include "frame.h"

struct scene
{
	void *wnd_handler;
	void *render_device;

	scene();
	~scene();
	void set_window(void *wnd_handler_value);
	void render(bitmap &upper_layer);

};

struct scene_frame
{
	frame fm;
	scene *sc;
	handleable<frame> layout;

	scene_frame();
};
