#pragma once
#include "string.h"

enum struct network_server_state
{
	inactive,
	running
};

struct ip_address
{
	uint32 v1;
	uint32 v2;
	uint32 v3;
	uint32 v4;

	ip_address() {}

	ip_address(uint32 v1, uint32 v2, uint32 v3, uint32 v4)
		: v1(v1), v2(v2), v3(v3), v4(v4) {}
};

enum struct http_method
{
	get,
	post
};

struct http_request
{
	http_method method;
	string query;
	string version;
	string content_type;

	bool setup(const char32 *source);
	void clear();
};

struct http_response
{
	string version;
	string status_code;
	string status_message;
	string content_type;
	array<byte> data;

	void compose(array<byte> *target);
};

struct network_connection
{
	void *handler;
};

struct network_server
{
	network_server_state state;
	ip_address ip;
	uint16 port;
	bool (*process_request)(
		byte *input,
		uint64 input_size,
		byte **output,
		uint64 *output_size);
	void *handler;

	network_server();
	~network_server();
	void run();
	void close();
};
