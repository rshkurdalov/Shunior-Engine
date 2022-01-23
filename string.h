#pragma once
#include "array.h"
#include "real.h"

typedef array<char32> string;

template<typename char_type> struct string_line
{
	char_type *addr;
	uint64 length;

	string_line() {}

	string_line(const char_type *addr, uint64 length)
		: addr(const_cast<char_type *>(addr)), length(length) {}
};

struct string_mapping_params
{
	uint64 min_fraction_digits;
	uint64 max_fraction_digits;

	string_mapping_params();
};

string_mapping_params *string_mapping();

bool operator<(const string &value1, const string &value2);
bool operator==(const string &value1, const string &value2);
bool operator!=(const string &value1, const string &value2);
bool operator>(const string &value1, const string &value2);
string &operator<<(string &target, string &source);
string &operator<<(string &target, char32 ch);
string &operator<<(string &target, const char8 *source);
string &operator<<(string &target, const char16 *source);
string &operator<<(string &target, const char32 *source);
string &operator<<(string &target, string_line<char8> source);
string &operator<<(string &target, string_line<char16> source);
string &operator<<(string &target, string_line<char32> source);
string &operator<<(string &target, int8 value);
string &operator<<(string &target, uint8 value);
string &operator<<(string &target, int16 value);
string &operator<<(string &target, uint16 value);
string &operator<<(string &target, int32 value);
string &operator<<(string &target, uint32 value);
string &operator<<(string &target, int64 value);
string &operator<<(string &target, uint64 value);
string &operator<<(string &target, real value);
void operator>>(string_line<char8> str, int8 &value);
void operator>>(string_line<char16> str, int8 &value);
void operator>>(string_line<char32> str, int8 &value);
void operator>>(string_line<char8> str, uint8 &value);
void operator>>(string_line<char16> str, uint8 &value);
void operator>>(string_line<char32> str, uint8 &value);
void operator>>(string_line<char8> str, int16 &value);
void operator>>(string_line<char16> str, int16 &value);
void operator>>(string_line<char32> str, int16 &value);
void operator>>(string_line<char8> str, uint16 &value);
void operator>>(string_line<char16> str, uint16 &value);
void operator>>(string_line<char32> str, uint16 &value);
void operator>>(string_line<char8> str, int32 &value);
void operator>>(string_line<char16> str, int32 &value);
void operator>>(string_line<char32> str, int32 &value);
void operator>>(string_line<char8> str, uint32 &value);
void operator>>(string_line<char16> str, uint32 &value);
void operator>>(string_line<char32> str, uint32 &value);
void operator>>(string_line<char8> str, int64 &value);
void operator>>(string_line<char16> str, int64 &value);
void operator>>(string_line<char32> str, int64 &value);
void operator>>(string_line<char8> str, uint64 &value);
void operator>>(string_line<char16> str, uint64 &value);
void operator>>(string_line<char32> str, uint64 &value);
void operator>>(string_line<char8> str, real &value);
void operator>>(string_line<char16> str, real &value);
void operator>>(string_line<char32> str, real &value);
char8 *create_u8sz(string &str);
char16 *create_u16sz(string &str);
char32 *create_u32sz(string &str);
uint64 u8sz_length(char8 *str);
uint64 u16sz_length(char16 *str);
uint64 u32sz_length(char32 *str);
