#pragma once
#include "global_types.h"
#include "frame_templates.h"
#include "text_layout.h"

void os_create_window(window *wnd);
void os_destroy_window(window *wnd);
void os_open_window(window *wnd);
void os_resize_window(window *wnd, uint32 width, uint32 height);
void os_update_window_size(window *wnd);
vector<int32, 2> os_window_content_position(window *wnd);
vector<uint32, 2> os_window_content_size(window *wnd);
void os_message_loop(window *wnd);
void os_render_window(window *wnd);
bool os_load_glyph(glyph_data *data);
uint64 os_current_timestamp();
void os_copy_text_to_clipboard(string &text);
void os_copy_text_from_clipboard(string *text);
