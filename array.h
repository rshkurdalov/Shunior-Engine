#pragma once
#include "global_operators.h"

template<typename value_type> struct array
{
	value_type *addr;
	uint64 size;
	uint64 capacity;

	array()
	{
		addr = nullptr;
		size = 0;
		capacity = 0;
	}

	~array()
	{
		if(addr == nullptr) return;
		destroy_range(addr, addr + size);
		delete[] (byte *)addr;
	}

	void increase_capacity(uint64 min_value)
	{
		min_value += capacity;
		if(capacity == 0) capacity = 6;
		while(capacity < min_value) capacity <<= 1;
		if(addr == nullptr)
			addr = (value_type *)new byte[capacity * sizeof(value_type)];
		else
		{
			value_type *target_addr = (value_type *)new byte[capacity * sizeof(value_type)];
			copy_memory(addr, target_addr, size * sizeof(value_type));
			delete[] (byte *)addr;
			addr = target_addr;
		}
	}

	void insert(uint64 idx, const value_type &value)
	{
		if(size == capacity) increase_capacity(1);
		move_memory(addr + idx, addr + idx + 1, (size - idx) * sizeof(value_type));
		addr[idx] = value;
		size++;
	}

	void insert_range(uint64 idx, const value_type *begin, const value_type *end)
	{
		uint64 count = end - begin;
		if(capacity < size + count)
			increase_capacity(size + count - capacity);
		move_memory(addr + idx, addr + idx + count, (size - idx) * sizeof(value_type));
		copy_memory(begin, addr + idx, count * sizeof(value_type));
		size += count;
	}

	void insert_default(uint64 idx, uint64 count)
	{
		if(capacity < size + count)
			increase_capacity(size + count - capacity);
		move_memory(addr + idx, addr + idx + count, (size - idx) * sizeof(value_type));
		construct_range(addr + idx, addr + idx + count);
		size += count;
	}

	void push(const value_type &value)
	{
		if(size == capacity) increase_capacity(1);
		addr[size] = value;
		size++;
	}

	void remove(uint64 idx)
	{
		destroy(addr + idx);
		move_memory(addr + idx + 1, addr + idx, (size - idx - 1) * sizeof(value_type));
		size--;
	}

	void remove_range(uint64 begin, uint64 end)
	{
		destroy_range(addr + begin, addr + end);
		move_memory(addr + end, addr + begin, (size - end) * sizeof(value_type));
		size -= end - begin;
	}

	void pop()
	{
		destroy(addr + size - 1);
		size--;
	}

	void clear()
	{
		destroy_range(addr, addr + size);
		size = 0;
	}

	void reset()
	{
		if(addr == nullptr) return;
		destroy_range(addr, addr + size);
		delete[] (byte *)addr;
		addr = nullptr;
		size = 0;
		capacity = 0;
	}

	uint64 lower_bound(const key<value_type> &key_value)
	{
		if(size == 0) return 0;
		if(key<value_type>(addr[size - 1]) < key_value) return size;
		uint64 l = 0, r = size - 1, m;
		while(true)
		{
			m = (l + r) / 2;
			if(l == m)
			{
				if(!(key<value_type>(addr[l]) < key_value))
					return l;
				else return r;
			}
			if(!(key<value_type>(addr[m]) < key_value))
				r = m;
			else l = m;
		}
	}

	uint64 upper_bound(const key<value_type> &key_value)
	{
		if(size == 0) return 0;
		if(key_value < key<value_type>(addr[0])) return size;
		uint64 l = 0, r = size - 1, m;
		while(true)
		{
			m = (l + r) / 2;
			if(l == m)
			{
				if(!(key_value < key<value_type>(addr[r])))
					return r;
				else return l;
			}
			if(!(key_value < key<value_type>(addr[m])))
				l = m;
			else r = m;
		}
	}

	uint64 binary_search(const key<value_type> &key_value)
	{
		uint64 idx = lower_bound(key_value);
		if(idx == size || key_value < key<value_type>(addr[idx]))
			return size;
		else return idx;
	}
	
	void binary_insert(const value_type &value)
	{
		uint64 idx = upper_bound(key<value_type>(value));
		if(idx == size) insert(0, value);
		else insert(idx + 1, value);
	}

	void binary_remove(const key<value_type> &key_value)
	{
		uint64 l = lower_bound(key_value), r = l;
		while(r != size && !(key_value < key<value_type>(addr[r]))) r++;
		remove_range(l, r);
	}
};
