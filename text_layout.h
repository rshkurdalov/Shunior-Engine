#pragma once
#include "string.h"
#include "geometry.h"
#include "graphics.h"
#include "ui_types.h"

struct glyph_data
{
	char32 code;
	geometry_path path;
	vector<int32, 2> advance;
	bitmap bmp;
	vector<real, 2> bmp_offset;
	string font_name;
	uint32 size;
	bool italic;
	uint32 weight;
	uint32 ascent;
	uint32 descent;
	uint32 internal_leading;
	int32 underline_offset;
	uint32 underline_size;
	int32 strikethrough_offset;
	uint32 strikethrough_size;
};

struct glyph
{
	char32 code;
	string font_name;
	uint32 size;
	bool italic;
	uint32 weight;
	bool underlined;
	bool strikedthrough;
	glyph_data *data;

	glyph() : italic(false), weight(400),
		underlined(false), strikedthrough(false) {}
};

struct text_layout
{
	array<glyph> glyphs;
	uint32 width;
	uint32 height;
	horizontal_align halign;
	vertical_align valign;
	bool multiline;

	text_layout();
	void insert_text(uint64 idx, string &str, string &font, uint32 font_size);
	void update();
	vector<int32, 2> content_size();
	void hit_test_position(uint64 idx, vector<int32, 2> *point, int32 *line_height);
	void hit_test_point(vector<int32, 2> point, uint64 *idx);
	void render(
		vector<int32, 2> point,
		void (*render_glyph_callback)(
			glyph &gl,
			vector<int32, 2> point,
			int32 baseline,
			int32 line_height,
			uint64 idx,
			void *data,
			bitmap *bmp),
		void *data,
		bitmap *bmp);
};

void text_layout_render_glyph_def(
	glyph &gl,
	vector<int32, 2> point,
	int32 baseline,
	int32 line_height,
	uint64 idx,
	void *data,
	bitmap *bmp);
