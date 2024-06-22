#include "string.h"

string::string()
{
	addr = nullptr;
	size = 0;
	capacity = 0;
}

string::string(const char32 *source)
{
	addr = nullptr;
	size = 0;
	capacity = 0;
	*this << source;
}

string::~string()
{
	if(addr != nullptr) delete[] addr;
}

void string::increase_capacity(uint64 min_value)
{
	min_value += capacity;
	if(capacity == 0) capacity = 6;
	while(capacity < min_value) capacity <<= 1;
	if(addr == nullptr)
		addr = new char32[capacity];
	else
	{
		char32 *target_addr = new char32[capacity];
		copy_memory(addr, target_addr, size * sizeof(char32));
		delete[] addr;
		addr = target_addr;
	}
}

void string::insert(uint64 idx, const char32 &value)
{
	if(size == capacity) increase_capacity(1);
	move_memory(addr + idx, addr + idx + 1, (size - idx) * sizeof(char32));
	size++;
	addr[idx] = value;
}

void string::insert_range(uint64 idx, const char32 *begin, const char32 *end)
{
	uint64 count = end - begin;
	if(capacity < size + count)
		increase_capacity(size + count - capacity);
	move_memory(addr + idx, addr + idx + count, (size - idx) * sizeof(char32));
	size += count;
	copy_memory(begin, addr + idx, count * sizeof(char32));
}

void string::insert_default(uint64 idx, uint64 count)
{
	if(capacity < size + count)
		increase_capacity(size + count - capacity);
	move_memory(addr + idx, addr + idx + count, (size - idx) * sizeof(char32));
	construct_range(addr + idx, addr + idx + count);
	size += count;
}

void string::push(const char32 &value)
{
	if(size == capacity) increase_capacity(1);
	addr[size] = value;
	size++;
}

void string::push_default()
{
	if(size == capacity) increase_capacity(1);
	size++;
	construct(addr + size - 1);
}

void string::remove(uint64 idx)
{
	move_memory(addr + idx + 1, addr + idx, (size - idx - 1) * sizeof(char32));
	size--;
}

void string::remove_range(uint64 begin, uint64 end)
{
	move_memory(addr + end, addr + begin, (size - end) * sizeof(char32));
	size -= end - begin;
}

void string::pop()
{
	size--;
}

void string::clear()
{
	size = 0;
}

void string::reset()
{
	if(addr == nullptr) return;
	delete[] addr;
	addr = nullptr;
	size = 0;
	capacity = 0;
}

char32 &string::front()
{
	return addr[0];
}

char32 &string::back()
{
	return addr[size - 1];
}

char32 &string::operator[](uint64 idx)
{
	return addr[idx];
}

string &string::operator<<(string &source)
{
	insert_range(size, source.addr, source.addr + source.size);
	return *this;
}

string &string::operator<<(char32 ch)
{
	push(ch);
	return *this;
}

string &string::operator<<(const char32 *source)
{
	uint64 length = 0;
	while(source[length] != 0) length++;
	insert_range(size, source, source + length);
	return *this;
}

string &string::operator<<(string_line source)
{
	insert_range(size, source.addr, source.addr + source.length);
	return *this;
}

string &string::operator<<(int8 value)
{
	return *this << int64(value);
}

string &string::operator<<(uint8 value)
{
	return *this << uint64(value);
}

string &string::operator<<(int16 value)
{
	return *this << int64(value);
}

string &string::operator<<(uint16 value)
{
	return *this << uint64(value);
}

string &string::operator<<(int32 value)
{
	return *this << int64(value);
}

string &string::operator<<(uint32 value)
{
	return *this << uint64(value);
}

string &string::operator<<(int64 value)
{
	if(value < 0)
	{
		push(U'-');
		value = -value;
	}
	return *this << uint64(value);
}

string &string::operator<<(uint64 value)
{
	if(value == 0) push(U'0');
	uint64 position = size;
	while(value != 0)
	{
		insert(position, value % 10 + U'0');
		value /= 10;
	}
	return *this;
}

string &string::operator<<(real value)
{
	if(value.negative) push(U'-');
	*this << uint64(value.integer);
	uint64 position = size;
	push('.');
	*this << uint64(value.fraction);
	while(size - position - 1 < utility<string>().config()->min_fraction_digits)
		push(U'0');
	while(size - position - 1 > utility<string>().config()->max_fraction_digits)
		pop();
	if(position == size - 1) pop();
	return *this;
}

string &string::operator<<=(string &source)
{
	clear();
	return *this << source;
}

string &string::operator<<=(char32 ch)
{
	clear();
	return *this << ch;
}

string &string::operator<<=(const char32 *source)
{
	clear();
	return *this << source;
}

string &string::operator<<=(string_line source)
{
	clear();
	return *this << source;
}

string &string::operator<<=(int8 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(uint8 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(int16 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(uint16 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(int32 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(uint32 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(int64 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(uint64 value)
{
	clear();
	return *this << value;
}

string &string::operator<<=(real value)
{
	clear();
	return *this << value;
}

string_stream::string_stream(const char32 *string_addr, uint64 string_size)
{
    addr = const_cast<char32 *>(string_addr);
    size = string_size;
    position = 0;
    chars_skiped = 0;
    chars_converted = 0;
}

bool string_stream::ended()
{
	return position == size;
}

void string_stream::operator>>(char32 &ch)
{
    if(ended())
    {
        chars_skiped = 0;
        chars_converted = 0;
        return;
    }
    ch = addr[position];
    position++;
    chars_skiped = 0;
    chars_converted = 1;
}

void string_stream::operator>>(string &str)
{
    if(ended())
    {
        chars_skiped = 0;
        chars_converted = 0;
        return;
    }
    str.push(addr[position]);
    position++;
    chars_skiped = 0;
    chars_converted = 1;
}

void string_stream::operator>>(int8 &value)
{
    int64 result;
    *this >> result;
    value = int8(result);
}

void string_stream::operator>>(uint8 &value)
{
    uint64 result;
    *this >> result;
    value = uint8(result);
}

void string_stream::operator>>(int16 &value)
{
    int64 result;
    *this >> result;
	value = int16(result);
}

void string_stream::operator>>(uint16 &value)
{
    uint64 result;
    *this >> result;
    value = uint16(result);
}

void string_stream::operator>>(int32 &value)
{
	int64 result;
	*this >> result;
	value = int32(result);
}

void string_stream::operator>>(uint32 &value)
{
    uint64 result;
    *this >> result;
    value = uint32(result);
}

void string_stream::operator>>(int64 &value)
{
	int64 result = 0;
    chars_skiped = 0;
    chars_converted = 0;
    while(!ended() && (addr[position] < U'0' || addr[position] > U'9'))
    {
        position++;
        chars_skiped++;
    }
	uint64 num_iter = position;
	while(num_iter != size && addr[num_iter] >= U'0' && addr[num_iter] <= U'9')
	{
		result *= 10;
		result += int64(addr[num_iter]) - 48;
		chars_converted++;
		num_iter++;
	}
	if(chars_converted != 0)
	{
		if(chars_skiped != 0 && addr[position - 1] == '-')
			result = -result;
		position = num_iter;
		value = result;
	}
}

void string_stream::operator>>(uint64 &value)
{
    uint64 result = 0;
    chars_skiped = 0;
    chars_converted = 0;
    while(!ended() && (addr[position] < U'0' || addr[position] > U'9'))
    {
        position++;
        chars_skiped++;
    }
	uint64 num_iter = position;
	while(num_iter != size && addr[num_iter] >= U'0' && addr[num_iter] <= U'9')
	{
		result *= 10;
		result += uint64(addr[num_iter]) - 48;
		chars_converted++;
		num_iter++;
	}
	if(chars_converted != 0)
	{
		position = num_iter;
		value = result;
	}
}

void string_stream::operator>>(real &value)
{
	real result;
	int64 integer;
	*this >> integer;
	if(chars_converted != 0)
	{
		if(integer < 0)
		{
			result.negative = true;
			result.integer = uint32(-integer);
		}
		else
		{
			result.negative = false;
			result.integer = uint32(integer);
		}
		if(!ended() && addr[position] == '.'
			&& position + 1 != size && addr[position + 1] >= U'0' && addr[position + 1] <= U'9')
		{
			position++;
			chars_converted++;
			uint32 chars_converted_temp = chars_converted;
			uint64 fraction;
			*this >> fraction;
			result.fraction = uint32(fraction);
			chars_converted += chars_converted_temp;
		}
		else result.fraction = 0;
		value = result;
	}
}

string_mapping_config smp;

string_mapping_config::string_mapping_config()
{
	min_fraction_digits = 1;
	max_fraction_digits = 9;
}

string_mapping_config *utility<string>::config()
{
	return &smp;
}

compare_result utility<string>::compare(const string &value1, const string &value2)
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

compare_result utility<string>::compare_lines(string_line value1, string_line value2)
{
	compare_result result = compare_memory<char32>(
		value1.addr,
		value2.addr,
		min(value1.length, value2.length) * sizeof(char32));
	if(result == compare_result::equal)
	{
		if(value1.length == value2.length) return compare_result::equal;
		else if(value1.length < value2.length) return compare_result::less;
		else return compare_result::greater;
	}
	else return result;
}

void utility<string>::copy(const string &source, string *target)
{
	if(target->capacity < source.size)
		target->increase_capacity(source.size - target->capacity);
	target->size = source.size;
	copy_memory(source.addr, target->addr, source.size * sizeof(char32));
}

uint64 utility<string>::length(const char32 *str)
{
	uint64 length = 0;
	while(str[length] != U'\0') length++;
	return length;
}

char32 *utility<string>::create_sz(string &str)
{
	char32 *addr = new char32[str.size + 1];
	copy_memory(str.addr, addr, str.size * sizeof(char32));
	addr[str.size] = U'\0';
	return addr;
}
