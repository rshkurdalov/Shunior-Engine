#pragma once
#include "string.h"

enum struct parse_mode
{
	binary,
	text
};

enum string_format
{
	u8,
	u16,
	u32
};

bool is_word_char_def(char32 ch);

struct parser
{
	void *addr;
	uint64 size;
	parse_mode mode;
	string_format format;
	uint64 position;
	uint64 chars_skiped;
	uint64 chars_converted;
	bool (*is_word_char)(char32 ch);

	parser() {}
	parser(
		void *string_addr,
		uint64 string_size,
		parse_mode parsing_mode,
		string_format encoding);
	char32 current_char();
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
