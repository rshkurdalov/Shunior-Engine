#include "array.h"
#include "tests.h"
#include "vector.h"
#include "matrix.h"
#include "frame_templates.h"
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
	//test_common();
	//test_real();
	//test_linear_algebra();

	flow_layout fl, fl1;
	text_field tf, tf1, tf2, tf3, tf4, tf5, tf6;

	// fl

	fl.fm.width_desc = 100uiabs;
	fl.fm.height_desc = 100uiabs;

	/*string font, str;
	font << U"cambriai";
	str << U"ab\ncde";
	char32 ch = U'a';
	for(uint64 i = 0; i < 10000; i++)
	{
		if(ch == U'z')
		{
			glyph gl;
			gl.code = U' ';
			gl.size = 16.0r;
			tf.tl.glyphs.push(gl);
			ch = U'a';
		}
		glyph gl;
		gl.code = ch;
		gl.size = 16.0r;
		tf.tl.glyphs.push(gl);
		ch++;
	}*/
	string text;
	text << U"text 1";
	tf.insert(text);
	tf.fm.width_desc = 0.2uirel;
	tf.fm.height_desc = 0.2uirel;
	tf.editable = true;
	tf.fm.focusable = true;
	fl.frames.push(flow_layout_frame(&tf.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 2";
	tf1.insert(text);
	tf1.fm.width_desc = 100uiauto;
	tf1.fm.height_desc = 100uiauto;
	tf1.fm.min_width = 100;
	tf1.fm.min_height = 40;
	tf1.editable = true;
	tf1.fm.focusable = true;
	tf1.tl.halign = horizontal_align::center;
	tf1.tl.valign = vertical_align::center;
	fl.frames.push(flow_layout_frame(&tf1.fm, horizontal_align::left, vertical_align::center, false));

	text.clear();
	text << U"text 3";
	tf2.insert(text);
	tf2.fm.width_desc = 100uiauto;
	tf2.fm.height_desc = 100uiauto;
	tf2.fm.min_width = 100;
	tf2.fm.min_height = 30;
	tf2.editable = true;
	tf2.fm.focusable = true;
	tf2.tl.halign = horizontal_align::right;
	tf2.tl.valign = vertical_align::bottom;
	fl.frames.push(flow_layout_frame(&tf2.fm, horizontal_align::right, vertical_align::top, true));

	text.clear();
	text << U"text 4";
	tf3.insert(text);
	tf3.fm.width_desc = 100uiauto;
	tf3.fm.height_desc = 100uiauto;
	tf3.fm.min_width = 100;
	tf3.fm.min_height = 30;
	tf3.editable = true;
	tf3.fm.focusable = true;
	fl.frames.push(flow_layout_frame(&tf3.fm, horizontal_align::center, vertical_align::top, false));

	// fl1

	fl1.fm.width_desc = 1.0uirel;
	fl1.fm.height_desc = 100uiauto;

	text.clear();
	text << U"text 5";
	tf4.insert(text);
	tf4.fm.width_desc = 100uiauto;
	tf4.fm.height_desc = 1.0uirel;
	tf4.fm.min_width = 100;
	tf4.fm.min_height = 30;
	tf4.editable = true;
	tf4.fm.focusable = true;
	fl1.frames.push(flow_layout_frame(&tf4.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 6";
	tf5.insert(text);
	tf5.fm.width_desc = 100uiabs;
	tf5.fm.height_desc = 1.0uirel;
	tf5.fm.min_width = 100;
	tf5.fm.min_height = 30;
	tf5.editable = true;
	tf5.fm.focusable = true;
	fl1.frames.push(flow_layout_frame(&tf5.fm, horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 7";
	tf6.insert(text);
	tf6.fm.width_desc = 100uiabs;
	tf6.fm.height_desc = 0.5uirel;
	tf6.fm.min_width = 100;
	tf6.fm.min_height = 30;
	tf6.editable = true;
	tf6.fm.focusable = true;
	fl1.frames.push(flow_layout_frame(&tf6.fm, horizontal_align::left, vertical_align::center, false));

	fl.frames.push(flow_layout_frame(&fl1.fm, horizontal_align::left, vertical_align::top, false));

	window wnd;
	wnd.layout = &fl.fm;
	wnd.open();
	wnd.update();
	wnd.message_loop();

	return 0;
}
