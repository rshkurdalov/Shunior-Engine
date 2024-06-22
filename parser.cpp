#include "parser.h"
/*
bool is_word_char_def(char32 ch)
{
    return ch >= char32(33);
}

string_stream::string_stream(void *string_addr, uint64 string_size)
{
    addr = string_addr;
    size = string_size;
    position = 0;
    chars_skiped = 0;
    chars_converted = 0;
    is_word_char = is_word_char_def;
}

char32 string_stream::current_char()
{
    if(format == string_format::u32)
       return ((char32 *)(addr))[position];
}

void string_stream::operator>>(char32 &ch)
{
    if(position == size)
    {
        chars_skiped = 0;
        chars_converted = 0;
        return;
    }
    ch = current_char();
    position++;
    chars_skiped = 0;
    chars_converted = 1;
}

void string_stream::operator>>(string &str)
{
    uint64 word_position;
    chars_skiped = 0;
    chars_converted = 0;
    while(position != size && !is_word_char(current_char()))
    {
        position++;
        chars_skiped++;
    }
    word_position = position;
    while(position != size && is_word_char(current_char()))
    {
        position++;
        chars_converted++;
    }
    if(format == string_format::u32)
        str << string_line<char32>((char32 *)(addr) + word_position, position - word_position);
}

void string_stream::operator>>(int8 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 1 <= size)
        {
            static_copy<1>((byte *)(addr) + position, &value);
            position += 1;
            chars_converted = 1;
        }
        else chars_converted = 0;
    }
    else
    {
       int64 result;
       *this >> result;
       value = int8(result);
    }
}

void string_stream::operator>>(uint8 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 1 <= size)
        {
            static_copy<1>((byte *)(addr) + position, &value);
            position += 1;
            chars_converted = 1;
        }
        else chars_converted = 0;
    }
    else
    {
       uint64 result;
       *this >> result;
       value = uint8(result);
    }
}

void string_stream::operator>>(int16 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 2 <= size)
        {
            static_copy<2>((byte *)(addr) + position, &value);
            position += 2;
            chars_converted = 2;
        }
        else chars_converted = 0;
    }
    else
    {
       int64 result;
       *this >> result;
       value = int16(result);
    }
}

void string_stream::operator>>(uint16 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 2 <= size)
        {
            static_copy<2>((byte *)(addr) + position, &value);
            position += 2;
            chars_converted = 2;
        }
        else chars_converted = 0;
    }
    else
    {
       uint64 result;
       *this >> result;
       value = uint16(result);
    }
}

void string_stream::operator>>(int32 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 4 <= size)
        {
            static_copy<4>((byte *)(addr) + position, &value);
            position += 4;
            chars_converted = 4;
        }
        else chars_converted = 0;
    }
    else
    {
       int64 result;
       *this >> result;
       value = int32(result);
    }
}

void string_stream::operator>>(uint32 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 4 <= size)
        {
            static_copy<4>((byte *)(addr) + position, &value);
            position += 4;
            chars_converted = 4;
        }
        else chars_converted = 0;
    }
    else
    {
       uint64 result;
       *this >> result;
       value = uint32(result);
    }
}

void string_stream::operator>>(int64 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 8 <= size)
        {
            static_copy<8>((byte *)(addr) + position, &value);
            position += 8;
            chars_converted = 8;
        }
        else chars_converted = 0;
    }
    else
    {
        uint64 number_position;
        char32 ch;
        chars_skiped = 0;
        chars_converted = 0;
        while(position != size)
        {
            ch = current_char();
            if(ch == U'-' && position + 1 != size)
            {
                position++;
                ch = current_char();
                if(ch >= U'0' && ch <= U'9')
                {
                    position--;
                    break;
                }
                position--;
            }
            else if(ch >= U'0' && ch <= U'9') break;
            position++;
            chars_skiped++;
        }
        number_position = position;
        if(position != size && current_char() == U'-')
        {
            position++;
            chars_converted++;
        }
        while(position != size)
        {
            ch = current_char();
            if(ch < U'0' || ch > U'9') break;
            position++;
            chars_converted++;
        }
        if(format == string_format::u32)
            string_line<char32>((char32 *)(addr) + number_position, position - number_position) >> value;
    }
}

void string_stream::operator>>(uint64 &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 8 <= size)
        {
            static_copy<8>((byte *)(addr) + position, &value);
            position += 8;
            chars_converted = 8;
        }
        else chars_converted = 0;
    }
    else
    {
        uint64 number_position;
        char32 ch;
        chars_skiped = 0;
        chars_converted = 0;
        while(position != size)
        {
            ch = current_char();
            if(ch >= U'0' && ch <= U'9') break;
            position++;
            chars_skiped++;
        }
        number_position = position;
        while(position != size)
        {
            ch = current_char();
            if(ch < U'0' || ch > U'9') break;
            position++;
            chars_converted++;
        }
        if(format == string_format::u32)
            string_line<char32>((char32 *)(addr) + number_position, position - number_position) >> value;
    }
}

void string_stream::operator>>(real &value)
{
    if(mode == parse_mode::binary)
    {
        chars_skiped = 0;
        if(position + 8 <= size)
        {
            static_copy<8>((byte *)(addr) + position, &value);
            position += 8;
            chars_converted = 8;
        }
        else chars_converted = 0;
    }
    else
    {
        uint64 number_position;
        char32 ch;
        chars_skiped = 0;
        chars_converted = 0;
        while(position != size)
        {
            ch = current_char();
            if(ch == U'-' && position + 1 != size)
            {
                position++;
                ch = current_char();
                if(ch >= U'0' && ch <= U'9')
                {
                    position--;
                    break;
                }
                position--;
            }
            else if(ch >= U'0' && ch <= U'9') break;
            position++;
            chars_skiped++;
        }
        number_position = position;
        if(position != size && current_char() == U'-')
        {
            position++;
            chars_converted++;
        }
        while(position != size)
        {
            ch = current_char();
            if(ch < U'0' || ch > U'9') break;
            position++;
            chars_converted++;
        }
        if(position + 1 < size && current_char() == U'.')
        {
            position++;
            ch = current_char();
            if(ch >= U'0' && ch <= U'9')
            {
                chars_converted++;
                while(position != size)
                {
                    ch = current_char();
                    if(ch < U'0' || ch > U'9') break;
                    position++;
                    chars_converted++;
                }
            }
            else position--;
        }
        if(format == string_format::u32)
            string_line<char32>((char32 *)(addr) + number_position, position - number_position) >> value;
    }
}*/
