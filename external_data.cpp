#include "external_data.h"

string &operator<<(string &target, const char8 *source)
{
	uint64 length = u8sz_length(source);
	target.insert_default(target.size, length);
	char32 *target_addr = target.addr + target.size - length;
	while(length != 0)
	{
		*target_addr = char32(*source);
		target_addr++;
		source++;
		length--;
	}
	return target;
}

string &operator<<=(string &target, const char8 *source)
{
	target.clear();
	return target << source;
}

string &operator<<(string &target, const char16 *source)
{
	uint64 length = u16sz_length(source);
	target.insert_default(target.size, length);
	char32 *target_addr = target.addr + target.size - length;
	while(length != 0)
	{
		*target_addr = char32(*source);
		target_addr++;
		source++;
		length--;
	}
	return target;
}

string &operator<<=(string &target, const char16 *source)
{
	target.clear();
	return target << source;
}

char8 *create_u8sz(string &str)
{
	char8 *addr = new char8[str.size + 1];
	for(uint64 i = 0; i < str.size; i++)
		addr[i] = char8(str.addr[i]);
	addr[str.size] = '\0';
	return addr;
}

char16 *create_u16sz(string &str)
{
	char16 *addr = new char16[str.size + 1];
	for(uint64 i = 0; i < str.size; i++)
		addr[i] = char16(str.addr[i]);
	addr[str.size] = u'\0';
	return addr;
}

uint64 u8sz_length(const char8 *str)
{
	uint64 length = 0;
	while(str[length] != '\0') length++;
	return length;
}

uint64 u16sz_length(const char16 *str)
{
	uint64 length = 0;
	while(str[length] != u'\0') length++;
	return length;
}
