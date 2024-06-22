#pragma once
#include "array.h"
#include "real.h"

struct string_line
{
	char32 *addr;
	uint64 length;

	string_line() {}

	template<uint64 size> string_line(const char32 (&str)[size])
		: addr(const_cast<char32 *>(str)), length(size - 1) {}

	string_line(const char32 *addr, uint64 length)
		: addr(const_cast<char32 *>(addr)), length(length) {}

};

struct string
{
	char32 *addr;
	uint64 size;
	uint64 capacity;

	string();
	string(const char32 *source);
	~string();
	void increase_capacity(uint64 min_value);
	void insert(uint64 idx, const char32 &value);
	void insert_range(uint64 idx, const char32 *begin, const char32 *end);
	void insert_default(uint64 idx, uint64 count);
	void push(const char32 &value);
	void push_default();
	void remove(uint64 idx);
	void remove_range(uint64 begin, uint64 end);
	void pop();
	void clear();
	void reset();
	char32 &front();
	char32 &back();
	char32 &operator[](uint64 idx);
	string &operator<<(string &source);
	string &operator<<(char32 ch);
	string &operator<<(const char32 *source);
	string &operator<<(string_line source);
	string &operator<<(int8 value);
	string &operator<<(uint8 value);
	string &operator<<(int16 value);
	string &operator<<(uint16 value);
	string &operator<<(int32 value);
	string &operator<<(uint32 value);
	string &operator<<(int64 value);
	string &operator<<(uint64 value);
	string &operator<<(real value);
	string &operator<<=(string &source);
	string &operator<<=(char32 ch);
	string &operator<<=(const char32 *source);
	string &operator<<=(string_line source);
	string &operator<<=(int8 value);
	string &operator<<=(uint8 value);
	string &operator<<=(int16 value);
	string &operator<<=(uint16 value);
	string &operator<<=(int32 value);
	string &operator<<=(uint32 value);
	string &operator<<=(int64 value);
	string &operator<<=(uint64 value);
	string &operator<<=(real value);
};

struct string_stream
{
	char32 *addr;
	uint64 size;
	uint64 position;
	uint64 chars_skiped;
	uint64 chars_converted;

	string_stream() {}
	string_stream(const char32 *string_addr, uint64 string_size);
	bool ended();
	void operator>>(char32 &ch);
	void operator>>(string &str);
	void operator>>(int8 &value);
	void operator>>(uint8 &value);
	void operator>>(int16 &value);
	void operator>>(uint16 &value);
	void operator>>(int32 &value);
	void operator>>(uint32 &value);
	void operator>>(int64 &value);
	void operator>>(uint64 &value);
	void operator>>(real &value);
};

struct string_mapping_config
{
	uint64 min_fraction_digits;
	uint64 max_fraction_digits;

	string_mapping_config();
};

template<> struct utility<string>
{
	string_mapping_config *config();
	compare_result compare(const string &value1, const string &value2);
	compare_result compare_lines(string_line value1, string_line value2);
	void copy(const string &source, string *target);
	uint64 length(const char32 *str);
	char32 *create_sz(string &str);
};
