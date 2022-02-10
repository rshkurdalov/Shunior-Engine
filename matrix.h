#pragma once
#include "global_operators.h"
#include "vector.h"

template<typename value_type, uint32 rows, uint32 columns> struct matrix
{
	value_type m[rows][columns];

	matrix() {}

	matrix(value_type right[rows][columns])
	{
		static_copy<rows * columns * sizeof(value_type)>(right, m);
	}

	matrix(value_type right)
	{
		*this = right;
	}

	matrix<value_type, rows, columns> &operator=(value_type right)
	{
		coherent_run<rows * columns>([right](value_type &value) -> void { value = right; }, *m);
		return *this;
	}
};

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator-(matrix<value_type, rows, columns> value)
{
	coherent_run<rows * columns>(negate<value_type>, *value.m);
	return value;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> &operator+=(
	matrix<value_type, rows, columns> &left,
	matrix<value_type, rows, columns> right)
{
	coherent_run<rows * columns>(add<value_type>, *left.m, *right.m);
	return left;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator+(
	matrix<value_type, rows, columns> left,
	matrix<value_type, rows, columns> right)
{
	return left += right;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> &operator-=(
	matrix<value_type, rows, columns> &left,
	matrix<value_type, rows, columns> right)
{
	coherent_run<rows * columns>(sub<value_type>, *left.m, *right.m);
	return left;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator-(
	matrix<value_type, rows, columns> left,
	matrix<value_type, rows, columns> right)
{
	return left -= right;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> &operator*=(
	matrix<value_type, rows, columns> &left,
	value_type right)
{
	coherent_run<rows * columns>([right](value_type &value) -> void { value *= right; }, *left.m);
	return left;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator*(
	matrix<value_type, rows, columns> left,
	value_type right)
{
	return left *= right;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator*(
	value_type left,
	matrix<value_type, rows, columns> right)
{
	return right *= left;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator*(
	matrix<value_type, rows, 1> left,
	vector<value_type, columns> right)
{
	matrix<value_type, rows, columns> result;
	for(uint32 i = 0; i < rows; i++)
	{
		for(uint32 j = 0; j < columns; j++)
			result.m[i][j] = left.m[i][0] * right.coord[j];
	}
	return result;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, 1, columns> operator*(
	vector<value_type, rows> left,
	matrix<value_type, rows, columns> right)
{
	matrix<value_type, 1, columns> result;
	for(uint32 i = 0; i < columns; i++)
	{
		result.m[0][i] = static_cast<value_type>(0);
		for(uint32 j = 0; j < rows; j++)
			result.m[0][i] += left.coord[j] * right.m[j][i];
	}
	return result;
}

template<typename value_type, uint32 rows, uint32 columns1, uint32 columns2>
matrix<value_type, rows, columns2> operator*(
	matrix<value_type, rows, columns1> left,
	matrix<value_type, columns1, columns2> right)
{
	matrix<value_type, rows, columns2> result;
	for(uint32 i = 0; i < rows; i++)
	{
		for(uint32 j = 0; j < columns2; j++)
		{
			result.m[i][j] = static_cast<value_type>(0);
			for(uint32 k = 0; k < columns1; k++)
				result.m[i][j] += left.m[i][k] * right.m[k][j];
		}
	}
	return result;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> &operator/=(
	matrix<value_type, rows, columns> &left,
	value_type right)
{
	coherent_run<rows * columns>([right](value_type &value) -> void { value /= right; }, *left.m);
	return left;
}

template<typename value_type, uint32 rows, uint32 columns>
matrix<value_type, rows, columns> operator/(
	matrix<value_type, rows, columns> left,
	value_type right)
{
	return left /= right;
}

template<typename value_type> struct dynamic_matrix
{
	value_type *addr;
	uint64 rows;
	uint64 columns;

	dynamic_matrix()
	{
		addr = nullptr;
		rows = 0;
		columns = 0;
	}
	
	~dynamic_matrix()
	{
		if(addr != nullptr)
			delete[] addr;
	}

	void insert_rows(uint64 insert_idx, uint64 insert_count)
	{
		rows += insert_count;
		if(columns == 0) return;
		value_type *new_addr = new value_type[rows * columns];
		for(uint64 i = 0; i < insert_idx; i++)
			for(uint64 j = 0; j < columns; j++)
				new_addr[i * columns + j] = addr[i * columns + j];
		for(uint64 i = insert_idx; i < rows - insert_count; i++)
			for(uint64 j = 0; j < columns; j++)
				new_addr[(i + insert_count) * columns + j] = addr[i * columns + j];
		if(addr != nullptr) delete[] addr;
		addr = new_addr;
	}

	void remove_rows(uint64 remove_idx, uint64 remove_count)
	{
		rows -= remove_count;
		if(rows == 0 || columns == 0)
		{
			if(addr != nullptr) delete[] addr;
			return;
		}
		value_type *new_addr = new value_type[rows * columns];
		for(uint64 i = 0; i < remove_idx; i++)
			for(uint64 j = 0; j < columns; j++)
				new_addr[i * columns + j] = addr[i * columns + j];
		for(uint64 i = remove_idx; i < rows; i++)
			for(uint64 j = 0; j < columns; j++)
				new_addr[i * columns + j] = addr[(i + remove_count) * columns + j];
		delete[] addr;
		addr = new_addr;
	}

	void insert_columns(uint64 insert_idx, uint64 insert_count)
	{
		columns += insert_count;
		if(rows == 0) return;
		value_type *new_addr = new value_type[rows * columns];
		for(uint64 i = 0; i < rows; i++)
			for(uint64 j = 0; j < insert_idx; j++)
				new_addr[i * columns + j] = addr[i * (columns - insert_count) + j];
		for(uint64 i = 0; i < rows; i++)
			for(uint64 j = insert_idx; j < columns - insert_count; j++)
				new_addr[i * columns + j + insert_count] = addr[i * (columns - insert_count) + j];
		if(addr != nullptr) delete[] addr;
		addr = new_addr;
	}

	void remove_columns(uint64 remove_idx, uint64 remove_count)
	{
		columns -= remove_count;
		if(rows == 0 || columns == 0)
		{
			if(addr != nullptr) delete[] addr;
			return;
		}
		value_type *new_addr = new value_type[rows * columns];
		for(uint64 i = 0; i < rows; i++)
			for(uint64 j = 0; j < remove_idx; j++)
				new_addr[i * columns + j] = addr[i * (columns + remove_count) + j];
		for(uint64 i = 0; i < rows; i++)
			for(uint64 j = remove_idx; j < columns; j++)
				new_addr[i * columns + j] = addr[i * (columns + remove_count) + j + remove_count];
		delete[] addr;
		addr = new_addr;
	}

	value_type &at(uint64 row_idx, uint64 column_idx)
	{
		return addr[row_idx * columns + column_idx];
	}
};
