#pragma once
#include "ui_types.h"
#include "real.h"
#include "frame.h"
#include "geometry.h"

ui_size operator""uiabs(uint64 value);
ui_size operator""uirel(const char8 *str);
ui_size operator""uiauto(uint64 value);
uint32 resolve_ui_size(ui_size size, uint32 related_size);
rectangle<int32> frame_viewport(frame *fm);
rectangle<int32> frame_content_viewport(frame *fm);
vector<uint32, 2> content_size_to_frame_size(frame *fm, uint32 content_width, uint32 content_height);
vector<uint32, 2> evaluate_frame_size(frame *fm, uint32 viewport_width, uint32 viewport_height);
void frame_global_subframes(frame *fm, array<frame *> *frames);
bool frame_hit_test_def(frame *fm, vector<int32, 2> point);
void frame_subframes_def(frame *fm, array<frame *> *frames); 
vector<uint32, 2> frame_content_size_def(frame *fm, uint32 viewport_width, uint32 viewport_height);
void frame_render_def(frame *fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp);
void frame_mouse_click_def(frame *fm);
void frame_mouse_release_def(frame *fm);
void frame_mouse_move_def(frame *fm);
void frame_start_hover_def(frame *fm);
void frame_end_hover_def(frame *fm);
void frame_focus_receive_def(frame *fm);
void frame_focus_loss_def(frame *fm);
void frame_mouse_wheel_rotate_def(frame *fm);
void frame_key_press_def(frame *fm);
void frame_key_release_def(frame *fm);
void frame_char_input_def(frame *fm);
