#pragma once
#include "string.h"

bool is_word_char_def(char32 ch);

/*struct string_stream
{
	void *addr;
	uint64 size;
	uint64 position;
	uint64 chars_skiped;
	uint64 chars_converted;

	string_stream() {}
	string_stream(void *string_addr, uint64 string_size);
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
};*/
