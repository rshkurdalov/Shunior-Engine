#include "tests.h"
#include "string.h"
#include "parser.h"
#include "vector.h"
#include "matrix.h"
#include "array.h"
#include "frame.h"
#include "frame_templates.h"
#include "set.h"
#include "file.h"
#include "fileset.h"
#include "network.h"
#include "scene_frame.h"
#include "structured_file.h"

void test_array()
{
	array<uint32> arr;
	arr.push(1);
	arr.push(2);
	arr.push(3);
	arr.push(4);
	arr.push(5);
	arr.push(6);
	arr.push(7);
	uint32 extArr[3] = {8, 9, 10};
	arr.insert_range(1, extArr, extArr + 2);
	arr.pop();
	arr.remove(1);
	arr.remove_range(1, 5);
	arr.clear();
	arr.push(7);
	arr.reset();
	arr.insert(0, uint32(8));
	arr.insert_default(0, 4);
	arr.clear();

	uint64 idx;
	arr.binary_insert(5);
	idx = arr.lower_bound(key<uint32>(1));
	idx = arr.lower_bound(key<uint32>(5));
	idx = arr.lower_bound(key<uint32>(6));
	arr.binary_insert(3);
	idx = arr.lower_bound(key<uint32>(1));
	idx = arr.lower_bound(key<uint32>(3));
	idx = arr.lower_bound(key<uint32>(5));
	idx = arr.lower_bound(key<uint32>(6));
	arr.binary_insert(3);
	arr.binary_insert(3);
	arr.binary_remove(3);
	arr.binary_remove(4);
	idx = arr.binary_search(5);
	idx = arr.binary_search(4);
}

void test_string()
{
	string str, str1;
	str << U"text";
	str << U"text";
	str << U"text";
	str.clear();
	str << U"abc" << -2 << U'+' << int8(42) << U'=' << uint16(40) << U"  r = " << 40.0r;
	int32 v1;
	string_stream(U"-50", 3) >> v1;
	uint32 v2;
	string_stream(U"500", 3) >> v2;
	real v3;
	string_stream(U"-50.689", 7) >> v3;
	string_stream(U"50.08", 5) >> v3;

	char32 str2[] = U"15 89.6 -8 - ab -9. 874yt-14.0";
	string_stream p(str2, array_size(str2) - 1);
	p >> v1;
	p >> v3;
	p >> v1;
	p >> v3;
	p >> v3;
	p >> v3;
	p.position = 0;
	str.clear();
	p >> str;
	str << U' ';
	p >> str;
}

void test_vector()
{
	vector<int32, 2> v, t, r;
	v.x = 0;
	v.y = 1;
	t.x = 5;
	t.y = 11;
	r = v + t; // x = 5, y = 12
	r += t; // x = 10, y = 23
	r -= vector<int32, 2>(4, 3); // x = 6, y = 20
	r = -r; // x = -6, y = -20
	r = r - vector<int32, 2>(-6, -20); // x = 0, y = 0
	r += t; // x = 5, y = 11
	r -= t; // x = 0, y = 0
	r += 2 * t; // x = 10, y = 22
	r += t * 3; // x = 25, y = 55
	r *= 2; // x = 50, y = 110
	r /= 2; // x = 25, y = 55
	r -= t / 2; // x = 23, y = 50
}

void test_matrix()
{
	matrix<real, 2, 3> mat = 1.0r, mat1;
	mat = 0.0r;
	mat1 = 1;
	mat += mat1;
	mat += mat1 + mat1;
	mat.m[1][1] -= 0.5r;
	mat.m[1][2] -= 1.0r;
	mat -= mat1;
	mat = 3.0r * mat1 - mat;
	mat *= 2.0r;
	mat /= 2.0r;
	mat = mat1 / 2.0r + mat;
	real mtValues[3][2] =
	{
		{2.0r, 6.0r},
		{3.0r, 1.0r},
		{4.0r, 1.5r}
	};
	matrix<real, 3, 2> mt(mtValues);
	vector<real, 3> vt(2.5r, 5.0r, 50.0r);
	matrix<real, 1, 2> a = vt * mt;
	matrix<real, 2, 1> b;
	b.m[0][0] = 1.0r;
	b.m[1][0] = 5.0r;
	matrix<real, 2, 3> c = b * vt;
	matrix<real, 3, 3> d = mt * c;
	real eValues[5][5] =
	{
		{8, 9, 8, 5, 1},
		{74, 8, 5, 35, 12},
		{54, 12, 32, 9, 10},
		{12, 54, 87, 45, 24},
		{70, 4, 7, 8, 1}
	};
	matrix<real, 5, 5> e(eValues);
	real det = determinant(e); // det = 2996358
	matrix<real, 2, 3> mtTransposed = transpose_matrix(mt);
	invert_matrix(&e);

	real tr_values[3][3] =
	{
		{5.5r, 10.0r, 0.0r},
		{8.0r, 9.0r, 0.0r},
		{50.0r, 4.0r, 1.0r}
	};
	matrix<real, 3, 3> tr(tr_values);
	matrix<real, 3, 3> itr = tr;
	invert_matrix(&itr);
	matrix<real, 1, 3> mul = vector<real, 3>(1.0r, 1.0r, 1.0r) * tr;
	vector<real, 2> p = vector<real, 2>(mul.m[0][0], mul.m[0][1]);
	mul = vector<real, 3>(p.x, p.y, 1.0r) * itr;
	p = vector<real, 2>(mul.m[0][0], mul.m[0][1]);
}

void test_real()
{
	real a = 12.24r, b = 0.5r, c = a / b; // c = 24.48
	real d = 2.0r, e = 3.0r;
	real f = d / e; // f = 0.666666666
	real g = 1.0r, h = -1.0r;
	real l = g / h; // l = -1
	l = h / h; // l = 1
	l = e / h; // l = -3
	real m = 0.0r;
	l = m / e; // l = 0
	l = e / m;
	l = m / m;
	l = f / m;
	l = m / h;
	l = b - g; // l = -0.5
	l = e - a; // l = -9.24
	l = root(4.0r, 2); // l = 2
	l = root(80.0r, 2); // l = 8.944271909
	l = root(100000000.0r, 8); // l = 10
}

void test_linear_algebra()
{
	real r = angle_normal(0.111408460r).y; // r = 0.644217687
	r = vector_angle(angle_normal(0.111408460r));
	r = angle_normal(0.5r).y; // r = 0
	r = vector_angle(angle_normal(0.5r));
	r = angle_normal(1.0r).y; // r = 0
	r = angle_normal(-1.0r).y; // r = 0
	r = vector_angle(angle_normal(-1.0r));
	r = angle_normal(1.5r).y; // r = 0
	r = vector_angle(angle_normal(1.5r));
	r = angle_normal(0.0r).y; // r = 0
	r = angle_normal(0.25r).y; // r = 1
	r = angle_normal(-0.25r).y; // r = -1

	r = angle_normal(0.0r).x; // r = 1
	r = angle_normal(0.111408460r).x; // r = 0.764842187
	r = angle_normal(0.5r).x; // r = -1

	r = root(9.0r, 2);
	r = root(8.0r, 3);
	r = root(800000.0r, 2);

	vector<real, 2> v = vector<real, 2>(96.107510240r - 95.080509997r, 2.660436428r - 2.380373700r);
	r = vector_dot(v, v);
	r = root(r, 2);
}

void test_ui()
{
	flow_layout *fl = new flow_layout(),
		*fl1 = new flow_layout();
	text_field *tf = new text_field(),
		*tf1 = new text_field(),
		*tf2 = new text_field(),
		*tf3 = new text_field(),
		*tf4 = new text_field(),
		*tf5 = new text_field(),
		*tf6 = new text_field(),
		*tf7 = new text_field(),
		*tf8 = new text_field();
	push_button *pb = new push_button();

	// fl

	fl->fm.width_desc = 100uiabs;
	fl->fm.height_desc = 100uiabs;

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
			tf->data.tl.glyphs.push(gl);
			ch = U'a';
		}
		glyph gl;
		gl.code = ch;
		gl.size = 16;
		tf->data.tl.glyphs.push(gl);
		ch++;
	}
	string text;
	text << U"text 1";
	tf->data.insert(text);
	tf->fm.width_desc = 0.8uirel;
	tf->fm.height_desc = 0.8uirel;
	tf->data.editable = true;
	tf->fm.focusable = true;
	fl->data.frames.push(flow_layout_frame(handleable<frame>(tf, &tf->fm), horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 2";
	tf1->data.insert(text);
	tf1->fm.width_desc = 100uiauto;
	tf1->fm.height_desc = 100uiauto;
	tf1->fm.min_width = 100;
	tf1->fm.min_height = 40;
	tf1->data.editable = true;
	tf1->fm.focusable = true;
	tf1->data.tl.halign = horizontal_align::center;
	tf1->data.tl.valign = vertical_align::center;
	fl->data.frames.push(flow_layout_frame(handleable<frame>(tf1, &tf1->fm), horizontal_align::left, vertical_align::center, false));

	text.clear();
	text << U"text 3";
	tf2->data.insert(text);
	tf2->fm.width_desc = 100uiauto;
	tf2->fm.height_desc = 100uiauto;
	//tf2->fm.min_width = 100;
	tf2->fm.min_height = 30;
	tf2->data.editable = true;
	tf2->fm.focusable = true;
	tf2->data.tl.halign = horizontal_align::right;
	tf2->data.tl.valign = vertical_align::bottom;
	fl->data.frames.push(flow_layout_frame(handleable<frame>(tf2, &tf2->fm), horizontal_align::right, vertical_align::top, true));

	text.clear();
	text << U"text 4";
	tf3->data.insert(text);
	tf3->fm.width_desc = 100uiauto;
	tf3->fm.height_desc = 100uiauto;
	tf3->fm.min_width = 100;
	tf3->fm.min_height = 30;
	tf3->data.editable = true;
	tf3->fm.focusable = true;
	fl->data.frames.push(flow_layout_frame(handleable<frame>(tf3, &tf3->fm), horizontal_align::center, vertical_align::top, false));

	// fl1

	fl1->fm.width_desc = 1.0uirel;
	fl1->fm.height_desc = 100uiauto;

	/*text.clear();
	text << U"text 5";
	tf4->data.insert(text);
	tf4->fm.width_desc = 100uiauto;
	tf4->fm.height_desc = 1.0uirel;
	tf4->fm.min_width = 100;
	tf4->fm.min_height = 30;
	tf4->data.editable = true;
	tf4->fm.focusable = true;
	fl1->data.frames.push(flow_layout_frame(handleable<frame>(tf4, &tf4->fm), horizontal_align::left, vertical_align::top, false));*/
	text.clear();
	text << U"text 5";
	pb->tf_data.insert(text);
	pb->fm.width_desc = 100uiauto;
	pb->fm.height_desc = 1.0uirel;
	pb->fm.min_width = 100;
	pb->fm.min_height = 30;
	fl1->data.frames.push(flow_layout_frame(handleable<frame>(pb, &pb->fm), horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 6";
	tf5->data.insert(text);
	tf5->fm.width_desc = 100uiabs;
	tf5->fm.height_desc = 1.0uirel;
	//tf5->fm.min_width = 100;
	tf5->fm.min_height = 30;
	tf5->data.editable = true;
	tf5->fm.focusable = true;
	fl1->data.frames.push(flow_layout_frame(handleable<frame>(tf5, &tf5->fm), horizontal_align::left, vertical_align::top, false));

	text.clear();
	text << U"text 7";
	tf6->data.insert(text);
	tf6->fm.width_desc = 100uiabs;
	tf6->fm.height_desc = 0.5uirel;
	tf6->fm.min_width = 100;
	tf6->fm.min_height = 30;
	tf6->data.editable = true;
	tf6->fm.focusable = true;
	fl1->data.frames.push(flow_layout_frame(handleable<frame>(tf6, &tf6->fm), horizontal_align::left, vertical_align::center, false));

	fl->data.frames.push(flow_layout_frame(handleable<frame>(fl1, &fl1->fm), horizontal_align::left, vertical_align::top, false));

	fl->fm.margin_left = 0uiabs;
	fl->fm.margin_bottom = 0uiabs;
	fl->fm.margin_right = 0uiabs;
	fl->fm.margin_top = 0uiabs;
	fl->model.background_color = alpha_color(235, 235, 235, 0);
	fl->data.direction = flow_axis::y;

	/*Grid layout test*/

	grid_layout *gl = new grid_layout();
	gl->fm.margin_left = 0uiabs;
	gl->fm.margin_bottom = 0uiabs;
	gl->fm.margin_right = 0uiabs;
	gl->fm.margin_top = 0uiabs;
	gl->model.background_color = alpha_color(235, 235, 235, 0);

	gl->data.insert_row(0, 100uiabs);
	gl->data.insert_row(1, 100uiauto);
	gl->data.growth_row = 1;
	gl->data.insert_row(2, 0.2uirel);
	
	gl->data.insert_column(0, 100uiabs);
	gl->data.insert_column(1, 100uiauto);
	gl->data.growth_column = 1;
	gl->data.insert_column(2, 50uiauto);

	tf->fm.width_desc = 0.5uirel;
	tf->fm.height_desc = 0.5uirel;
	gl->data.frames.at(0, 0) = grid_layout_frame(handleable<frame>(tf, &tf->fm), horizontal_align::center, vertical_align::center);

	tf1->fm.width_desc = 1.0uirel;
	tf1->fm.height_desc = 1.0uirel;
	gl->data.frames.at(0, 1) = grid_layout_frame(handleable<frame>(tf1, &tf1->fm), horizontal_align::center, vertical_align::center);

	tf2->fm.width_desc = 1.0uirel;
	tf2->fm.height_desc = 1.0uirel;
	gl->data.frames.at(0, 2) = grid_layout_frame(handleable<frame>(tf2, &tf2->fm), horizontal_align::center, vertical_align::center);

	tf3->fm.width_desc = 1.0uirel;
	tf3->fm.height_desc = 1.0uirel;
	gl->data.frames.at(1, 0) = grid_layout_frame(handleable<frame>(tf3, &tf3->fm), horizontal_align::center, vertical_align::center);

	tf4->fm.width_desc = 1.0uirel;
	tf4->fm.height_desc = 1.0uirel;
	gl->data.frames.at(1, 1) = grid_layout_frame(handleable<frame>(tf4, &tf4->fm), horizontal_align::center, vertical_align::center);

	tf5->fm.width_desc = 1.0uirel;
	tf5->fm.height_desc = 1.0uirel;
	gl->data.frames.at(1, 2) = grid_layout_frame(handleable<frame>(tf5, &tf5->fm), horizontal_align::center, vertical_align::center);

	tf6->fm.width_desc = 1.0uirel;
	tf6->fm.height_desc = 1.0uirel;
	gl->data.frames.at(2, 0) = grid_layout_frame(handleable<frame>(tf6, &tf6->fm), horizontal_align::center, vertical_align::center);

	/*tf7->fm.width_desc = 1.0uirel;
	tf7->fm.height_desc = 1.0uirel;
	gl->data.frames.at(2, 1) = grid_layout_frame(handleable<frame>(tf7, &tf7->fm), horizontal_align::center, vertical_align::center);

	tf8->fm.width_desc = 1.0uirel;
	tf8->fm.height_desc = 1.0uirel;
	gl->data.frames.at(2, 2) = grid_layout_frame(handleable<frame>(tf8, &tf8->fm), horizontal_align::center, vertical_align::center);*/

	window *wnd = new window();
	scene_frame *sf = new scene_frame();
	sf->sc = new scene();
	sf->sc->set_window(wnd->handler);
	sf->layout = handleable<frame>(gl, &gl->fm);
	sf->fm.focusable = true;
	gl->fm.return_mouse_move = true;
	gl->fm.return_mouse_click = true;
	wnd->layout = handleable<frame>(sf, &sf->fm);
	wnd->open();
	wnd->update();
}

void test_set()
{
	set<int32> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(3);
	s.remove(key<int32>(3));

	s.insert(3);
	s.remove(2);

	s.insert(2);
	s.clear();

	s.insert(2);
	s.insert(0);
	s.insert(1);

	s.clear();
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(0);
	s.remove(2);
}

void test_file()
{
	file f;
	f.read_access = true;
	f.write_access = true;
	f.filename << U"C:\\Users\\rshkurdalov\\Download";
	bool b = f.exists();
	f.filename << U's';
	b = f.exists();
	f.filename << U"\\file1";
	f.open();
	f.resize(2000);
	uint64 n = f.write(f.filename.addr, f.filename.size * sizeof(char32));
	f.position = 0;
	string str;
	str.insert_default(0, f.filename.size);
	n = f.read(f.filename.size * sizeof(char32), str.addr);
	f.close();
}

void test_fileset()
{
	auto dump = [](fileset<int32> &fs, string *str) -> void
	{
		str->clear();
		*str << U"elements:\n";
		for(fileset_iterator<int32> iter = fs.begin(); iter.idx != -1; iter = fs.next(iter))
		{
			*str << U"[" << iter.node.value << U' ' << iter.node.upper << U' ' << iter.node.left << U' ' << iter.node.right << U"]\n";
		}
		*str << U"free slots: ";
		fileset_iterator<int32> iter;
		iter.idx = fs.free_idx;
		while(iter.idx != -1)
		{
			iter = fs.iterator(iter.idx);
			*str << iter.idx << U' ';
			iter.idx = iter.node.left;
		}
	};
	fileset<int32> s;
	string filename;
	filename << U"C:\\Users\\rshkurdalov\\Downloads\\file2.fs";
	s.create(filename);
	string str;
	dump(s, &str);
	s.insert(1);
	dump(s, &str);
	s.insert(2);
	dump(s, &str);
	s.insert(3);
	s.insert(3);
	dump(s, &str);
	s.remove(key<int32>(3));
	dump(s, &str);

	s.insert(3);
	s.remove(2);
	dump(s, &str);

	s.insert(2);
	s.clear();
	dump(s, &str);

	s.insert(2);
	s.insert(0);
	s.insert(1);
	dump(s, &str);

	s.clear();
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(0);
	s.remove(2);
	dump(s, &str);

	s.close();
}

array<byte> request_output;
void test_web_server()
{
	/*network_server *ws = new network_server();
	ws->ip = ip_address(127, 0, 0, 1);
	ws->port = 8000;
	auto process_request = [](byte *input, uint64 input_size, byte **output, uint64 *output_size) -> bool
	{
		http_request request;
		if(!request.setup(string_line<char8>((char8 *)(input), input_size))) return false;
		http_response response;
		response.version << U"HTTP/1.1";
		if(compare_string_lines<char32>(string_line<char32>(request.query), string_line<char32>(U"/")) == compare_result::equal)
		{
			response.status_code << U"200";
			response.status_message << U"OK";
			response.content_type << U"text/html";
			file f;
			f.read_access = true;
			f.write_access = true;
			f.filename << U"C:\\Users\\rshkurdalov\\source\\repos\\WebApp\\index.html";
			f.open();
			response.data.insert_default(0, f.size);
			f.position = 0;
			f.read(f.size, response.data.addr);
			
		}
		else if(compare_string_lines<char32>(string_line<char32>(request.query), string_line<char32>(U"/style.css")) == compare_result::equal)
		{
			response.status_code << U"200";
			response.status_message << U"OK";
			response.content_type << U"text/css";
			file f;
			f.read_access = true;
			f.write_access = true;
			f.filename << U"C:\\Users\\rshkurdalov\\source\\repos\\WebApp\\style.css";
			f.open();
			response.data.insert_default(0, f.size);
			f.position = 0;
			f.read(f.size, response.data.addr);
		}
		else
		{
			response.status_code << U"404";
			response.status_message << U"Not Found";
		}
		request_output.clear();
		response.compose(&request_output);
		*output = request_output.addr;
		*output_size = request_output.size;
		return true;
	};
	ws->process_request = process_request;
	ws->run();*/
}

void test_structured_file()
{
	string filename(U"C:\\Users\\Shunior\\Desktop\\sf.txt");
	structured_file sf;

	sf.root.name << U"root";
	sf.root.type = snode_type::array;

	sf.root.elements.push_default();
	sf.root.elements[0].name <<= U"attr1";
	sf.root.elements[0].type = snode_type::string;
	sf.root.elements[0].value <<= U"value1";

	sf.root.elements.push_default();
	sf.root.elements[1].name <<= U"attr2";
	sf.root.elements[1].type = snode_type::array;
	sf.root.elements[1].elements.push_default();
	sf.root.elements[1].elements[0].name <<= U"attr21";
	sf.root.elements[1].elements[0].type = snode_type::string;
	sf.root.elements[1].elements[0].value <<= U"value21";
	sf.root.elements[1].elements.push_default();
	sf.root.elements[1].elements[1].name <<= U"attr22";
	sf.root.elements[1].elements[1].type = snode_type::string;
	sf.root.elements[1].elements[1].value <<= U"value22";

	sf.root.elements.push_default();
	sf.root.elements[2].name <<= U"attr3";
	sf.root.elements[2].type = snode_type::string;
	sf.root.elements[2].value <<= U"value3";

	sf.save(filename);
	sf.clear();
	sf.load(filename);
}
