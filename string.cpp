#include "string.h"

bool operator<(const string &value1, const string &value2)
{
	compare_result result = compare_memory<char32>(
		value1.addr,
		value2.addr,
		min(value1.size, value2.size) * sizeof(char32));
	return result == compare_result::less
		|| result == compare_result::equal && value1.size < value2.size;
}

bool operator==(const string &value1, const string &value2)
{
	return value1.size == value2.size
		&& compare_memory<char32>(
			value1.addr,
			value2.addr,
			min(value1.size, value2.size) * sizeof(char32)) == compare_result::equal;
}

bool operator!=(const string &value1, const string &value2)
{
	return !(value1 == value2);
}

bool operator>(const string &value1, const string &value2)
{
	compare_result result = compare_memory<char32>(
		value1.addr,
		value2.addr,
		min(value1.size, value2.size) * sizeof(char32));
	return result == compare_result::greater
		|| result == compare_result::equal && value1.size > value2.size;
}

void operator<<(string &target, const char8 *source)
{
	for(uint64 i = 0; source[i] != '\0'; i++)
		target.push(char32(source[i]));
}

void operator<<(string &target, const char16 *source)
{
	for(uint64 i = 0; source[i] != u'\0'; i++)
		target.push(char32(source[i]));
}

void operator<<(string &target, const char32 *source)
{
	uint64 length = 0;
	while(source[length] != 0) length++;
	target.insert_range(target.size, source, source + length);
}
