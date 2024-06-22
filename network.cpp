#include "network.h"
#include "parser.h"
#include "os_api.h"

bool http_request::setup(const char32 *source)
{
	/*clear();
	uint64 p = 0, e = 0;
	string str;
	while(e < source.length && source.addr[e] != '\r') e++;
	if(e + 3 >= source.length || source.addr[e + 1] != '\n') return false;
	string_stream pr(source.addr + p, e - p, parse_mode::text, string_format::u8);
	pr >> str;
	if(compare_string_lines<char32>(string_line<char32>(str), string_line<char32>(U"GET")) == compare_result::equal)
		method = http_method::get;
	else if(compare_string_lines<char32>(string_line<char32>(str), string_line<char32>(U"POST")) == compare_result::equal)
		method = http_method::post;
	else return false;
	if(method == http_method::get)
		pr >> query;
	pr >> version;
	p = e + 2;
	while(true)
	{
		if(source.addr[p] == '\r' && source.addr[p + 1] == '\n') break;
		p += 2;
		e = p;
		while(e < source.length && source.addr[e] != '\r') e++;
		if(e + 3 >= source.length || source.addr[e + 1] != '\n') return false;
		pr = string_stream(source.addr + p, e - p, parse_mode::text, string_format::u8);
		pr >> str;
		if(compare_string_lines<char32>(string_line<char32>(str), string_line<char32>(U"Content-type:")) == compare_result::equal)
		{
			if(pr.position < pr.size) pr.position++;
			else return false;
			content_type << string_line<char8>((char8 *)(pr.addr) + pr.position, pr.size - pr.position);
		}
		p += 2;
	}
	return true;*/
	return true;
}

void http_request::clear()
{
	query.reset();
	version.reset();
	content_type.reset();
}

void http_response::compose(array<byte> *target)
{
	for(uint64 i = 0; i < version.size; i++)
		target->push(byte(version.addr[i]));
	target->push(byte(U' '));
	for(uint64 i = 0; i < status_code.size; i++)
		target->push(byte(status_code.addr[i]));
	target->push(byte(U' '));
	for(uint64 i = 0; i < status_message.size; i++)
		target->push(byte(status_message.addr[i]));
	target->push(byte(U'\r'));
	target->push(byte(U'\n'));
	if(content_type.size != 0)
	{
		constexpr const char8 content_type_key[] = "Content-type: ";
		target->insert_range(
			target->size,
			(byte *)(content_type_key),
			(byte *)(content_type_key) + array_size(content_type_key) - 1);
		for(uint64 i = 0; i < content_type.size; i++)
			target->push(byte(content_type.addr[i]));
		target->push(byte(U'\r'));
		target->push(byte(U'\n'));
	}
	if(data.size != 0)
	{
		constexpr const char8 content_type_key[] = "Content-Length: ";
		target->insert_range(
			target->size,
			(byte *)(content_type_key),
			(byte *)(content_type_key) + array_size(content_type_key) - 1);
		string content_length;
		content_length << data.size;
		for(uint64 i = 0; i < content_length.size; i++)
			target->push(byte(content_length.addr[i]));
	}
	target->push(byte(U'\r'));
	target->push(byte(U'\n'));
	target->push(byte(U'\r'));
	target->push(byte(U'\n'));
	if(data.size != 0)
	{
		target->insert_range(
			target->size,
			data.addr,
			data.addr + data.size);
	}
}

network_server::network_server()
{
	state = network_server_state::inactive;
}

network_server::~network_server()
{

}

void network_server::run()
{
	os_regiser_web_server(this);
}

void network_server::close()
{
	os_unregister_web_server(this);
}
