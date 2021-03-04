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
	real size;
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
	real width;
	real height;
	horizontal_align halign;
	vertical_align valign;
	bool multiline;

	text_layout();
	void insert_text(uint64 idx, string &str, string &font, real font_size);
	void update();
	vector<real, 2> content_size();
	void hit_test_position(uint64 idx, vector<real, 2> *point, real *line_height);
	void hit_test_point(vector<real, 2> point, uint64 *idx);
	void render(
		vector<real, 2> point,
		void (*render_glyph_callback)(
			glyph &gl,
			vector<real, 2> point,
			real baseline,
			real line_height,
			uint64 idx,
			void *data,
			bitmap *bmp),
		void *data,
		bitmap *bmp);
};

void text_layout_render_glyph_def(
	glyph &gl,
	vector<real, 2> point,
	real baseline,
	real line_height,
	uint64 idx,
	void *data,
	bitmap *bmp);
