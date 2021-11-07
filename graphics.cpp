#include "graphics.h"

bitmap::bitmap()
{
	data = nullptr;
}

bitmap::~bitmap()
{
	if(data != nullptr) delete[] data;
}


void bitmap::resize(uint32 width_value, uint32 height_value)
{
	width = width_value;
	height = height_value;
	if(data != nullptr)
	{
		delete[] data;
		data = nullptr;
	}
	if(width != 0 && height != 0)
		data = new alpha_color[width * height];
}

bitmap_processor::bitmap_processor()
{
	rasterization = rasterization_mode::fill;
	line_width = 1.0r;
	set_identity_matrix(&transform);
	opacity = 1.0r;
	color_interpolation = color_interpolation_mode::flat;
	set_solid_color_brush(alpha_color(0, 0, 0, 255));
}

void bitmap_processor::push_scissor(rectangle<int32> rect)
{
	if(scissor_stack.size == 0)
		scissor_stack.push(rect);
	else
	{
		rectangle<int32> target_rect;
		target_rect.position.x = max(
			rect.position.x,
			scissor_stack.addr[scissor_stack.size - 1].position.x);
		target_rect.position.y = max(
			rect.position.y,
			scissor_stack.addr[scissor_stack.size - 1].position.y);
		target_rect.extent.x = min(
			rect.position.x + rect.extent.x,
			scissor_stack.addr[scissor_stack.size - 1].position.x
			+ scissor_stack.addr[scissor_stack.size - 1].extent.x)
			- target_rect.position.x;
		target_rect.extent.y = min(
			rect.position.y + rect.extent.y,
			scissor_stack.addr[scissor_stack.size - 1].position.y
			+ scissor_stack.addr[scissor_stack.size - 1].extent.y)
			- target_rect.position.y;
		scissor_stack.push(target_rect);
	}
}

void bitmap_processor::pop_scissor()
{
	scissor_stack.pop();
}

void bitmap_processor::set_solid_color_brush(alpha_color color_value)
{
	brush = brush_type::solid;
	color = color_value;
}

void bitmap_processor::set_linear_gradient_brush(
	gradient_stop *gradient_collection,
	uint32 size,
	vector<real, 2> begin,
	vector<real, 2> end)
{
	brush = brush_type::linear_gradient;
	gradients.reset();
	gradients.insert_range(0, gradient_collection, gradient_collection + size);
	v1 = begin;
	v2 = end;
}

void bitmap_processor::set_radial_gradient_brush(
	gradient_stop *gradient_collection,
	uint32 size,
	vector<real, 2> center,
	vector<real, 2> offset,
	real rx_value,
	real ry_value)
{
	brush = brush_type::radial_gradient;
	gradients.reset();
	gradients.insert_range(0, gradient_collection, gradient_collection + size);
	v1 = center;
	v2 = offset;
	rx = rx_value;
	ry = ry_value;
}

void bitmap_processor::set_bitmap_brush(bitmap *source_bitmap, matrix<real, 3, 3> &bitmap_transform_matrix)
{
	brush = brush_type::bitmap;
	bitmap_addr = source_bitmap;
	bitmap_transform = bitmap_transform_matrix;
	reverse_transform = bitmap_transform;
	invert_matrix(&reverse_transform);
}

alpha_color bitmap_processor::point_color(uint32 x, uint32 y)
{
	if(brush == brush_type::solid)
	{
		return color;
	}
	else if(brush == brush_type::bitmap)
	{
		matrix<real, 1, 3> mp = vector<real, 3>(real(x) + 0.5r, real(y) + 0.5r, 1.0r) * reverse_transform;
		vector<real, 2> p = vector<real, 2>(mp.m[0][0], mp.m[0][1]);
		int32 bx = int32(round(p.x)), by = int32(round(p.y));
		if(bx < 0 || bx >= int32(bitmap_addr->width) || by < 0 || by >= int32(bitmap_addr->height))
			return alpha_color(0, 0, 0, 0);
		return bitmap_addr->data[(bitmap_addr->height - 1 - uint32(by)) * bitmap_addr->width + uint32(bx)];
	}
	else
	{
		if(gradients.size == 0) return alpha_color(0, 0, 0, 0);
		vector<real, 2> sample = vector<real, 2>(real(x) + 0.5r, real(y) + 0.5r);
		real grad;
		alpha_color color;
		bool hasColor = true;
		if(brush == brush_type::linear_gradient)
		{
			vector<real, 2> dir = vector_normal(v2 - v1);
			real a = vector_dot(sample - v1, dir);
			grad = vector_length(v1 + a * dir - v1) / vector_length(v2 - v1);
			if(a < 0.0r) grad = -grad;
		}
		else
		{
			real a, b, c, d, k;
			vector<real, 2> p1, p2;
			sample -= v1;
			if(abs(sample.x - v2.x) < 0.1r)
			{
				a = ry * ry * (1.0r - v2.x * v2.x / (rx * rx));
				if(a < 0.0r) hasColor = false;
				else
				{
					a = root(a, 2);
					p1.x = v2.x;
					p1.y = a;
					p2.x = v2.x;
					p2.y = -a;
					a = (sample.y - v2.y) * (p1.y - v2.y);
					b = (sample.y - v2.y) * (p2.y - v2.y);
					if(a < 0.0r && b < 0.0r) hasColor = false;
					else
					{
						if(a < 0.0r || b >= 0.0r && a < b) swap(&p1, &p2);
						grad = (sample.y - v2.y) / (p1.y - v2.y);
					}
				}
			}
			else
			{
				k = (v2.y - sample.y) / (v2.x - sample.x);
				a = k * k + ry * ry / (rx * rx);
				b = k * (sample.y - k * sample.x);
				c = (sample.y - k * sample.x) * (sample.y - k * sample.x) - ry * ry;
				d = b * b - a * c;
				if(d < 0.0r) hasColor = false;
				else
				{
					d = root(d, 2);
					p1.x = (d - b) / a;
					p1.y = (sample.y - k * sample.x) + k * p1.x;
					p2.x = -(b + d) / a;
					p2.y = (sample.y - k * sample.x) + k * p2.x;
					a = vector_dot(sample - v2, p1 - v2);
					b = vector_dot(sample - v2, p2 - v2);
					if(a < 0.0r && b < 0.0r) hasColor = false;
					else
					{
						if(a < 0.0r || b >= 0.0r && a < b) swap(&p1, &p2);
						grad = vector_length(sample - v2) / vector_length(p1 - v2);
					}
				}
			}
				
		}
		if(!hasColor) return alpha_color(0, 0, 0, 0);
		if(grad < gradients.addr[0].offset)
			color = gradients.addr[0].color;
		else if(grad >= gradients.addr[gradients.size - 1].offset)
			color = gradients.addr[gradients.size - 1].color;
		else
		{
			uint32 j = 1;
			while(grad >= gradients.addr[j].offset) j++;
			if(color_interpolation == color_interpolation_mode::flat)
				color = gradients.addr[j - 1].color;
			else
			{
				real w = (grad - gradients.addr[j - 1].offset)
					/ (gradients.addr[j].offset - gradients.addr[j - 1].offset);
				if(color_interpolation == color_interpolation_mode::smooth)
					w = w * w * (3.0r - 2.0r * w);
				color.r = uint8(round(real(gradients.addr[j - 1].color.r)) * (1.0r - w)
					+ real(gradients.addr[j].color.r) * w);
				color.g = uint8(round(real(gradients.addr[j - 1].color.g)) * (1.0r - w)
					+ real(gradients.addr[j].color.g) * w);
				color.b = uint8(round(real(gradients.addr[j - 1].color.b)) * (1.0r - w)
					+ real(gradients.addr[j].color.b) * w);
				color.a = uint8(round(real(gradients.addr[j - 1].color.a)) * (1.0r - w)
					+ real(gradients.addr[j].color.a) * w);
			}
		}
		return color;
	}
		
}

void outline_path(real width, geometry_path *path)
{
	if(path->data.size == 0) return;
	geometry_path outline_path;
	vector<real, 2> p1, p2, p3, j1, j2, j3, j4, v1, v2;
	uint64 last_move = 0;
	width *= 0.5r;
	for(uint64 i = 0; i < path->data.size; i++)
	{
		if(path->data.addr[i].type == geometry_path_unit::move)
		{
			last_move = i;
			p3 = path->data.addr[i].p1;
		}
		else
		{
			if(i == 0 || path->data.addr[i - 1].type == geometry_path_unit::move)
			{
				p3 = path->data.addr[i].p1;
				v2 = vector_normal(p3 - p2);
				v2 = width * vector<real, 2>(-v2.y, v2.x);
				j3 = p2 + v2;
				j4 = p2 - v2;
				outline_path.move(j3);
				outline_path.push_line(j4);
			}
			else
			{
				p3 = path->data.addr[i].p1;
				v2 = vector_normal(p3 - p2);
				v2 = width * vector<real, 2>(-v2.y, v2.x);
				if(intersect_lines(j1, p2 + v1, p2 + v2, p3 + v2, &j3) && vector_length(j3 - p2) < 3.0r * width
					&& intersect_lines(j2, p2 - v1, p2 - v2, p3 - v2, &j4) && vector_length(j4 - p2) < 3.0r * width)
				{
					outline_path.move(j3);
					outline_path.push_line(j1);
					outline_path.move(j2);
					outline_path.push_line(j4);
				}
				else
				{
					j3 = p2 + v1;
					outline_path.move(j3);
					outline_path.push_line(j1);
					j4 = p2 - v1;
					outline_path.move(j2);
					outline_path.push_line(j4);
					if((p3.x - p2.x) * (p2.x - p1.x) + (p3.y - p2.y) * (p2.y - p1.y) < 0.0r)
					{
						outline_path.push_line(j3);
						outline_path.move(j4);
						outline_path.push_line(j3);
						swap(&j3, &j4);
					}
				}
			}
		}
		p1 = p2;
		p2 = p3;
		v1 = v2;
		j1 = j3;
		j2 = j4;
		if(i == path->data.size - 1 || path->data.addr[i + 1].type == geometry_path_unit::move)
		{
			if(i >= last_move + 4
				&& path->data.addr[last_move].type == geometry_path_unit::move
				&& path->data.addr[i].p1 == path->data.addr[last_move].p1)
			{
				p3 = path->data.addr[last_move + 1].p1;
				v2 = vector_normal(p3 - p2);
				v2 = width * vector<real, 2>(-v2.y, v2.x);
				if(intersect_lines(j1, p2 + v1, p2 + v2, p3 + v2, &j3)
					&& intersect_lines(j2, p2 - v1, p2 - v2, p3 - v2, &j4))
				{
					outline_path.move(j3);
					outline_path.push_line(j1);
					outline_path.move(j2);
					outline_path.push_line(j4);
				}
				else
				{
					j3 = p2 + v1;
					outline_path.move(j3);
					outline_path.push_line(j1);
					j4 = p2 - v1;
					outline_path.move(j2);
					outline_path.push_line(j4);
					if((p3.x - p2.x) * (p2.x - p1.x) + (p3.y - p2.y) * (p2.y - p1.y) < 0.0r)
						swap(&j3, &j4);
				}
				outline_path.move(j4);
				outline_path.push_line(p3 - v2);
				outline_path.push_line(p3 + v2);
				outline_path.push_line(j3);

			}
			else if(path->data.addr[i].type == geometry_path_unit::line)
			{
				outline_path.move(j2);
				outline_path.push_line(p2 - v1);
				outline_path.push_line(p2 + v1);
				outline_path.push_line(j1);
			}
		}
	}
	swap(&path->data, &outline_path.data);
}

void bitmap_processor::render(geometry_path &path, bitmap *bmp)
{
	const uint64 sublines = 4;
	const real dy = 0.25r;
	int32 x1, x2, b1, b2, m, xb, yb;
	uint64 idx, j, l, l1, l2, k;
	real lx = max_real, hx = min_real,
		ly = max_real, hy = min_real,
		x, y, a, c, t, ts, tm;
	alpha_color color_value, *color_addr;
	struct range_coordinate
	{
		real coord;
		bool negative_direction;

		range_coordinate(real coord, bool negative_direction)
			: coord(coord), negative_direction(negative_direction) {}
	};
	array<array<range_coordinate>> ranges;
	array<real> s;
	vector<real, 2> v, v1, v2, v3;
	geometry_path transformed_path;
	matrix<real, 3, 3> elliptic_arc_transform;
	matrix<real, 1, 3> p;
	vector<real, 2> p1, p2, p3, p4;
	if(path.data.size == 0) return;
	transformed_path.data.increase_capacity(2 * path.data.size);
	for(uint64 i = 0; i < path.data.size; i++)
	{
		if(path.data.addr[i].type == geometry_path_unit::move)
		{
			p = vector<real, 3>(path.data.addr[i].p1.x, path.data.addr[i].p1.y, 1.0r) * transform;
			p2 = vector<real, 2>(p.m[0][0], p.m[0][1]);
			transformed_path.move(p2);
		}
		else if(path.data.addr[i].type == geometry_path_unit::line)
		{
			p = vector<real, 3>(path.data.addr[i].p1.x, path.data.addr[i].p1.y, 1.0r) * transform;
			p2 = vector<real, 2>(p.m[0][0], p.m[0][1]);
			if(transformed_path.data.size != 0 && transformed_path.data.addr[transformed_path.data.size - 1].p1 != p2)
				transformed_path.push_line(p2);
		}
		else if(path.data.addr[i].type == geometry_path_unit::quadratic_arc)
		{
			p = vector<real, 3>(path.data.addr[i].p1.x, path.data.addr[i].p1.y, 1.0r) * transform;
			p2 = vector<real, 2>(p.m[0][0], p.m[0][1]);
			p = vector<real, 3>(path.data.addr[i].p2.x, path.data.addr[i].p2.y, 1.0r) * transform;
			p3 = vector<real, 2>(p.m[0][0], p.m[0][1]);
			ts = 1.0r / max(
				max(p1.x, p2.x, p3.x) - min(p1.x, p2.x, p3.x),
				max(p1.y, p2.y, p3.y) - min(p1.y, p2.y, p3.y));
			tm = 1.0r - ts;
			for(t = ts; t <= tm; t += ts)
			{
				p4 = quadratic_bezier_point(p1, p2, p3, t);
				if(transformed_path.data.size != 0 && transformed_path.data.addr[transformed_path.data.size - 1].p1 != p4)
					transformed_path.push_line(p4);
			}
			if(transformed_path.data.size != 0 && transformed_path.data.addr[transformed_path.data.size - 1].p1 != p3)
				transformed_path.push_line(p3);
			p2 = p3;
		}
		else
		{
			p = vector<real, 3>(path.data.addr[i].p1.x, path.data.addr[i].p1.y, 1.0r) * transform;
			p2 = vector<real, 2>(p.m[0][0], p.m[0][1]);
			tm = path.data.addr[i].end_angle;
			if(path.data.addr[i].begin_angle >= path.data.addr[i].end_angle)
				tm += 1.0r;
			ts = (tm - path.data.addr[i].begin_angle) / (2.0r * 3.14r
				* root(0.5r * (path.data.addr[i].rx * path.data.addr[i].rx + path.data.addr[i].ry * path.data.addr[i].ry), 2));
			tm -= ts;
			elliptic_arc_transform = rotate_matrix(path.data.addr[i].rotation, path.data.addr[i].p2) * transform;
			for(t = path.data.addr[i].begin_angle + ts; t <= tm; t += ts)
			{
				p4 = elliptic_arc_point(path.data.addr[i].p2, path.data.addr[i].rx, path.data.addr[i].ry, t);
				p = vector<real, 3>(p4.x, p4.y, 1.0r) * elliptic_arc_transform;
				p4 = vector<real, 2>(p.m[0][0], p.m[0][1]);
				if(transformed_path.data.size != 0 && transformed_path.data.addr[transformed_path.data.size - 1].p1 != p4)
					transformed_path.push_line(p4);
			}
			if(transformed_path.data.size != 0 && transformed_path.data.addr[transformed_path.data.size - 1].p1 != p2)
				transformed_path.push_line(p2);
		}
		p1 = p2;
	}
	if(rasterization == rasterization_mode::outline)
		outline_path(line_width, &transformed_path);
	transformed_path.orientation = path.orientation;
	for(idx = 0; idx < transformed_path.data.size; idx++)
	{
		lx = min(lx, transformed_path.data.addr[idx].p1.x);
		hx = max(hx, transformed_path.data.addr[idx].p1.x);
		ly = min(ly, transformed_path.data.addr[idx].p1.y);
		hy = max(hy, transformed_path.data.addr[idx].p1.y);
	}
	lx = floor(lx - 1.0r);
	x1 = int32(lx);
	hx = ceil(hx + 2.0r);
	x2 = int32(hx);
	ly = floor(ly);
	hy = ceil(hy);
	ranges.insert_default(0, uint64((hy - ly).integer + 1) * sublines);
	for(idx = 0; idx < transformed_path.data.size; idx++)
	{
		if(transformed_path.data.addr[idx].type == geometry_path_unit::move)
			v1 = transformed_path.data.addr[idx].p1;
		else
		{
			v2 = transformed_path.data.addr[idx].p1;
			l1 = uint64((v1.y - ly) * real(sublines));
			l2 = uint64((v2.y - ly) * real(sublines));
			if(l2 < l1) swap(&l1, &l2);
			y = ly + real(l1 + 1) * dy;
			a = (v2.x - v1.x) / (v2.y - v1.y);
			while(l1 < l2)
			{
				x = v1.x + a * (y - v1.y);
				for(j = 0; j < ranges.addr[l1].size && x > ranges.addr[l1].addr[j].coord; j++);
				ranges.addr[l1].insert(j, range_coordinate(x, v2.y < v1.y));
				l1++;
				y += dy;
			}
			v1 = v2;
		}
	}
	s.insert_default(0, (uint64)(x2 - x1));
	for(idx = 0; idx < s.size; idx++)
		s.addr[idx] = 0.0r;
	for(l = 0; l < ranges.size; l += sublines)
	{
		yb = int32(ly) + int32(uint32(l) / sublines);
		if(yb < 0 || yb >= int32(bmp->height)
			|| scissor_stack.size != 0
			&& (yb < scissor_stack.addr[scissor_stack.size - 1].position.y
				|| yb >= scissor_stack.addr[scissor_stack.size - 1].position.y
				+ scissor_stack.addr[scissor_stack.size - 1].extent.y)) continue;
		for(j = 0; j < sublines; j++)
		{
			m = 0;
			for(k = 0; k + 1 < ranges.addr[l + j].size; k++)
			{
				if(ranges.addr[l + j].addr[k].negative_direction) m++;
				else m--;
				if(transformed_path.orientation == face_orientation::counterclockwise && m <= 0
					|| transformed_path.orientation == face_orientation::clockwise && m >= 0) continue;
				b1 = int32(floor(ranges.addr[l + j].addr[k].coord));
				b2 = int32(floor(ranges.addr[l + j].addr[k + 1].coord));
				if(b1 == b2) s.addr[b1 - x1]
					+= (ranges.addr[l + j].addr[k + 1].coord - ranges.addr[l + j].addr[k].coord) * dy;
				else
				{
					s.addr[b1 - x1] += (real(b1 + 1) - ranges.addr[l + j].addr[k].coord) * dy;
					s.addr[b2 - x1] += (ranges.addr[l + j].addr[k + 1].coord - real(b2)) * dy;
					for(b1++; b1 < b2; b1++)
						s.addr[b1 - x1] += dy;
				}
			}
		}
		for(idx = 0; idx < s.size; idx++)
		{
			if(s.addr[idx] == 0.0r) continue;
			xb = x1 + int32(idx);
			if(xb < 0 || xb >= int32(bmp->width)
				|| scissor_stack.size != 0
				&& (xb < scissor_stack.addr[scissor_stack.size - 1].position.x
				|| xb >= scissor_stack.addr[scissor_stack.size - 1].position.x
				+ scissor_stack.addr[scissor_stack.size - 1].extent.x))
			{
				s.addr[idx] = 0.0r;
				continue;
			}
			a = s.addr[idx] * opacity;
			color_value = point_color(xb, yb);
			color_addr = &bmp->data[(int32(bmp->height) - 1 - yb) * int32(bmp->width) + xb];
			if(color_value.a == 255 && a == 1.0r)
				*color_addr = color_value;
			else
			{
				color_value.a = uint8(round(real(color_value.a) * a));
				color_addr->r = (uint32(color_value.a) * color_value.r + (255 - color_value.a) * color_addr->r) / 255;
				color_addr->g = (uint32(color_value.a) * color_value.g + (255 - color_value.a) * color_addr->g) / 255;
				color_addr->b = (uint32(color_value.a) * color_value.b + (255 - color_value.a) * color_addr->b) / 255;
				color_addr->a = max(color_addr->a, color_value.a);
			}
			s.addr[idx] = 0.0r;
		}
	}
}

void bitmap_processor::fill_area(rectangle<int32> target_area, bitmap *target) //!!!opacity
{
	vector<int32, 2> p,
		p1(max(0, target_area.position.x), max(0, target_area.position.y)),
		p2(min(target_area.position.x + target_area.extent.x, int32(target->width)),
			min(target_area.position.y + target_area.extent.y, int32(target->height)));
	if(scissor_stack.size != 0)
	{
		p1.x = max(p1.x, scissor_stack.addr[scissor_stack.size - 1].position.x);
		p1.y = max(p1.y, scissor_stack.addr[scissor_stack.size - 1].position.y);
		p2.x = min(p2.x, scissor_stack.addr[scissor_stack.size - 1].position.x
			+ scissor_stack.addr[scissor_stack.size - 1].extent.x);
		p2.y = min(p2.y, scissor_stack.addr[scissor_stack.size - 1].position.y
			+ scissor_stack.addr[scissor_stack.size - 1].extent.y);
	}
	alpha_color color_value, *color_addr;
	for(p.x = p1.x; p.x < p2.x; p.x++)
	{
		for(p.y = p1.y; p.y < p2.y; p.y++)
		{
			color_value = point_color(p.x, p.y);
			if(color_value.a == 255)
				target->data[(int32(target->height) - 1 - p.y) * int32(target->width) + p.x] = color_value;
			else if(color_value.a != 0)
			{
				color_addr = &target->data[(int32(target->height) - 1 - p.y) * int32(target->width) + p.x];
				color_addr->r = (uint32(color_value.a) * color_value.r + (255 - color_value.a) * color_addr->r) / 255;
				color_addr->g = (uint32(color_value.a) * color_value.g + (255 - color_value.a) * color_addr->g) / 255;
				color_addr->b = (uint32(color_value.a) * color_value.b + (255 - color_value.a) * color_addr->b) / 255;
				color_addr->a = max(color_addr->a, color_value.a);
			}
		}
	}
}

void bitmap_processor::fill_bitmap(bitmap &source, vector<int32, 2> target_point, bitmap *target) //!!!opacity
{
	vector<int32, 2> p,
		p1(max(0, target_point.x), max(0, target_point.y)),
		p2(min(target_point.x + int32(source.width), int32(target->width)),
			min(target_point.y + int32(source.height), int32(target->height)));
	if(scissor_stack.size != 0)
	{
		p1.x = max(p1.x, scissor_stack.addr[scissor_stack.size - 1].position.x);
		p1.y = max(p1.y, scissor_stack.addr[scissor_stack.size - 1].position.y);
		p2.x = min(p2.x, scissor_stack.addr[scissor_stack.size - 1].position.x
			+ scissor_stack.addr[scissor_stack.size - 1].extent.x);
		p2.y = min(p2.y, scissor_stack.addr[scissor_stack.size - 1].position.y
			+ scissor_stack.addr[scissor_stack.size - 1].extent.y);
	}
	uint32 i, j;
	alpha_color color_value, *color_addr;
	for(p.x = p1.x; p.x < p2.x; p.x++)
	{
		i = uint32(p.x - target_point.x);
		for(p.y = p1.y; p.y < p2.y; p.y++)
		{
			j = uint32(p.y - target_point.y);
			if(source.data[(source.height - 1 - j) * source.width + i].a == 255)
				target->data[(int32(target->height) - 1 - p.y) * int32(target->width) + p.x]
					= source.data[(source.height - 1 - j) * source.width + i];
			else if(source.data[(source.height - 1 - j) * source.width + i].a != 0)
			{
				color_value = source.data[(source.height - 1 - j) * source.width + i];
				color_addr = &target->data[(int32(target->height) - 1 - p.y) * int32(target->width) + p.x];
				color_addr->r = (uint32(color_value.a) * color_value.r + (255 - color_value.a) * color_addr->r) / 255;
				color_addr->g = (uint32(color_value.a) * color_value.g + (255 - color_value.a) * color_addr->g) / 255;
				color_addr->b = (uint32(color_value.a) * color_value.b + (255 - color_value.a) * color_addr->b) / 255;
				color_addr->a = max(color_addr->a, color_value.a);
			}
		}
	}
}

void bitmap_processor::fill_opacity_bitmap(bitmap &source, vector<int32, 2> target_point, bitmap *target) //!!!opacity
{
	vector<int32, 2> p,
		p1(max(0, target_point.x), max(0, target_point.y)),
		p2(min(target_point.x + int32(source.width), int32(target->width)),
			min(target_point.y + int32(source.height), int32(target->height)));
	if(scissor_stack.size != 0)
	{
		p1.x = max(p1.x, scissor_stack.addr[scissor_stack.size - 1].position.x);
		p1.y = max(p1.y, scissor_stack.addr[scissor_stack.size - 1].position.y);
		p2.x = min(p2.x, scissor_stack.addr[scissor_stack.size - 1].position.x
			+ scissor_stack.addr[scissor_stack.size - 1].extent.x);
		p2.y = min(p2.y, scissor_stack.addr[scissor_stack.size - 1].position.y
			+ scissor_stack.addr[scissor_stack.size - 1].extent.y);
	}
	uint32 i, j;
	alpha_color color_value, *color_addr;
	for(p.x = p1.x; p.x < p2.x; p.x++)
	{
		i = uint32(p.x - target_point.x);
		for(p.y = p1.y; p.y < p2.y; p.y++)
		{
			j = uint32(p.y - target_point.y);
			if(source.data[(source.height - 1 - j) * source.width + i].a != 0)
			{
				color_value = point_color(p.x, p.y);
				color_value.a = uint8(uint32(color_value.a) * uint32(source.data[(source.height - 1 - j) * source.width + i].a) / 255);
				color_addr = &target->data[(int32(target->height) - 1 - p.y) * int32(target->width) + p.x];
				color_addr->r = (uint32(color_value.a) * color_value.r + (255 - color_value.a) * color_addr->r) / 255;
				color_addr->g = (uint32(color_value.a) * color_value.g + (255 - color_value.a) * color_addr->g) / 255;
				color_addr->b = (uint32(color_value.a) * color_value.b + (255 - color_value.a) * color_addr->b) / 255;
				color_addr->a = max(color_addr->a, color_value.a);
			}
		}
	}
}
