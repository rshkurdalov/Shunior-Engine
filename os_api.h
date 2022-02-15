#pragma once
#include "global_types.h"
#include "frame_templates.h"
#include "text_layout.h"
#include "file.h"
#include "network.h"

void os_create_window(window *wnd);
void os_destroy_window(window *wnd);
void os_open_window(window *wnd);
void os_resize_window(window *wnd, uint32 width, uint32 height);
void os_update_window_size(window *wnd);
vector<int32, 2> os_window_content_position(window *wnd);
vector<uint32, 2> os_window_content_size(window *wnd);
void os_message_loop();
void os_window_render_buffer(window *wnd, void **bits);
void os_render_window(window *wnd);
bool os_load_glyph(glyph_data *data);
int64 os_current_timestamp();
void os_copy_text_to_clipboard(string &text);
void os_copy_text_from_clipboard(string *text);
void os_update_internal_timer();
void os_update_windows();
bool os_filename_exists(string &filename);
void os_open_file(file *f);
void os_close_file(file *f);
void os_resize_file(uint64 size, file *f);
uint64 os_read_file(file *f, uint64 size, void *addr);
uint64 os_write_file(file *f, void *addr, uint64 size);
bool os_delete_file(string &filename);
void os_regiser_web_server(network_server *ws);
void os_unregister_web_server(network_server *ws);
