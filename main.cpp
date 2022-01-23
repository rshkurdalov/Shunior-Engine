#include "array.h"
#include "tests.h"
#include "vector.h"
#include "matrix.h"
#include "frame_templates.h"
#include "os_api.h"
#include <Windows.h>

int __stdcall wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow)
{
	//test_array();
	//test_string();
	//test_vector();
	//test_matrix();
	//test_real();
	//test_linear_algebra();
	//test_set();
	//test_file();

	flow_layout fl, fl1;
	text_field tf, tf1, tf2, tf3, tf4, tf5, tf6;
	push_button pb;

	// fl

	fl.fm.width_desc = 100uiabs;
	fl.fm.height_desc = 100uiabs;

	string font, str;
	font << U"cambriai";
	str << U"ab\ncde";
	char32 ch = U'a';
	for(uint64 i = 0; i < 10000; i++)
	{
		if(ch == U'z')
		{
			glyph gl;
			gl.code = U' ';
			gl.size = 16;
			tf.data.tl.glyphs.push(gl);
			ch = U'a';
		}
		glyph gl;
		gl.code = ch;
		gl.size = 16;
		tf.data.tl.glyphs.push(gl);
		ch++;
	}
	string text;
	text << U"text 1";
	tf.data.insert(text);
	tf.fm.width_desc = 0.8uirel;
	tf.fm.height_desc = 0.8uirel;
	tf.data.editable = true;
	tf.fm.focusable = true;
	fl.data.frames.push(flow_layout_frame(&tf.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 2";
	tf1.data.insert(text);
	tf1.fm.width_desc = 100uiauto;
	tf1.fm.height_desc = 100uiauto;
	tf1.fm.min_width = 100;
	tf1.fm.min_height = 40;
	tf1.data.editable = true;
	tf1.fm.focusable = true;
	tf1.data.tl.halign = horizontal_align::center;
	tf1.data.tl.valign = vertical_align::center;
	fl.data.frames.push(flow_layout_frame(&tf1.fm, horizontal_align::left, vertical_align::center, false));

	text.clear();
	text << U"text 3";
	tf2.data.insert(text);
	tf2.fm.width_desc = 100uiauto;
	tf2.fm.height_desc = 100uiauto;
	tf2.fm.min_width = 100;
	tf2.fm.min_height = 30;
	tf2.data.editable = true;
	tf2.fm.focusable = true;
	tf2.data.tl.halign = horizontal_align::right;
	tf2.data.tl.valign = vertical_align::bottom;
	fl.data.frames.push(flow_layout_frame(&tf2.fm, horizontal_align::right, vertical_align::top, true));

	text.clear();
	text << U"text 4";
	tf3.data.insert(text);
	tf3.fm.width_desc = 100uiauto;
	tf3.fm.height_desc = 100uiauto;
	tf3.fm.min_width = 100;
	tf3.fm.min_height = 30;
	tf3.data.editable = true;
	tf3.fm.focusable = true;
	fl.data.frames.push(flow_layout_frame(&tf3.fm, horizontal_align::center, vertical_align::top, false));

	// fl1

	fl1.fm.width_desc = 1.0uirel;
	fl1.fm.height_desc = 100uiauto;

	/*text.clear();
	text << U"text 5";
	tf4.data.insert(text);
	tf4.fm.width_desc = 100uiauto;
	tf4.fm.height_desc = 1.0uirel;
	tf4.fm.min_width = 100;
	tf4.fm.min_height = 30;
	tf4.data.editable = true;
	tf4.fm.focusable = true;
	fl1.data.frames.push(flow_layout_frame(&tf4.fm, horizontal_align::left, vertical_align::top, false));*/
	text.clear();
	text << U"text 5";
	pb.tf_data.insert(text);
	pb.fm.width_desc = 100uiauto;
	pb.fm.height_desc = 1.0uirel;
	pb.fm.min_width = 100;
	pb.fm.min_height = 30;
	fl1.data.frames.push(flow_layout_frame(&pb.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 6";
	tf5.data.insert(text);
	tf5.fm.width_desc = 100uiabs;
	tf5.fm.height_desc = 1.0uirel;
	tf5.fm.min_width = 100;
	tf5.fm.min_height = 30;
	tf5.data.editable = true;
	tf5.fm.focusable = true;
	fl1.data.frames.push(flow_layout_frame(&tf5.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 7";
	tf6.data.insert(text);
	tf6.fm.width_desc = 100uiabs;
	tf6.fm.height_desc = 0.5uirel;
	tf6.fm.min_width = 100;
	tf6.fm.min_height = 30;
	tf6.data.editable = true;
	tf6.fm.focusable = true;
	fl1.data.frames.push(flow_layout_frame(&tf6.fm, horizontal_align::left, vertical_align::center, false));

	fl.data.frames.push(flow_layout_frame(&fl1.fm, horizontal_align::left, vertical_align::top, false));

	fl.fm.margin_left = 0uiabs;
	fl.fm.margin_bottom = 0uiabs;
	fl.fm.margin_right = 0uiabs;
	fl.fm.margin_top = 0uiabs;
	fl.model.background_color = alpha_color(235, 235, 235, 255);

	window wnd;
	wnd.layout = &fl.fm;
	wnd.open();
	wnd.update();

	os_message_loop();

	return 0;
}
