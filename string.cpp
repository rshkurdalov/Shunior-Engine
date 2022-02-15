#include "string.h"

string_mapping_params smp;

string_mapping_params::string_mapping_params()
{
	min_fraction_digits = 1;
	max_fraction_digits = 9;
}

string_mapping_params *string_mapping()
{
	return &smp;
}

compare_result compare_strings(const string &value1, const string &value2)
{
	compare_result result = compare_memory<char32>(
		value1.addr,
		value2.addr,
		min(value1.size, value2.size) * sizeof(char32));
	if(result == compare_result::equal)
	{
		if(value1.size == value2.size) return compare_result::equal;
		else if(value1.size < value2.size) return compare_result::less;
		else return compare_result::greater;
	}
	else return result;
}

string &operator<<(string &target, string &source)
{
	target.insert_range(target.size, source.addr, source.addr + source.size);
	return target;
}

string &operator<<(string &target, char32 ch)
{
	target.push(ch);
	return target;
}

string &operator<<(string &target, const char8 *source)
{
	for(uint64 i = 0; source[i] != '\0'; i++)
		target.push(char32(source[i]));
	return target;
}

string &operator<<(string &target, const char16 *source)
{
	for(uint64 i = 0; source[i] != u'\0'; i++)
		target.push(char32(source[i]));
	return target;
}

string &operator<<(string &target, const char32 *source)
{
	uint64 length = 0;
	while(source[length] != 0) length++;
	target.insert_range(target.size, source, source + length);
	return target;
}

string &operator<<(string &target, string_line<char8> source)
{
	for(uint64 i = 0; i < source.length; i++)
		target.push(char32(source.addr[i]));
	return target;
}

string &operator<<(string &target, string_line<char16> source)
{
	for(uint64 i = 0; i < source.length; i++)
		target.push(char32(source.addr[i]));
	return target;
}

string &operator<<(string &target, string_line<char32> source)
{
	target.insert_range(target.size, source.addr, source.addr + source.length);
	return target;
}

string &operator<<(string &target, int8 value)
{
	return target << int64(value);
}

string &operator<<(string &target, uint8 value)
{
	return target << uint64(value);
}

string &operator<<(string &target, int16 value)
{
	return target << int64(value);
}

string &operator<<(string &target, uint16 value)
{
	return target << uint64(value);
}

string &operator<<(string &target, int32 value)
{
	return target << int64(value);
}

string &operator<<(string &target, uint32 value)
{
	return target << uint64(value);
}

string &operator<<(string &target, int64 value)
{
	if(value < 0)
	{
		target.push(U'-');
		value = -value;
	}
	return target << uint64(value);
}

string &operator<<(string &target, uint64 value)
{
	if(value == 0) target.push(U'0');
	uint64 position = target.size;
	while(value != 0)
	{
		target.insert(position, value % 10 + U'0');
		value /= 10;
	}
	return target;
}

string &operator<<(string &target, real value)
{
	if(value.negative) target.push(U'-');
	target << uint64(value.integer);
	uint64 position = target.size;
	target.push('.');
	target << uint64(value.fraction);
	while(target.size - position - 1 < string_mapping()->min_fraction_digits)
		target.push(U'0');
	while(target.size - position - 1 > string_mapping()->max_fraction_digits)
		target.pop();
	if(position == target.size - 1) target.pop();
	return target;
}

void operator>>(string_line<char8> str, int8 &value)
{
	int64 result;
	str >> result;
	value = int8(result);
}

void operator>>(string_line<char16> str, int8 &value)
{
	int64 result;
	str >> result;
	value = int8(result);
}

void operator>>(string_line<char32> str, int8 &value)
{
	int64 result;
	str >> result;
	value = int8(result);
}

void operator>>(string_line<char8> str, uint8 &value)
{
	uint64 result;
	str >> result;
	value = uint8(result);
}

void operator>>(string_line<char16> str, uint8 &value)
{
	uint64 result;
	str >> result;
	value = uint8(result);
}

void operator>>(string_line<char32> str, uint8 &value)
{
	uint64 result;
	str >> result;
	value = uint8(result);
}

void operator>>(string_line<char8> str, int16 &value)
{
	int64 result;
	str >> result;
	value = int16(result);
}

void operator>>(string_line<char16> str, int16 &value)
{
	int64 result;
	str >> result;
	value = int16(result);
}

void operator>>(string_line<char32> str, int16 &value)
{
	int64 result;
	str >> result;
	value = int16(result);
}

void operator>>(string_line<char8> str, uint16 &value)
{
	uint64 result;
	str >> result;
	value = uint16(result);
}

void operator>>(string_line<char16> str, uint16 &value)
{
	uint64 result;
	str >> result;
	value = uint16(result);
}

void operator>>(string_line<char32> str, uint16 &value)
{
	uint64 result;
	str >> result;
	value = uint16(result);
}

void operator>>(string_line<char8> str, int32 &value)
{
	int64 result;
	str >> result;
	value = int32(result);
}

void operator>>(string_line<char16> str, int32 &value)
{
	int64 result;
	str >> result;
	value = int32(result);
}

void operator>>(string_line<char32> str, int32 &value)
{
	int64 result;
	str >> result;
	value = int32(result);
}

void operator>>(string_line<char8> str, uint32 &value)
{
	uint64 result;
	str >> result;
	value = uint32(result);
}

void operator>>(string_line<char16> str, uint32 &value)
{
	uint64 result;
	str >> result;
	value = uint32(result);
}

void operator>>(string_line<char32> str, uint32 &value)
{
	uint64 result;
	str >> result;
	value = uint32(result);
}

void operator>>(string_line<char8> str, int64 &value)
{
	int64 multiplier = 1;
	if(str.length != 0 && str.addr[0] == '-')
	{
		str.addr++;
		str.length--;
		multiplier = -1;
	}
	value = 0;
	while(str.length != 0)
	{
		value += int64(str.addr[str.length - 1] - '0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char16> str, int64 &value)
{
	int64 multiplier = 1;
	if(str.length != 0 && str.addr[0] == u'-')
	{
		str.addr++;
		str.length--;
		multiplier = -1;
	}
	value = 0;
	while(str.length != 0)
	{
		value += int64(str.addr[str.length - 1] - u'0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char32> str, int64 &value)
{
	int64 multiplier = 1;
	if(str.length != 0 && str.addr[0] == U'-')
	{
		str.addr++;
		str.length--;
		multiplier = -1;
	}
	value = 0;
	while(str.length != 0)
	{
		value += int64(str.addr[str.length - 1] - U'0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char8> str, uint64 &value)
{
	uint64 multiplier = 1;
	value = 0;
	while(str.length != 0)
	{
		value += uint64(str.addr[str.length - 1] - '0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char16> str, uint64 &value)
{
	uint64 multiplier = 1;
	value = 0;
	while(str.length != 0)
	{
		value += uint64(str.addr[str.length - 1] - u'0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char32> str, uint64 &value)
{
	uint64 multiplier = 1;
	value = 0;
	while(str.length != 0)
	{
		value += uint64(str.addr[str.length - 1] - U'0') * multiplier;
		str.length--;
		multiplier *= 10;
	}
}

void operator>>(string_line<char8> str, real &value)
{
	if(str.length != 0 && str.addr[0] == '-')
	{
		str.addr++;
		str.length--;
		value.negative = true;
	}
	else value.negative = false;
	uint64 idx = 0;
	while(idx != str.length && str.addr[idx] != '.') idx++;
	string_line<char8>(str.addr, idx) >> value.integer;
	uint32 fraction = 0;
	if(idx != str.length)
	{
		string_line<char8>(str.addr + idx + 1, str.length - idx - 1) >> fraction;
		for(uint32 j = 0; j < 9 - (str.length - idx - 1); fraction *= 10, j++);
	}
	value.fraction = fraction;
}

void operator>>(string_line<char16> str, real &value)
{
	if(str.length != 0 && str.addr[0] == u'-')
	{
		str.addr++;
		str.length--;
		value.negative = true;
	}
	else value.negative = false;
	uint64 idx = 0;
	while(idx != str.length && str.addr[idx] != u'.') idx++;
	string_line<char16>(str.addr, idx) >> value.integer;
	uint32 fraction = 0;
	if(idx != str.length)
	{
		string_line<char16>(str.addr + idx + 1, str.length - idx - 1) >> fraction;
		for(uint32 j = 0; j < 9 - (str.length - idx - 1); fraction *= 10, j++);
	}
	value.fraction = fraction;
}

void operator>>(string_line<char32> str, real &value)
{
	if(str.length != 0 && str.addr[0] == U'-')
	{
		str.addr++;
		str.length--;
		value.negative = true;
	}
	else value.negative = false;
	uint64 idx = 0;
	while(idx != str.length && str.addr[idx] != U'.') idx++;
	string_line<char32>(str.addr, idx) >> value.integer;
	uint32 fraction = 0;
	if(idx != str.length)
	{
		string_line<char32>(str.addr + idx + 1, str.length - idx - 1) >> fraction;
		for(uint32 j = 0; j < 9 - (str.length - idx - 1); fraction *= 10, j++);
	}
	value.fraction = fraction;
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

char32 *create_u32sz(string &str)
{
	char32 *addr = new char32[str.size + 1];
	for(uint64 i = 0; i < str.size; i++)
		addr[i] = str.addr[i];
	addr[str.size] = U'\0';
	return addr;
}

uint64 u8sz_length(char8 *str)
{
	uint64 length = 0;
	while(str[length] != '\0') length++;
	return length;
}

uint64 u16sz_length(char16 *str)
{
	uint64 length = 0;
	while(str[length] != u'\0') length++;
	return length;
}

uint64 u32sz_length(char32 *str)
{
	uint64 length = 0;
	while(str[length] != U'\0') length++;
	return length;
}
